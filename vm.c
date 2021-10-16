/*
  Copyright (C) 2021 Christian Jaeger, <ch@christianjaeger.ch>
  See the LICENSE file that comes bundled with this file.
*/

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include "chj-64lib/util.h"
#include "chj-64lib/debug.h"
#include "chj-ctest/test.h"
#include "errors.h"
#include "vm.h"


#define ON_FAIL return 0

/*
INLINE static
bool stack_push(struct vm_stack *s, val val) {
    stacksize_t sp = s->sp;
    if (sp < s->len) {
        s->vals[sp] = val;
        s->sp++;
        return true;
    } else {
        FAIL2(s->failure, "stack_push", "stack is full");
    }
}

INLINE static
val stack_pop(struct vm_stack *s) {
    stacksize_t sp = s->sp;
    if (sp) {
        sp--;
        s->sp = sp;
        return s->vals[sp];
    } else {
        FAIL2(s->failure, "stack_pop", "stack is empty");
    }
}

INLINE static
val stack_last(struct vm_stack *s) {
    stacksize_t sp = s->sp;
    if (sp) {
        return s->vals[sp - 1];
    } else {
        FAIL2(s->failure, "stack_last", "stack is empty");
    }
}

INLINE static
void stack_last_unsafe_set(struct vm_stack *s, val v) {
    s->vals[s->sp - 1] = v;
}

INLINE static
bool stack_drop1(struct vm_stack *s) {
    stacksize_t sp = s->sp;
    if (sp) {
        sp--;
        s->sp = sp;
        return true;
    } else {
        FAIL2(s->failure, "stack_drop1", "stack is empty");
    }
}

INLINE static
val stack_ref(struct vm_stack *s, uint8_t i) {
    if (i < s->sp) {
        return s->vals[s->sp - 1 -i];
    } else {
        FAIL2(s->failure, "stack_ref", "index outside range");
    }
}

INLINE static
bool stack_set(struct vm_stack *s, uint8_t i, val v) {
    if (i < s->sp) {
        s->vals[s->sp - 1 -i] = v;
        return true;
    } else {
        FAIL2(s->failure, "stack_set", "index outside range");
    }
}

*/

/* INLINE */ static
bool stack_swap(struct vm_stack *s) {
    if (s->sp >= 2) {
        stacksize_t i = s->sp - 2;
        val tmp = s->vals[i];
        s->vals[i] = s->vals[i+1];
        s->vals[i+1] = tmp;
        return true;
    } else {
        FAIL2(s->failure, "stack_swap", "have fewer than 2 values");
    }
}

/* INLINE */ static
bool stack_dup(struct vm_stack *s) {
    stacksize_t sp = s->sp;
    if (sp) {
        if (sp < s->len) {
            s->vals[sp] = s->vals[sp-1];
            s->sp = sp+1;
            return true;
        } else {
            FAIL2(s->failure, "stack_dup", "stack is full");
        }
    } else {
        FAIL2(s->failure, "stack_dup", "stack is empty");
    }
}

/*
INLINE static
bool stack_ensure_has(struct vm_stack *s, stacksize_t n) {
    // ensure stack has at least n elements.
    if (s->sp >= n) return true;
    FAIL2(s->failure, "stack_ensure_has", "missing elements on stack");
}

INLINE static
bool stack_ensure_free(struct vm_stack *s, stacksize_t n) {
    // ensure stack has space for at least n more elements.
    // XX slight risk for number overflow
    if (s->sp < s->len - n) return true;
    FAIL2(s->failure, "stack_ensure_free", "not enough space on stack");
}

INLINE static
bool stack_alloc(struct vm_stack *s, stacksize_t n) {
    // Allocate n slots on the stack. CAREFUL: does not initialize
    // them!
    stacksize_t sp = s->sp + n; // XX risk for overflow
    if (sp <= s->len) {
        s->sp = sp;
        return true;
    }
    FAIL2(s->failure, "stack_ensure_free", "not enough space on stack");
}
*/

INLINE static
void stack_clear(struct vm_stack *s) {
    s->sp = 0;
}

#define VALARRAY_WRITE(vals, Tlen, len)         \
    {                                           \
        Tlen i;                                 \
        printf("(");                            \
        for (i=0; i < (len); i++) {             \
            if (i) printf(" ");                 \
            SCM_WRITE(DEREF((vals)[i]));        \
        }                                       \
        printf(")");                            \
    }

void vm_process_stack_write(struct vm_process *process) {
#define DEREF(x) x
    VALARRAY_WRITE(process->stack.vals, stacksize_t, process->stack.sp);
#undef DEREF
    // print the register stacks, too
    printf(" ");
#define DEREF(x) *(x)
    VALARRAY_WRITE(process->val_roots, uint8_t, process->num_val_roots);
#undef DEREF
    printf(" ");
#define DEREF(x) ALLOCATED_FROM_POINTER((*(x))-1)
    VALARRAY_WRITE(process->ptr_roots, uint8_t, process->num_ptr_roots);
#undef DEREF
}

void vm_process_registers_write(struct vm_process *process,
                                const char* separator) {
    printf("A=");
    SCM_WRITE(process->A);
    printf("%sB=", separator);
    SCM_WRITE(process->B);
    printf("%sC=", separator);
    SCM_WRITE(process->C);
    printf("%sD=", separator);
    SCM_WRITE(process->D);
}

void vm_process_stack_clear(struct vm_process *process) {
    stack_clear(&process->stack);
}

#ifdef VM_TRACE

const char *before = "           ";
const char *after = "\n";

static void vm_trace_dword(const char* name, dword_t val) {
    printf("%s"
           "%s=" FPRI_dword " " FPRI_signed_dword " " FPRIx_dword
           "%s",
           before,
           name, val, val, val,
           after);
}

static void vm_trace_word(const char* name, word_t val) {
    printf("%s"
           "%s=" FPRI_word " " FPRI_signed_word " " FPRIx_word
           "%s",
           before,
           name, val, val, val,
           after);
}

static void vm_trace(struct vm_process *process,
                     uintptr_t pcoffset,
                     const char *opcodename,
                     /* copies of non-GC'd registers: */
                     word_t M,
                     word_t N) {
    // Note: bytecode_load_and_run is printing very similar output
    printf("           ");
    vm_process_stack_write(process);
    printf("\n           ");
    vm_process_registers_write(process, ", ");
    printf("\n");
    vm_trace_word("M", M);
    vm_trace_word("N", N);
    printf("%3" PRIu16 " - %4" PRIuPTR " %s\n",
           process->stack.sp, pcoffset, opcodename);
}

#endif

struct vm_failure {
    struct vm_stack_failure *maybe_parent_failure;
    const char *message;
    const char *file;
    int line;
};

void vm_process_run(struct vm_process *process, uint8_t *code) {
    struct vm_failure failure;
    uint8_t *pc = code;

    // always the same parent failure, OK? So that can re-use FAIL
    // macro.
    failure.maybe_parent_failure = &process->stack.failure;

#define PC (pc - code)
#define SET_PC(i) pc = code + i
#define OPCODE pc[0]

#undef ON_FAIL
#define ON_FAIL goto on_error

#define PUSH(x)                                                         \
    if (SP < process->stack.len) {                                      \
        process->stack.vals[SP] = (x);                                  \
        SP_INC;                                                         \
    } else {                                                            \
        FAIL1(failure, "PUSH");                                         \
    }
#define LET_POP(varname)                                                \
    val varname;                                                        \
    if (SP) {                                                           \
        SP_DEC;                                                         \
        varname = process->stack.vals[SP];                              \
    } else {                                                            \
        FAIL1(failure, "LET_POP");                                      \
    }
#define LET_STACK_REF(varname,i)                                        \
    val varname;                                                        \
    {                                                                   \
        stacksize_t _i = (i);                                           \
        if (_i < SP) {                                                  \
            varname = process->stack.vals[SP - 1 - _i];                 \
        } else {                                                        \
            FAIL1(failure, "LET_STACK_REF");                            \
        }                                                               \
    }
#define LET_STACK_LAST(varname)                                         \
    val varname;                                                        \
    if (SP) {                                                           \
        varname = process->stack.vals[SP - 1];                          \
    } else {                                                            \
        FAIL1(failure, "LET_STACK_LAST");\
    }
#define STACK_SET(i,v)                                                  \
    do {                                                                \
        stacksize_t _i = (i);                                           \
        if (_i < SP) {                                                  \
            process->stack.vals[SP - 1 - _i] = (v);                     \
        } else {                                                        \
            FAIL1(failure, "LET_STACK_SET");                            \
        }                                                               \
    } while(0)
#define STACK_DROP1                                                     \
    if (SP) {                                                           \
        SP_DEC;                                                         \
    } else {                                                            \
        FAIL1(failure, "STACK_DROP1");                                  \
    }
#define STACK_SWAP                                                      \
    _STORESP;                                                           \
    if (! stack_swap(&process->stack)) FAIL1(failure, "STACK_SWAP");    \
    _RESTORESP;
#define STACK_DUP                                                       \
    _STORESP;                                                           \
    if (! stack_dup(&process->stack)) FAIL1(failure, "STACK_DUP");      \
    _RESTORESP;
// Dangerous optimizations:
// XX todo optim: goto shared failure handlers, please!
#define STACK_ENSURE_HAS(n)                         \
    if (! (SP >= n))                                \
        FAIL1(failure, "STACK_ENSURE_HAS");
#define STACK_ENSURE_FREE(n)                            \
    /* XX slight risk for number overflow */            \
        if (! (SP < process->stack.len - 1))            \
            FAIL1(failure, "STACK_ENSURE_FREE");
#define STACK_ALLOC(n)                                                  \
    /* Allocate n slots on the stack. CAREFUL: does not initialize them!! */ \
        SP_ADD(n); /* XX risk for overflow */                           \
        if (! (SP <= process->stack.len)) {                             \
            SP_SUB(n);                                                  \
            FAIL1(failure, "STACK_ALLOC");                              \
        }
#define STACK_UNSAFE_REF(i)                     \
        process->stack.vals[SP - 1 - i]
#define STACK_UNSAFE_SET(i, v)                  \
        process->stack.vals[SP - 1 - i] = v
#define STACK_UNSAFE_REMOVE(n)                  \
        SP_SUB(n)
#define STACK_UNSAFE_SET_LAST(v)                \
        process->stack.vals[SP - 1] = v;

#define ARGB1 (pc[1])
#define ARGB2 (pc[2])
#define ARGB3 (pc[3])
#define ARGIM1 (pc[1]+(pc[2]<<8))
#define ARGIM2 (pc[3]+(pc[4]<<8))
    // ^ XX boundary checks!

#ifdef VM_TRACE
# define TRACE_OP(opcodename) do {                                      \
            if (process->trace_on) {                                    \
                STORE_ALL;                                              \
                vm_trace(process, pc-code, opcodename, M, N);           \
            }                                                           \
    } while (0)
#else
# define TRACE_OP(opcodename)
#endif

#if DEBUG_MEM_VERIFY
# define  MEM_VERIFY                                    \
    ASSERT((*(process->alloc_area) == 0xAAAA) ||        \
           (*(process->alloc_area) == 0xBBBB))
#else
# define  MEM_VERIFY
#endif

#include "_opcode_dispatch.h"

halt:
    /* WARN("machine halted"); */
    return;
on_error:
    DIE_("vm_process_run: error at PC %" PRIuPTR ": \n"
         "  %s at %s %i\n"
         "    %s: %s at %s %i",
         PC,
         failure.message, failure.file, failure.line,
         failure.maybe_parent_failure->context_message,
         failure.maybe_parent_failure->error_message,
         failure.maybe_parent_failure->file,
         failure.maybe_parent_failure->line
        );
}


