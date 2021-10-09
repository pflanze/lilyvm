(define optim-opcodes
  ;; These depend on `computed-goto?` being #t (meaning,
  ;; fib_compiled*_35.bytecode won't work and vmtest can't be compiled)
  '(
    ;; Test full program compilation:
    (220 fib 0 #f "
fib_entry:
STACK_SWAP;
// (CMPBR_KEEP_LT_IM_REL8, FIX(2), 12)
{
    LET_STACK_LAST(x);
    if (SCM_NUMBER_CMP(x, FIX(2)) == LT) {
        /* goto fib_end;
           can 'inline' it here: */
        STACK_DROP1;
        // (RET_IM, FIX(1))
        {
            LET_POP(origpc);
            PUSH(FIX(1));
            if (is_fixnum(origpc)) {
                goto *(const void *)((uintptr_t)&&fib_entry + INT(origpc));
            } else {
                SET_PC(PCNUM_TO_WORD(origpc));
                DISPATCH;
            }
        }
    }
}
// DEC__DUP
{
    STACK_ALLOC(1);
    val x = SCM_DEC(STACK_UNSAFE_REF(1));
    STACK_UNSAFE_SET(1, x);
    STACK_UNSAFE_SET(0, x);
}
// (JSR_REL8, -6)
{
    /*
       We are not using PC here, but the native CPU pc.
       goto is changing the latter. rts must do the same.
       We must store the native pc; well, label values.
       Differences, to fit in `val`. There are no IMMEDIATE_KIND:s
       left, so for this test, use FIX (unlike PCNUM it's signed,
       in case the compiler re-orders labels; also, now
       rts can distinguish between the two cases).
    */
    PUSH(FIX((uintptr_t)&&fib_ret_1 - (uintptr_t)&&fib_entry));
    // ^ relies on undefined behaviour re signed numbers,
    //   but then we rely on that anyway already.
    goto fib_entry;
}
fib_ret_1:
// SWAP__DEC
{
    STACK_SWAP;
    LET_STACK_LAST(x);
    STACK_UNSAFE_SET_LAST(SCM_DEC(x));
}
// (JSR_REL8, -9)
{
    PUSH(FIX((uintptr_t)&&fib_ret_2 - (uintptr_t)&&fib_entry));
    goto fib_entry;
}
fib_ret_2:
// ADD
{
    STACK_ENSURE_HAS(2);
    STACK_UNSAFE_SET(1, SCM_ADD(STACK_UNSAFE_REF(1), STACK_UNSAFE_REF(0)));
    STACK_UNSAFE_REMOVE(1);
}
// RET_POP
STACK_ENSURE_HAS(2);
{
#define origpc tmp1
    origpc = STACK_UNSAFE_REF(1);
    STACK_UNSAFE_SET(1, STACK_UNSAFE_REF(0));
    STACK_UNSAFE_REMOVE(1);
    if (is_fixnum(origpc)) {
        goto *(const void *)((uintptr_t)&&fib_entry + INT(origpc));
    } else {
        SET_PC(PCNUM_TO_WORD(origpc));
        DISPATCH;
    }
#undef origpc
}
// fib_end:  see 'inlined' above")

    ;; Same but avoiding some stack ops by using some register
    ;; variables:
    (221 fib_with_registers 0 #f "
// NOTE:  this causes breakage sometimes when compiled with gcc -O2 or -O3,
// and especially when compiled with g++ -O3. Why? I already avoid
// any fresh variables in nested scopes (unless short-lived, i.e.
// used in a scope where no goto happens), apparently that's not the problem?


// Change calling conventions (A is used for the first argument
// and the return value):
STACK_SWAP;
{
    LET_POP(n);
    A = n;
}
// jsr fib_with_registers_entry
PUSH(FIX((uintptr_t)&&fib_with_registers_end_calling_conventions
         - (uintptr_t)&&fib_with_registers_entry));
goto fib_with_registers_entry;
fib_with_registers_end_calling_conventions:
PUSH(A);
STACK_SWAP;
goto op_ret;


// entry with register based calling conventions:
fib_with_registers_entry:
// (CMPBR_KEEP_LT_IM_REL8, FIX(2), 12)
{
#define x A
    if (SCM_NUMBER_CMP(x, FIX(2)) == LT) {
        // LET_POP(origpc);
        // STACK_ENSURE_HAS(1);
#define origpc tmp1
        origpc = STACK_UNSAFE_REF(0);
        STACK_UNSAFE_REMOVE(1);
        A = FIX(1);
        // optim: it now never returns to a PC!--ehr, makes it SLOWER
        if (1 || is_fixnum(origpc)) {
            goto *(const void *)((uintptr_t)&&fib_with_registers_entry
                                 + INT(origpc));
        } else {
            SET_PC(PCNUM_TO_WORD(origpc));
            DISPATCH;
        }
#undef origpc
#undef x
    }
}
// DEC__DUP
{
#ifdef FIXNUM_UNSAFE
    A = POSITIVEFIXNUM_UNSAFE_DEC(A);
#else
STORE_EXCEPT_A;
    A = SCM_DEC(A);
RESTORE_EXCEPT_A;
#endif
    PUSH(A);
}
// (JSR_REL8, -6)
{
    PUSH(FIX((uintptr_t)&&fib_with_registers_ret_1
              - (uintptr_t)&&fib_with_registers_entry));
    goto fib_with_registers_entry;
}
fib_with_registers_ret_1:
// SWAP__DEC
/*
   Was: bring the saved copy of x back, saving the result on the stack
   (via swap). decrement one more, use in recursion (consume).
   Now: do the same with register and local var.
*/
{
#define oldx tmp1
    oldx = STACK_UNSAFE_REF(0);
    STACK_UNSAFE_SET_LAST(A);
#ifdef FIXNUM_UNSAFE
    A = POSITIVEFIXNUM_UNSAFE_DEC(oldx);
#else
STORE_EXCEPT_A;
    A = SCM_DEC(oldx);
RESTORE_EXCEPT_A;
#endif
#undef oldx
}
// (JSR_REL8, -9)
{
    PUSH(FIX((uintptr_t)&&fib_with_registers_ret_2
             - (uintptr_t)&&fib_with_registers_entry));
    goto fib_with_registers_entry;
}
fib_with_registers_ret_2:
// ADD
{
    // STACK_ENSURE_HAS(1); // optim: leave off
STORE_EXCEPT_A;
    A = SCM_ADD(A, STACK_UNSAFE_REF(0));
RESTORE_EXCEPT_A;
    STACK_UNSAFE_REMOVE(1);
}
// RET_POP
//STACK_ENSURE_HAS(1); // optim: leave off
{
#define origpc tmp1
    origpc = STACK_UNSAFE_REF(0);
    STACK_UNSAFE_REMOVE(1);
    if (is_fixnum(origpc)) {
        goto *(const void *)((uintptr_t)&&fib_with_registers_entry
                             + INT(origpc));
    } else {
        SET_PC(PCNUM_TO_WORD(origpc));
        DISPATCH;
    }
#undef origpc
}")))
