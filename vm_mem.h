/*
  Copyright (C) 2021 Christian Jaeger, <ch@christianjaeger.ch>
  See the LICENSE file that comes bundled with this file.
*/
#ifndef MEM_H_
#define MEM_H_

/*
  Memory management:

  `val` is either a pointer or an immediate according to the following
  bit pattern (which assumes `word_t` from vm_mem_config.h to be 16
  bits):

     hi        lo
  kksx'xxxx xxxx'xxxi
  i: 1 = immediate, 0 = allocated (value is directly a pointer)
  kk = immediate kind
  fixnum:  00sx'xxxx xxxx'xxx1  s,x = sign, rest of value
  pcnum:   01xx'xxxx xxxx'xxx1  x = value, offset from base of program space
                                     (^ XX: currently is only an offset on posix)
  char:    10xx'xxxx xxxx'xxx1  x = value
  special: 11xx'xxxx xxxx'xxx1  x = value

  Structure of objects allocated in the movable storage (small ones
  in first generation?) (note that they always have an even length in
  bytes, and always start on an even address):

  byte 0: type
  byte 1: numwords (length of *data* in words, excl. head);
          0 == moved, first data word is the pointer to the new location
    ... : data words
*/

#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include "chj-64lib/assert.h"
#include "chj-64lib/util.h"
#include "vm_process.h"
#include "vm_mem_config.h"


#define WORD_BITS (sizeof(word_t) * 8)
#define WORD_SIGNBIT_MASK (1 << (WORD_BITS-1))
/* ^ what for? NOT the fixnum sign bit! */

struct dword_words {
    word_t word[2];
};

#ifdef __C64__
// This *might* be faster with cc65 than the bit handling? But with
// gcc -O2 and warnings it will warn about it breaking
// strict-aliasing.
# define DWORD_LO(x) ((struct dword_words*)&(x))->word[0]
# define DWORD_HI(x) ((struct dword_words*)&(x))->word[1]
#else
# define DWORD_LO(x) ((word_t)(x))
# define DWORD_HI(x) ((word_t)((x) >> WORD_BITS))
#endif

#define WORD_BYTE0(w) ((word_t)(w) & 255)
#define WORD_BYTE1(w) ((word_t)(w) >> 8)

#define WORD_MAX 0xFFFF
#define WORD_SIGN_MASK 0x8000
#define WORD_IS_NEGATIVE(x) (((x) & WORD_SIGN_MASK) ? 1 : 0)

#define PTR_TYPE(p) ((unsigned char*)(p))[0]
#define WORD_TYPE(w) WORD_BYTE0(w)

#define PTR_NUMWORDS(p) ((unsigned char*)(p))[1]
#define WORD_NUMWORDS(w) WORD_BYTE1(w)



/* "ALLOCATED" is the subset of type `val` that represents an
   allocated object */

# ifdef __C64__

#  define ALLOCATED_FROM_POINTER(ptr) ((val)(p))
#  define ALLOCATED_PTR(allocated) ((word_t*)(allocated))

# else

// For secure processes I'd want to implement it that way on the C64,
// too. (In FPGA it could do memory ownership control instead, or use
// an MMU.)

// NOTE: these unhygienically access a variable `process` in their
// context!
#define ALLOCATED_FROM_POINTER(ptr) allocated_from_pointer(process, ptr)
#define ALLOCATED_PTR(allocated) allocated_ptr(process, allocated)

INLINE static
val allocated_from_pointer(struct vm_process *process,
                           word_t *ptr) {
    uintptr_t d = ((uintptr_t)ptr) - ((uintptr_t)process->alloc_area);
    ASSERT(d < process->alloc_size);
    ASSERT(d); // since the null pointer is reserved as error signal
#if DEBUG_MEM_DEBUG
    WARN_("ALLOCATED_FROM_POINTER(%p) = %" PRIu16, (void*)ptr, (val)d);
#endif
    return d;
}

INLINE static
word_t *allocated_ptr(struct vm_process *process,
                    val allocated) {
    word_t *res= (word_t*)(((uintptr_t)allocated) + (uintptr_t)process->alloc_area);
    ASSERT(allocated);
    ASSERT(IS_EVEN(allocated));
    ASSERT(((uintptr_t)allocated) < process->alloc_size);
    ASSERT(PTR_NUMWORDS(res)); // it has not been moved
#if DEBUG_MEM_DEBUG
    WARN_("ALLOCATED_PTR(%" PRIu16 ") = %p", allocated, (void*)res);
#endif
    return res;
}


//XX  LET_ALLOCATED_PTR(var, allocated) with auto register?

// Register variables as roots for GC: (this both keeps the value
// alife, as well as updates the registered variable)
#define VAL_REGISTER(var) vm_process_register_val_root(process, &var)
#define VAL_UNREGISTER(n) vm_process_unregister_val_roots(process, n)
#define PTR_REGISTER(var) vm_process_register_ptr_root(process, &var)
#define PTR_UNREGISTER(n) vm_process_unregister_ptr_roots(process, n)

// During GC:

//  the same, except for *not* checking that it hasn't moved:
INLINE static
word_t *allocated_ptr_fromspace(struct vm_process *process,
                              val allocated) {
    word_t *res= (word_t*)(((uintptr_t)allocated) + (uintptr_t)process->alloc_area);
    ASSERT(allocated);
    ASSERT(IS_EVEN(allocated));
    ASSERT(((uintptr_t)allocated) < process->alloc_size);
    // do NOT: ASSERT(PTR_NUMWORDS(res)); // it has not been moved
#if DEBUG_MEM_DEBUG
    WARN_("ALLOCATED_PTR(%" PRIu16 ") = %p", allocated, (void*)res);
#endif
    return res;
}
#define ALLOCATED_PTR_FROMSPACE(allocated) allocated_ptr_fromspace(process, allocated)

//  we need to refer to the other base:
INLINE static
val allocated_from_pointer_tospace(struct vm_process *process,
                                   word_t *ptr) {
    uintptr_t d = ((uintptr_t)ptr) - ((uintptr_t)process->alloc_area_fresh);
    ASSERT(d < process->alloc_size);
    ASSERT(d); // since the null pointer is reserved as error signal
#if DEBUG_MEM_DEBUG
    WARN_("ALLOCATED_FROM_POINTER_TOSPACE(%p) = %" PRIu16, (void*)ptr, (val)d);
#endif
    return d;
}
#define ALLOCATED_FROM_POINTER_TOSPACE(ptr) allocated_from_pointer_tospace(process, ptr)


INLINE static
word_t *allocated_ptr_tospace(struct vm_process *process,
                            val allocated) {
    word_t *res= (word_t*)(((uintptr_t)allocated)
                       + (uintptr_t)process->alloc_area_fresh);
    ASSERT(allocated);
    ASSERT(IS_EVEN(allocated));
    ASSERT(((uintptr_t)allocated) < process->alloc_size);
    ASSERT(PTR_NUMWORDS(res)); // it has not been moved
#if DEBUG_MEM_DEBUG
    WARN_("ALLOCATED_PTR_TOSPACE(%" PRIu16 ") = %p", allocated, (void*)res);
#endif
    return res;
}
#define ALLOCATED_PTR_TOSPACE(allocated) \
    allocated_ptr_tospace(process, allocated)


# endif


#define ALLOCATED_SLOT(allocated, i) ALLOCATED_PTR(allocated)[i]

word_t* _vm_process_alloc_slow(struct vm_process* process, numwords_t n);

INLINE static
word_t* vm_process_alloc(struct vm_process* process, numwords_t n) {
    word_t *ptr = process->alloc_ptr;
    ASSERT(ptr);
    ptr -= n;
    // Need to avoid allocating the last word, since that would give
    // the 0 val on non-C64 systems, which would clash with the error
    // value. Could avoid to over-allocate that word, but things will
    // change anyway.
    if (((uintptr_t)ptr) <= ((uintptr_t)process->alloc_area)) {
        return _vm_process_alloc_slow(process, n);
    } else {
        process->alloc_ptr = ptr;
#if DEBUG_MEM_SET
        memset(ptr, 0x99, n * sizeof(word_t));
#endif
        return ptr;
    }
}


#define IMMEDIATE_BIT_MASK 1

#define IMMEDIATE_KIND_MASK (3<<(WORD_BITS-2))
// [11000000 00000000]

#define IMMEDIATE_KIND_FIXNUM  (0<<(WORD_BITS-2))
#define IMMEDIATE_KIND_PCNUM   (1<<(WORD_BITS-2)) /* for jsr/ret */
#define IMMEDIATE_KIND_CHAR    (2<<(WORD_BITS-2))
#define IMMEDIATE_KIND_SPECIAL (3<<(WORD_BITS-2))

// #define IMMEDIATE_MASK (IMMEDIATE_KIND_MASK | 1)
// [11000000 00000001]

// special values
#define FAL   (IMMEDIATE_KIND_SPECIAL | 1 | (0<<1))
// [11000000 00000001] = FAL 
#define TRU   (IMMEDIATE_KIND_SPECIAL | 1 | (1<<1))
// [11000000 00000011] = TRU 
#define NIL   (IMMEDIATE_KIND_SPECIAL | 1 | (2<<1))
#define VOID  (IMMEDIATE_KIND_SPECIAL | 1 | (3<<1))
#define UNDEF (IMMEDIATE_KIND_SPECIAL | 1 | (4<<1))

// cmp 'type'/value group? good or horribly bad idea ? use for now,
// replace with symbols later?
#define LT    (IMMEDIATE_KIND_SPECIAL | 1 | (8<<1))
#define EQ    (IMMEDIATE_KIND_SPECIAL | 1 | (9<<1))
#define GT    (IMMEDIATE_KIND_SPECIAL | 1 | (10<<1))

// just the sign bit
#define FIXNUM_SIGN_MASK (1<<(WORD_BITS-3))
// [00100000 00000000] = FIXNUM_SIGN_MASK 

#define fixnum_is_negative(x) (((x) & FIXNUM_SIGN_MASK) ? 1 : 0)
#define FIXNUM_FROM_INT_MASK (FIXNUM_SIGN_MASK-1)
// [00011111 11111111] = FIXNUM_FROM_INT_MASK 

#define FIX(n)                                                          \
    ((((word_t)(n) & FIXNUM_FROM_INT_MASK) << 1) | 1)
#define PCNUM(n)                                                        \
    ((((word_t)(n) & FIXNUM_FROM_INT_MASK) << 1) | 1 | IMMEDIATE_KIND_PCNUM)

// the bits to OR with if negative
#define FIXNUM_TO_INT_NEGATIVE_BITS (((val)~0)<<(WORD_BITS-3))
// [11100000 00000000] = FIXNUM_TO_INT_NEGATIVE_BITS 
#define INT(v)                                                          \
    (int16_t)(((val)(v) >> 1)                                           \
              | ((((val)(v)) & FIXNUM_SIGN_MASK) << 2)                  \
              | (fixnum_is_negative(v) ? FIXNUM_TO_INT_NEGATIVE_BITS : 0))

#define PCNUM_TO_WORD(v)                                                \
    (word_t)(((val)(v) >> 1) & FIXNUM_FROM_INT_MASK)


#define CHAR(x) (((val)(x) << 1) | 1 | IMMEDIATE_KIND_CHAR)
#define ORD(x) (((val)(x) & ~IMMEDIATE_KIND_MASK) >> 1)

#define BOOL(x) ((x) ? TRU : FAL)

// These return a C, not Scheme, boolean  //XX use pointer 0 as Scheme false ?
#define is_null(x) ((x) == NIL)
#define is_false(x) ((x) == FAL)
#define is_trueish(x) (!is_false(x))
#define is_void(x) ((x) == VOID)
#define is_undef(x) ((x) == UNDEF)
#define is_fixnum(x)  (((x) & (IMMEDIATE_KIND_MASK | IMMEDIATE_BIT_MASK)) == (IMMEDIATE_KIND_FIXNUM | IMMEDIATE_BIT_MASK))
#define is_pcnum(x)  (((x) & (IMMEDIATE_KIND_MASK | IMMEDIATE_BIT_MASK)) == (IMMEDIATE_KIND_PCNUM | IMMEDIATE_BIT_MASK))
#define is_char(x)    (((x) & (IMMEDIATE_KIND_MASK | IMMEDIATE_BIT_MASK)) == (IMMEDIATE_KIND_CHAR | IMMEDIATE_BIT_MASK))
#define is_special(x) (((x) & (IMMEDIATE_KIND_MASK | IMMEDIATE_BIT_MASK)) == (IMMEDIATE_KIND_SPECIAL | IMMEDIATE_BIT_MASK))
#define is_immediate(x) ((x) & 1)
#define is_allocated(x) (!is_immediate(x))

/* An integer large enough to represent the result of fixnum + fixnum */
typedef int16_t fixaddint_t;
/* An integer large enough to represent the result of fixnum * fixnum;
   also see dword_t for unsigned variant. */
typedef int32_t fixmulint_t;
/*
  Is is never useful? e.g. (FIXMULINT_HI(x) == WORD_MAX) is wrong,
  would have to compare against -1. But maybe that may be useful?  But
  it may often be context dependent (as e.g. in bignum calculations)
  whether a word is to be treated signed or not. Better cast it there?
*/
/*
struct fixmulint_words {
    int16_t word[2];
};
#define FIXMULINT_LO(x) ((struct fixmulint_words*)&(x))->word[0]
#define FIXMULINT_HI(x) ((struct fixmulint_words*)&(x))->word[1]

Use DWORD_HI, DWORD_LO instead, or update the above in the same way now!
*/

#define WORD_FIXADDINT_MASK ((1 << (WORD_BITS-3))-1)
#define WORD_FIXNUM_MASK (WORD_FIXADDINT_MASK << 1)

// the max and min numbers that can be represented as fixnum (but
// those values are not fixnum val:s yet!)
#define FIXNUM_MAXINT (WORD_FIXADDINT_MASK>>1)
#define FIXNUM_MININT (-(FIXNUM_MAXINT+1))


typedef val head_t;
typedef unsigned char type_t;

#define HEAD_OF_LEN_TYPE(len,type) (((len)<<8) | type)

#define TYPE_IS_BINARY(t) ((t) & 128)

#define TYPE_PAIR 1
#define TYPE_BIGNUM 128

#define HEAD_PAIR HEAD_OF_LEN_TYPE(2, TYPE_PAIR)

#define HEAD(p) ALLOCATED_SLOT(p, 0)

#define ALLOCATED_TYPE(p) ((unsigned char*)ALLOCATED_PTR(p))[0]

// XX rename to ALLOCATED_LEN ?
#define ALLOCATED_NUMWORDS(v) ((unsigned char*)ALLOCATED_PTR(v))[1]

#define ALLOCATED_BODY(p) &(ALLOCATED_PTR(p)[1])


/* Predicates for allocated objects: */
INLINE static
bool is_pair(struct vm_process *process, val v) {
    return is_allocated(v) && (ALLOCATED_TYPE(v) == TYPE_PAIR);
}
#define IS_PAIR(v) is_pair(process, v)
INLINE static
bool is_bignum(struct vm_process *process, val v) {
    return is_allocated(v) && (ALLOCATED_TYPE(v) == TYPE_BIGNUM);
}
#define IS_BIGNUM(v) is_bignum(process, v)



#define CAR(p) ALLOCATED_SLOT(p, 1)
#define CDR(p) ALLOCATED_SLOT(p, 2)


// Need it for both fixaddint_t and fixmulint_t, thus use a macro:
#define IS_IN_FIX_RANGE(x) (((x) <= FIXNUM_MAXINT) && ((x) >= FIXNUM_MININT))

// (todo: easy de-macrofication? But anyway, only using it once each
// time anyway.)

INLINE static
val fixaddint_to_bignum(struct vm_process* process, fixaddint_t x) {
    word_t *p = vm_process_alloc(process, 2); // header, 1 field
    p[0] = HEAD_OF_LEN_TYPE(1, TYPE_BIGNUM);
    p[1] = x;
    return ALLOCATED_FROM_POINTER(p);
}
#define FIXADDINT_TO_BIGNUM(x) fixaddint_to_bignum(process, x)

INLINE static
val fixaddint_to_scm(struct vm_process* process, fixaddint_t x) {
    if (IS_IN_FIX_RANGE(x)) {
        return FIX(x);
    } else {
        return FIXADDINT_TO_BIGNUM(x);
    }
}
#define FIXADDINT_TO_SCM(x) fixaddint_to_scm(process, x)

val fixmulint_to_scm(struct vm_process* process, fixmulint_t x);
#define FIXMULINT_TO_SCM(x) fixmulint_to_scm(process, x)

val scm_cons(struct vm_process *process, val a, val b);
#define CONS(a, b) scm_cons(process, a, b)
val scm_write(struct vm_process *process, val v);
#define SCM_WRITE(v) scm_write(process, v)
val scm_length(struct vm_process *process, val l);
#define SCM_LENGTH(l) scm_length(process, l)

val scm_inc(struct vm_process* process, val x);
#define SCM_INC(x) scm_inc(process, x)
val scm_dec(struct vm_process* process, val x);
#define SCM_DEC(x) scm_dec(process, x)
val scm_add(struct vm_process* process, val x, val y);
#define SCM_ADD(x, y) scm_add(process, x, y)
val scm_mul(struct vm_process* process, val x, val y);
#define SCM_MUL(x, y) scm_mul(process, x, y)
val scm_bitwise_and(struct vm_process* process, val a, val b);
#define SCM_BITWISE_AND(x, y) scm_bitwise_and(process, x, y)
val scm_number_equal(struct vm_process* process, val x, val y);
#define SCM_NUMBER_EQUAL(x, y) scm_number_equal(process, x, y)
val scm_number_cmp(struct vm_process* process, val x, val y);
#define SCM_NUMBER_CMP(x, y) scm_number_cmp(process, x, y)

val pp_through(struct vm_process* process, val v);
#define PP_THROUGH(v) pp_through(process, v)

#define WRITELN(e) do { printf("%s = ", #e); SCM_WRITE(e); newline(); } while(0)


// #define POSITIVEFIXNUM_UNSAFE_DEC(n) FIX(INT(n) - 1)
#define POSITIVEFIXNUM_UNSAFE_DEC(n) ((n) - 2)


// XX only correct in 16-bit word_t case!
#define __test_assert_eq_fmt_failure_val __test_assert_eq_fmt_failure_uint16_t
#define __test_fmt_val __test_fmt_uint16_t
#define __test_t_for_val __test_t_for_uint16_t


#endif /* MEM_H_ */
