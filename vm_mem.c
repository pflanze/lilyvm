/*
  Copyright (C) 2021 Christian Jaeger, <ch@christianjaeger.ch>
  See the LICENSE file that comes bundled with this file.
*/
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "chj-64lib/util.h"
#include "chj-64lib/debug.h"
#include "chj-64lib/assert.h"
#include "chj-ctest/test.h"
#include "errors.h"
#include "cmp.h"
#include "vm_mem.h"
#include "vm_process.h"

#if DEBUG_MEM_SET
# include <string.h>
#endif


// ------------------------------------------------------------------
// Tests for vm_mem.h

static val scm_print_in_binary(val v) {
    unsigned char i = 0;
    unsigned char j;

    putchar('[');
    while (1) {
        for (j=0; j<8; j++) {
            putchar((v & WORD_SIGNBIT_MASK) ? '1' : '0');
            v <<= 1;
        }
        i += j;
        if (i < WORD_BITS) {
            putchar(' ');
        } else {
            putchar(']');
            return VOID;
        }
    }
}

#define PRINTLN_BINARY(e) do { scm_print_in_binary(e);  \
        printf(" = %s \r\n", #e); } while(0)


TEST(immediates) {
    // even though the only place that uses `process` is WRITELN
    LET_NEW_VM_PROCESS(process, 200, /* stacklen */ 200 /* heaplen */);
    val a;
    ASSERT_EQ(PCNUM(0), IMMEDIATE_KIND_PCNUM+1);
    ASSERT_EQ(PCNUM(1), IMMEDIATE_KIND_PCNUM+1+2);
    a = PCNUM(243);
    /* PRINTLN_BINARY(a); */
    /* PRINTLN_BINARY(FIXNUM_SIGN_MASK); */
    /* PRINTLN_BINARY(FIXNUM_FROM_INT_MASK); */
    ASSERT_EQ(is_pcnum(a), true);
    ASSERT_EQ(is_fixnum(a), false);
    ASSERT_EQ_(val, PCNUM_TO_WORD(a), 243);
    a = PCNUM(2432);
    ASSERT_EQ_(val, PCNUM_TO_WORD(a), 2432);

    /* PRINTLN_BINARY(fixnum_is_negative(BLA)); */
    /* PRINTLN_BINARY(FIXNUM_SIGN_MASK); */
    /* PRINTLN_BINARY(fixnum_is_negative(FIXNUM_SIGN_MASK)); */
    /* PRINTLN_BINARY(FIX(-2)); */
    /* PRINTLN_BINARY(WORD_FIXADDINT_MASK); */

    ASSERT_EQ(is_fixnum(FIX(1)), 1);
    ASSERT_EQ(is_fixnum(FIX(-2)), 1);
    ASSERT_EQ(is_fixnum(1), 1);
    ASSERT_EQ(is_fixnum(2), 0);
    ASSERT_EQ(is_fixnum(CHAR(1)), 0);
    ASSERT_EQ(is_char(CHAR(1)), 1);
    ASSERT_EQ(is_char(FIX(1)), 0);
    ASSERT_EQ(fixnum_is_negative(FIX(0)), 0);
    ASSERT_EQ(fixnum_is_negative(FIX(1)), 0);
    ASSERT_EQ(fixnum_is_negative(FIX(256)), 0);
    ASSERT_EQ_(val, fixnum_is_negative(FIX(-1)), 1);
    ASSERT_EQ_(val, fixnum_is_negative(FIX(-1000)), 1);

    /* printf("WORD_SIGNBIT_MASK=%i\r\n", WORD_SIGNBIT_MASK); */
    /* PRINTLN_BINARY(123); */
    /* PRINTLN_BINARY(17398); */
    /* PRINTLN_BINARY(INT(FIX(22))); */
    /* PRINTLN_BINARY(CHAR('A')); */
    /* PRINTLN_BINARY(ORD(CHAR('A'))); */
    /* PRINTLN_BINARY(CHAR('a')); */
    /* PRINTLN_BINARY(ORD(CHAR('a'))); */
    ASSERT_EQ(ORD(CHAR('A')), 'A');
    ASSERT_EQ(ORD(CHAR('a')), 'a');
    ASSERT_EQ(is_char(CHAR('a')), 1);

    /* PRINTLN_BINARY(FIX(22)); */
    /* PRINTLN_BINARY(INT(FIX(22))); */
    /* PRINTLN_BINARY(-2); */
    /* PRINTLN_BINARY(FIX(-2)); */
    /* PRINTLN_BINARY(INT(FIX(-2))); */
    a = FIX(22);
    ASSERT_EQ(INT(a), 22);
    ASSERT_EQ(INT(FIX(22)), 22);
    ASSERT_EQ_(val, INT(FIX(-2)), -2);
    /* WRITELN(a); */

    FREE_VM_PROCESS(process);
}

TEST(IS_IN_FIX_RANGE) {
    // these will fail if WORD is not 16 bits wide:
    //                             kksxxxxxxxxxxxxi
    //                             0b0111111111111
    ASSERT_EQ_(int16_t, FIXNUM_MAXINT, 4095);
    ASSERT_EQ_(int16_t, FIXNUM_MININT, -4096);
    // /16 bit
    ASSERT_EQ_(val, IS_IN_FIX_RANGE(10), true);
    ASSERT_EQ(IS_IN_FIX_RANGE(-10), true);
    ASSERT_EQ(IS_IN_FIX_RANGE(1000), true);
    ASSERT_EQ(IS_IN_FIX_RANGE(-1000), true);
    ASSERT_EQ(IS_IN_FIX_RANGE(2000), true);
    ASSERT_EQ(IS_IN_FIX_RANGE(-2000), true);
    ASSERT_EQ(IS_IN_FIX_RANGE(4000), true);
    ASSERT_EQ(IS_IN_FIX_RANGE(-4000), true);
    ASSERT_EQ(IS_IN_FIX_RANGE(8000), false);
    ASSERT_EQ(IS_IN_FIX_RANGE(-8000), false);
    ASSERT_EQ(IS_IN_FIX_RANGE(16000), false);
    ASSERT_EQ(IS_IN_FIX_RANGE(-16000), false);
    ASSERT_EQ(IS_IN_FIX_RANGE(30000), false);
    ASSERT_EQ(IS_IN_FIX_RANGE(-30000), false);
    ASSERT_EQ(IS_IN_FIX_RANGE(128000), false);
    ASSERT_EQ(IS_IN_FIX_RANGE(-128000), false);
}

// ------------------------------------------------------------------

int vm_errno; // want elsewhere or not at all? currently used by mem_s.s



/* INLINE */ static
void _gc_handle_slot_move(struct vm_process* process,
                          word_t *slot,
                          word_t **_newptr) {
    val v = *slot;
    word_t *p = ALLOCATED_PTR_FROMSPACE(v);
    numwords_t len = PTR_NUMWORDS(p);
    if (len) {
        ASSERT(len < 30); //XX
        *_newptr -= (len + 1);
        memcpy(*_newptr, p, (len + 1) * sizeof(word_t));
        {
            val vnew = ALLOCATED_FROM_POINTER_TOSPACE(*_newptr);
            PTR_NUMWORDS(p) = 0; // "forwarded"
            p[1] = vnew;
            *slot = vnew;
        }
        process->gc_moves++;
    } else {
        // moved already
        *slot = p[1];
    }
}

// keep hot path in a macro (cc65 does not support "inline")
#define GC_HANDLE_SLOT(slot, newptr)                            \
    if (is_allocated(slot)) {                                   \
        _gc_handle_slot_move(process, &(slot), &(newptr));      \
    }

void vm_mem_gc(struct vm_process* process) {
    // WARN("*** running gc");
    uintptr_t sz = process->alloc_size;
    word_t* newarea = process->alloc_area_fresh;
    word_t* newptr = add_word_and_bytes(newarea, sz);
    // GC root sets: walk them, copy non-moved allocated objects to
    // the new space. Update pointers at the same time.
    {
        // The registers
        GC_HANDLE_SLOT(process->A, newptr);
        GC_HANDLE_SLOT(process->B, newptr);
    }
    {
        // The root stacks
        uint8_t i;
        for (i = 0; i < process->num_val_roots; i++) {
            GC_HANDLE_SLOT(*process->val_roots[i], newptr);
        }
        for (i = 0; i < process->num_ptr_roots; i++) {
            word_t **pp = process->ptr_roots[i];
            val v = ALLOCATED_FROM_POINTER((*pp) - 1);
            GC_HANDLE_SLOT(v, newptr);
            {
                word_t *p = ALLOCATED_PTR_TOSPACE(v);
                *(process->ptr_roots[i]) = p + 1;
            }
        }
    }
    {
        // The execution stack
        uint16_t i;
        for (i = 0; i < process->stack.sp; i++) {
            GC_HANDLE_SLOT(process->stack.vals[i], newptr);
        }
    }
    {
        // Walk the to-space, copy nonforwarded objects, too (and
        // update pointers), until reaching allocation pointer.  Since
        // we're allocating downwards (but putting the object heads at
        // the downward end), we need to walk from the allocation
        // pointer upwards until the end, then walk from the new
        // allocation pointer upwards to the old start again until
        // there's nothing left.
        word_t *old_start;
        word_t *ptr;
        word_t *end = add_word_and_bytes(newarea, sz);
        while (1) {
            old_start = newptr;
            ptr = newptr;
            while (ptr < end) {
                numwords_t len = PTR_NUMWORDS(ptr);
                ASSERT (len); // those are the copies, they can't be fwd!
                if (! TYPE_IS_BINARY(PTR_TYPE(ptr))) {
                    // walk object body
                    uint16_t i;
                    for (i = 1; i < len; i++) {
                        GC_HANDLE_SLOT(ptr[i], newptr);
                    }
                }
                ptr += len + 1;
            }
            if (newptr < old_start) {
                end = old_start;
            } else {
                break;
            }
        }
    }
    {
        // Flip region pointers; we don't do this *before* running the
        // GC, since ALLOCATED_FROM_POINTER_TOSPACE and
        // ALLOCATED_PTR_FROMSPACE are expecting the old values.
        word_t* old = process->alloc_area;
        process->alloc_area = newarea;
        process->alloc_area_fresh = old;
    }
    process->alloc_ptr = newptr;
    process->gc_count++;
}


// The fast path is in `vm_process_alloc`; the slow path (mostly a
// copy-paste of the fast path!):
word_t* _vm_process_alloc_slow(struct vm_process* process, numwords_t n) {
    vm_mem_gc(process);
    word_t *ptr = process->alloc_ptr;
    ptr -= n;
    if (((uintptr_t)ptr) <= ((uintptr_t)process->alloc_area)) {
        DIE("out of memory");
    } else {
        process->alloc_ptr = ptr;
#if DEBUG_MEM_SET
        memset(ptr, 0x99, n * sizeof(word_t));
#endif
        return ptr;
    }
}



#ifdef __C64__
val fastcons(val a, val b);
#endif

val scm_cons(struct vm_process *process, val a, val b) {
    if (1 //XX fastcons needs updates
        || !IS_C64) {
        word_t* p= vm_process_alloc(process, 3); // header, 2 fields
        p[0] = HEAD_PAIR;
        p[1] = a;
        p[2] = b;
        return ALLOCATED_FROM_POINTER(p);
    } else {
#ifdef __C64__
        return fastcons(a, b);
#endif
    }
}

val pp_through(struct vm_process* process, val v) {
    SCM_WRITE(v);
    newline();
    return v;
}

val fixmulint_to_scm(struct vm_process* process, fixmulint_t x) {
    IFVERBOSE(printf("fixmulint_to_scm(%" PRIi32 ") = ", x));
    if (IS_IN_FIX_RANGE(x)) {
        IFVERBOSE(printf("fixnum "));
        return IFVERBOSE(PP_THROUGH)(FIX(x));
    } else {
        // x is 2 words wide, but upper word might be empty
        if (// positive representable in LO
            ((DWORD_HI(x) == 0) &&
             !WORD_IS_NEGATIVE(DWORD_LO(x)))
            ||
            // negative representable in LO
            ((DWORD_HI(x) == WORD_MAX) &&
             WORD_IS_NEGATIVE(DWORD_LO(x)))) {
            IFVERBOSE(printf("bignum 1 "));
            return IFVERBOSE(PP_THROUGH)(FIXADDINT_TO_BIGNUM(DWORD_LO(x)));
        } else {
            word_t *p = vm_process_alloc(process, 3); // header, 2 fields
            p[0] = HEAD_OF_LEN_TYPE(2, TYPE_BIGNUM);
            p[1] = DWORD_LO(x);
            p[2] = DWORD_HI(x);
            IFVERBOSE(printf("bignum 2 "));
            return IFVERBOSE(PP_THROUGH)(ALLOCATED_FROM_POINTER(p));
        }
    }
}


static NORETURN error_integer(struct vm_process* process, val v) {
    WRITELN(v);
    ERROR("not an integer: %i", v);
}
#define ERROR_INTEGER(v) error_integer(process, v)

val scm_inc(struct vm_process* process, val x) {
    if (is_fixnum(x)) {
        return FIXADDINT_TO_SCM(INT(x) + 1);
    } else if (IS_BIGNUM(x)) {
        UNFINISHED;
    } else {
        ERROR_INTEGER(x);
    }
}

val scm_dec(struct vm_process* process, val x) {
    if (is_fixnum(x)) {
        return FIXADDINT_TO_SCM(INT(x) - 1);
    } else if (IS_BIGNUM(x)) {
        UNFINISHED;
    } else {
        ERROR_INTEGER(x);
    }
}

// Tried a non-macro inline variant (see history) but oddly sometimes
// leads to larger code (in SMALL case), and oddly slower, too.
#define RETURN_BIGNUM_DISPATCH(x, y, fixnum_op, bignum_op)              \
    if (is_fixnum(x)) {                                                 \
        if (is_fixnum(y)) {                                             \
            return fixnum_op(process, x, y);                            \
        } else if (IS_BIGNUM(y)) {                                      \
            word_t xw = INT(x);                                           \
            return bignum_op(process,                                   \
                             &xw, 1, false,                             \
                             ALLOCATED_BODY(y), ALLOCATED_NUMWORDS(y), true); \
        } else {                                                        \
            ERROR_INTEGER(y);                                           \
        }                                                               \
    } else if (IS_BIGNUM(x)) {                                          \
        if (is_fixnum(y)) {                                             \
            word_t yw = INT(y);                                           \
            return bignum_op(process,                                   \
                             ALLOCATED_BODY(x), ALLOCATED_NUMWORDS(x), true, \
                             &yw, 1, false);                            \
        } else if (IS_BIGNUM(y)) {                                      \
            return bignum_op(process,                                   \
                             ALLOCATED_BODY(x), ALLOCATED_NUMWORDS(x), true, \
                             ALLOCATED_BODY(y), ALLOCATED_NUMWORDS(y), true); \
        } else {                                                        \
            ERROR_INTEGER(y);                                           \
        }                                                               \
    } else {                                                            \
        ERROR_INTEGER(x);                                               \
    }

// These return a scm boolean

// well, just re-use scm_eq (once I have it)?
static val fixnum_equal(UNUSED struct vm_process* process,
                        val a, val b) {
    return BOOL(a == b);
}
static val bignum_equal(UNUSED struct vm_process* process,
                        word_t *a, uint8_t lena, UNUSED bool amoves,
                        word_t *b, uint8_t lenb, UNUSED bool bmoves) {
    uint8_t i;
    if (lena != lenb) return FAL;
    for (i=0; i < lena; i++) {
        if (a[i] != b[i]) return FAL;
    }
    return TRU;
}
val scm_number_equal(struct vm_process* process, val x, val y) {
    RETURN_BIGNUM_DISPATCH(x, y, fixnum_equal, bignum_equal);
}


// These return a scm cmp (make this a dedicated type?):

static val fixnum_cmp(UNUSED struct vm_process* process,
                      val a, val b) {
    fixaddint_t x = INT(a);
    fixaddint_t y = INT(b);
    return x == y ? EQ : x < y ? LT : GT;
}
static val bignum_cmp(UNUSED struct vm_process* process,
                      word_t *a, uint8_t lena, UNUSED bool amoves,
                      word_t *b, uint8_t lenb, UNUSED bool bmoves) {
    if (lena == lenb) {

#define CHECK(t, a, b)                          \
        if (((t)(a)) < ((t)(b))) return LT;     \
        if (((t)(a)) > ((t)(b))) return GT;     \

        uint8_t i = lena - 1;
        CHECK(int16_t, a[i], b[i]);
        while (i) {
            i--;
            CHECK(uint16_t, a[i], b[i]);
        }
        return EQ;

#undef CHECK

    } else {
        bool neg_a = WORD_IS_NEGATIVE(a[lena-1]);
        bool neg_b = WORD_IS_NEGATIVE(b[lenb-1]);
        if (neg_a) {
            if (neg_b) {
                return lena < lenb ? GT : LT;
            } else {
                return LT;
            }
        } else {
            if (neg_b) {
                return GT;
            } else {
                return lena < lenb ? LT : GT;
            }
        }
    }
}
val scm_number_cmp(struct vm_process* process, val x, val y) {
    RETURN_BIGNUM_DISPATCH(x, y, fixnum_cmp, bignum_cmp);
}

TEST(scm_number_cmp) {
#define ASSERT_NUMBER_EQ(a, b)                    \
    ASSERT_EQ(SCM_NUMBER_CMP(a, b), EQ);          \
    ASSERT_EQ(SCM_NUMBER_EQUAL(a, b), TRU);
#define ASSERT_NUMBER_LT(a, b)                    \
    ASSERT_EQ(SCM_NUMBER_CMP(a, b), LT);          \
    ASSERT_EQ(SCM_NUMBER_EQUAL(a, b), FAL);
#define ASSERT_NUMBER_GT(a, b)                    \
    ASSERT_EQ(SCM_NUMBER_CMP(a, b), GT);          \
    ASSERT_EQ(SCM_NUMBER_EQUAL(a, b), FAL);
    
    LET_NEW_VM_PROCESS(process, 200, /* stacklen */ 200 /* heaplen */);
    {
        val a, b;
        a = FIXMULINT_TO_SCM(100);
        b = FIXMULINT_TO_SCM(101);
        ASSERT_NUMBER_EQ(a, a);
        ASSERT_NUMBER_LT(a, b);
        ASSERT_NUMBER_GT(b, a);
        a = FIXMULINT_TO_SCM(0);
        b = FIXMULINT_TO_SCM(-1);
        ASSERT_NUMBER_GT(a, b);
        ASSERT_NUMBER_LT(b, a);
        a = FIXMULINT_TO_SCM(5);
        b = FIXMULINT_TO_SCM(10000);
        ASSERT_NUMBER_LT(a, b);
        ASSERT_NUMBER_GT(b, a);
        a = FIXMULINT_TO_SCM(10000);
        ASSERT_EQ(ALLOCATED_NUMWORDS(a), 1);
        ASSERT_NUMBER_EQ(a, b);

        a = FIXMULINT_TO_SCM(5);
        b = FIXMULINT_TO_SCM(-10000);
        ASSERT_EQ(ALLOCATED_NUMWORDS(b), 1);
        ASSERT_NUMBER_GT(a, b);
        ASSERT_NUMBER_LT(b, a);
        a = FIXMULINT_TO_SCM(10000);
        ASSERT_NUMBER_GT(a, b);
        a = FIXMULINT_TO_SCM(-10000);
        ASSERT_NUMBER_EQ(a, b);

        a = FIXMULINT_TO_SCM(5);
        b = FIXMULINT_TO_SCM(-1000000);
        ASSERT_EQ(ALLOCATED_NUMWORDS(b), 2);
        ASSERT_NUMBER_GT(a, b);
        ASSERT_NUMBER_LT(b, a);
        a = FIXMULINT_TO_SCM(-1000000);
        ASSERT_NUMBER_EQ(a, b);
        b = FIXMULINT_TO_SCM(-1000001);
        ASSERT_NUMBER_GT(a, b);
        
        a = FIXMULINT_TO_SCM(1000000);
        ASSERT_EQ(ALLOCATED_NUMWORDS(a), 2);
        b = FIXMULINT_TO_SCM(1000001);
        ASSERT_NUMBER_LT(a, b);

        a = FIXMULINT_TO_SCM(10000);
        b = FIXMULINT_TO_SCM(10000000);
        ASSERT_NUMBER_LT(a, b);
        b = FIXMULINT_TO_SCM(-10000000);
        ASSERT_NUMBER_GT(a, b);
    }
    FREE_VM_PROCESS(process);
/* #undef ASSERT_NUMBER_EQ */
/* #undef ASSERT_NUMBER_LT */
/* #undef ASSERT_NUMBER_GT */
}

static val fixnum_add(struct vm_process *process, val a, val b) {
    return FIXADDINT_TO_SCM(((fixaddint_t)INT(a))
                            +
                            ((fixaddint_t)INT(b)));
}

bool trace = 0;
#ifdef DEBUG
# define TRACE(fmt, var)                        \
    if (trace) {                                \
        printf("%s = ", #var);                  \
        printf(fmt, var);                       \
        printf("\n");                           \
    }
# define TRACEu8(var) TRACE("%" PRIu8, var)
# define TRACEPTR(var) TRACE("%p", (void*)var)
#else
# define TRACE(fmt, var)
# define TRACEu8(var)
# define TRACEPTR(var)
#endif //DEBUG

static val bignum_add(struct vm_process *process,
                      word_t *a, uint8_t lena, bool amoves,
                      word_t *b, uint8_t lenb, bool bmoves) {
    uint8_t i;
    uint8_t len1; // len of the shorter
    uint8_t len2; // len of the longer
    uint8_t last_signs;
    bool need_overflow;
    word_t *z;
    word_t *r; // the longer of the two
    word_t sign_extension; // 0 for positive, 0xFFFF for negative
    dword_t A = 0;

    TRACEPTR(a);
    TRACEu8(lena);
    TRACEPTR(b);
    TRACEu8(lenb);
    if (lena < lenb) {
        len1 = lena;
        len2 = lenb;
        sign_extension = WORD_IS_NEGATIVE(a[lena-1]) ? ~0 : 0;
    } else {
        len1 = lenb;
        len2 = lena;
        r = a;
        sign_extension = WORD_IS_NEGATIVE(b[lenb-1]) ? ~0 : 0;
    }
    TRACEu8(len1);
    TRACEu8(len2);
    if (amoves) PTR_REGISTER(a);
    if (bmoves) PTR_REGISTER(b);
    z = vm_process_alloc(process, 1 + len2 + 1);
    // ^ head, data, data overflow reserve
    // No allocation from here on, thus:
    if (amoves+bmoves) PTR_UNREGISTER(amoves+bmoves);

    if (lena < lenb) {
        r = b;
    } else {
        r = a;
    }
    TRACEPTR(z);
    TRACEPTR(r);
    z++;
    TRACEPTR(z);
    ASSERT(len1>0);
    for (i=0; i<len1; i++) {
        A += a[i] + b[i];
        last_signs = WORD_IS_NEGATIVE(a[i]) + WORD_IS_NEGATIVE(b[i]);
        z[i] = DWORD_LO(A);
        A = DWORD_HI(A);
    }
    for (; i<len2; i++) {
        A += r[i] + sign_extension;
        last_signs = WORD_IS_NEGATIVE(r[i]) + WORD_IS_NEGATIVE(sign_extension);
        z[i] = DWORD_LO(A);
        A = DWORD_HI(A);
    }
    z--;
    switch (last_signs) {
    case 0:
        // * positive+positive: if the result became negative, we need
        //   a 0 overflow word. (the overflow bit is set, but we need
        //   the reverse.)
        need_overflow = WORD_IS_NEGATIVE(z[i]);
        sign_extension = 0;
        break;
    case 1:
        // * negative+positive: if the result becomes positive, it
        //   will also leave the overflow bit set. In that case we do
        //   *not* need an overflow word. If the result is negative,
        //   we also do *not* need an overflow word.
        need_overflow = 0;
        break;
    case 2:
        // * negative+negative will always leave overflow bit.
        //   Instead we know when to add an overflow word (FFFF) if
        //   the result is positive.
        need_overflow = !WORD_IS_NEGATIVE(z[i]);
        sign_extension = ~0;
        break;
    }
    if (need_overflow) {
        i++;
        z[i] = sign_extension;
    } else {
        // We might actually need to *reduce* the number length. The
        // result of an addition can be closer to 0 than the inputs!
        bool is_negative = WORD_IS_NEGATIVE(z[i]);
        sign_extension = is_negative ? ~0 : 0;
        z++; i--;
        while (i
               && (z[i] == sign_extension)
               && (WORD_IS_NEGATIVE(z[i-1]) == is_negative)) {
            i--;
        }
        if (i==0) {
            if (IS_IN_FIX_RANGE((signed_word_t)z[0])) {
                // XX future: free up z again instead of relying on
                // the GC
                return FIX(z[0]);
            }
        }
        z--; i++;
    }
    z[0] = HEAD_OF_LEN_TYPE(i, TYPE_BIGNUM);
    return ALLOCATED_FROM_POINTER(z);
}

val scm_add(struct vm_process* process, val x, val y) {
    RETURN_BIGNUM_DISPATCH(x, y, fixnum_add, bignum_add);
}

static void _assert_number_equal(struct vm_process* process,
                                 val a, val b,
                                 struct TestStatistics *stats,
                                 const char* file, int line) {
    if (SCM_NUMBER_EQUAL(a, b) == TRU) {
        stats->num_successes++;
    } else {
        stats->num_failures++;
        printf("* test failure in ASSERT_NUMBER_EQUAL(");
        SCM_WRITE(a);
        printf(", ");
        SCM_WRITE(b);
        printf(") on %s line %i\n", file, line);
    }
}
#define ASSERT_NUMBER_EQUAL(a, b) \
    _assert_number_equal(process, a, b, __test_stats, __FILE__, __LINE__)

TEST(scm_add) {
    LET_NEW_VM_PROCESS(process, 200, /* stacklen */ 2000 /* heaplen */);
    {
        val a, b, c;
        a = FIX(-10);
        b = FIX(19);
        ASSERT_NUMBER_EQUAL(SCM_ADD(a,b), FIX(9));
        b = FIX(-19);
        ASSERT_NUMBER_EQUAL(SCM_ADD(a,b), FIX(-29));

        ASSERT_NUMBER_EQUAL(FIXADDINT_TO_SCM(4000), FIX(4000));

        a = FIX(10);
        b = FIXMULINT_TO_SCM(10000);
        c = SCM_ADD(a, b);
        ASSERT_NUMBER_EQUAL(c, FIXMULINT_TO_SCM(10+10000));
        a = FIX(-10);
        c = SCM_ADD(a, b);
        ASSERT_NUMBER_EQUAL(c, FIXMULINT_TO_SCM(10000-10));
        c = SCM_ADD(c, FIX(10));
        ASSERT_NUMBER_EQUAL(c, FIXMULINT_TO_SCM(10000));
        c = SCM_ADD(c, c); // 20000
        ASSERT_NUMBER_EQUAL(c, FIXMULINT_TO_SCM(20000));
        c = SCM_ADD(c, c);
        ASSERT_NUMBER_EQUAL(c, FIXMULINT_TO_SCM(40000));
        c = SCM_ADD(c, c);
        ASSERT_NUMBER_EQUAL(c, FIXMULINT_TO_SCM(80000));
        c = FIXMULINT_TO_SCM(-1000);
        c = SCM_ADD(c, c);
        c = SCM_ADD(c, c);
        c = SCM_ADD(c, c);

        // positive overflow check:
        a = FIXMULINT_TO_SCM(4000);
        b = SCM_ADD(a, a);
        ASSERT_NUMBER_EQUAL(b, FIXMULINT_TO_SCM(8000));

        // continue with the negative numbers:
        
        trace = 1;
        c = SCM_ADD(c, c);
        c = SCM_ADD(c, c);
        c = SCM_ADD(c, c);
        c = SCM_ADD(c, c);
        ASSERT_NUMBER_EQUAL(c, FIXMULINT_TO_SCM(-128000));
        {
            uint8_t i;
            for (i=0; i<16; i++) {
                c = SCM_ADD(c, c);
            }
        }
        // todo: intercept or replace with binary comparison or so
        printf("-128000 << 16: "); SCM_WRITE(c); newline();

        c = SCM_ADD(FIXMULINT_TO_SCM(-128000), FIXMULINT_TO_SCM(128000));
        ASSERT_NUMBER_EQUAL(c, FIXMULINT_TO_SCM(0));
        ASSERT_EQ(is_fixnum(c), true);

        c = SCM_ADD(FIXMULINT_TO_SCM(-128001), FIXMULINT_TO_SCM(128000));
        ASSERT_NUMBER_EQUAL(c, FIXMULINT_TO_SCM(-1));
        ASSERT_EQ(is_fixnum(c), true);

        c = SCM_ADD(FIXMULINT_TO_SCM(-128001), FIXMULINT_TO_SCM(128002));
        ASSERT_NUMBER_EQUAL(c, FIXMULINT_TO_SCM(1));
        ASSERT_EQ(is_fixnum(c), true);
    }
    FREE_VM_PROCESS(process);
}

static val fixnum_mul(struct vm_process *process,
                      UNUSED val a, UNUSED val b) {
    return FIXMULINT_TO_SCM(INT(a) * INT(b));
}
static val bignum_mul(UNUSED struct vm_process *process,
                      UNUSED word_t *a, UNUSED uint8_t lena, UNUSED bool amoves,
                      UNUSED word_t *b, UNUSED uint8_t lenb, UNUSED bool bmoves) {
    UNFINISHED;
}


val scm_mul(struct vm_process *process, val x, val y) {
    RETURN_BIGNUM_DISPATCH(x, y, fixnum_mul, bignum_mul);
}

val scm_length(struct vm_process *process, val l) {
    val res = FIX(0);
    while (! is_null(l)) {
        if (IS_PAIR(l)) {
            res = SCM_INC(res);
            l = UNSAFE_CDR(l);
        } else {
            ERROR("not a pair or null: %" PRIx16, l);
        }
    }
    return res;
}

val scm_bitwise_and(UNUSED struct vm_process *process,
                    UNUSED val a, UNUSED val b) {
    // sigh and again crazy dispatch. hw do we do this efficiently and
    // generate efficiently SIGH ?
    DIE("not implemented yet");
}

// can't use #define PRINT puts
#define PRINT(s) printf("%s", s)


#define SCM_WRITE_BIGNUM_HEX(v) scm_write_bignum_hex(process, v)
static val scm_write_bignum_hex(struct vm_process *process, val v) {
    uint8_t i = ALLOCATED_NUMWORDS(v);
    word_t *p = ALLOCATED_BODY(v);
    printf("#bignum{");
    while (i) {
        i--;
        word_t d = p[i];
        printf("%04" PRIx16, d);
        if (i) {
            printf("-");
        }
    }
    printf("}");
    return VOID;
}

#define SCM_WRITE_NONNULL_LIST(v) SCM_WRITE_nonnull_list(process, v)
static val SCM_WRITE_nonnull_list(struct vm_process *process, val v) {
    // handle I/O and other errors!
    if (IS_PAIR(v)) {
        SCM_WRITE(UNSAFE_CAR(v));
        if (is_null(UNSAFE_CDR(v))) {
            PRINT(")");
            return VOID;
        } else {
            PRINT(" ");
            return SCM_WRITE_NONNULL_LIST(UNSAFE_CDR(v)); // TCO?
        }
    } else {
        // null case already handled in previous iteration
        PRINT(". ");
        SCM_WRITE(v);
        PRINT(")");
        return VOID;
    }
}


val scm_write(struct vm_process *process, val v) {
    if (is_allocated(v)) {
        type_t type = ALLOCATED_TYPE(v);
        if (type == TYPE_PAIR) {
            PRINT("(");
            return SCM_WRITE_NONNULL_LIST(v);
        } else if (type == TYPE_BIGNUM) {
            return SCM_WRITE_BIGNUM_HEX(v);
        } else {
            // WARN_("unknown type %i", type);
            printf("{object of unknown type %i}", type);
            BREAK();
        }
    } else if (is_null(v)) {
        PRINT("()");
    } else if (is_false(v)) {
        PRINT("#f");
    } else if (v == TRU) {
        PRINT("#t");
    } else if (is_void(v)) {
        PRINT("#!void");
    } else if (is_undef(v)) {
        PRINT("#!undefined");
    } else if (is_uninitialized(v)) {
        PRINT("#!uninitialized");
    } else if (is_fixnum(v)) {
        printf("%i", INT(v));
    } else if (is_pcnum(v)) {
        printf("#PC{%" PRIu16 "}", PCNUM_TO_WORD(v));
    } else if (is_char(v)) {
        // XX unicode ?
        printf("#\\%c", ORD(v));
    } else {
        PRINT("unknown type of: ");
        scm_print_in_binary(v);
        WARN_(" %x", v);
    }
    return VOID;
}

TEST(t1) {
    LET_NEW_VM_PROCESS(process, 200, /* stacklen */ 200 /* heaplen */);
    {
        val v, v2;
        v = CONS(CHAR('H'),
                 CONS(CHAR('e'),
                      CONS(CHAR('l'),
                           NIL)));

        v2 = CONS(v, FIX(1234));
        ASSERT_EQ_(val, UNSAFE_CAR(v2), v);
        ASSERT_EQ(INT(UNSAFE_CDR(v2)), 1234);
        WRITELN(v2);
        WRITELN(UNSAFE_CAR(v2));
        ASSERT_EQ(SCM_LENGTH(v), FIX(3));

        ASSERT_EQ_(val, SCM_INC(FIX(12)), FIX(13));
        // ASSERT_EQ_(val, scm_inc(FIX(-3333)), FIX(-3332));
        // ^ already out of range--XX?
        ASSERT_EQ_(val, SCM_INC(FIX(-333)), FIX(-332));
        ASSERT_EQ_(val, SCM_INC(FIX(-1)), FIX(0));
        // ASSERT_EQ_(val, scm_inc(CHAR(12)), 13);  "not an integer", OK
    }
    FREE_VM_PROCESS(process);
}

