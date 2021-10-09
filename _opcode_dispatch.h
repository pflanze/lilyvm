/* DO NOT EDIT. This file has been generated by opcodes.scm */


{
    static void* op2label[256] = { &&invalid_op, &&op_loadM_im, &&op_loadN_im, &&op_loadX_im, &&op_loadY_im, &&op_loadA_im, &&op_loadB_im, &&op_pushA, &&op_pushB, &&op_popA, &&op_popB, &&op_TAB, &&op_TBA, &&op_swapA, &&op_push_im, &&op_drop1, &&op_pick_b, &&op_swap, &&op_dup, &&op_pushM, &&op_inc, &&op_inc_, &&op_incA, &&op_pushN, &&op_swapN, &&op_dec, &&op_decA, &&op_decN, &&invalid_op, &&invalid_op, &&op_add, &&op_add_im, &&op_add__, &&op_addA, &&op_addM, &&op_mul__, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&op_bitwise_and, &&op_unsafe_bitwise_and, &&op_unsafe_bitwise_or, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&op_jmp_rel8, &&op_jmp_rel16, &&invalid_op, &&invalid_op, &&invalid_op, &&op_jsr_rel8, &&op_ret, &&op_ret_im, &&op_ret_pop, &&invalid_op, &&invalid_op, &&op_beq_im_rel16, &&op_bpos_keep_rel16, &&op_bneg0_keep_rel16, &&op_bneg_keep_rel16, &&op_bz_keep_rel16, &&op_bz_rel16, &&invalid_op, &&invalid_op, &&invalid_op, &&op_cmpbr_keep_lt_im_rel8, &&op_cmpbr_A_lt_im_rel8, &&op_cmpbr_N_lt_im_rel8, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&op_popN__pushA, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&op_dec__dup, &&op_swap__dec, &&op_jsr_rel8__swap, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&op_fib, &&op_fib_with_registers, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&invalid_op, &&op_trace_on, &&op_trace_off, &&op_nop, &&op_halt };
// not registered with GC, used just to avoid needing local vars
word_t tmp1;

// Registers for binary data (not registered with GC):
dword_t M = 0;
word_t N = 0;
word_t X = 0;
word_t Y = 0;

// Registers for (not registered, but copied to process struct and
// back when needed):
val A = UNINITIALIZED;
val B = UNINITIALIZED;

#define _STORE(X) do { process->X = X; } while (0)
#define _RESTORE(X) do { X = process->X; } while (0)
#define STORE_ALL do { _STORE(A); _STORE(B); } while (0);
#define RESTORE_ALL do { _RESTORE(A); _RESTORE(B); } while (0);
#define STORE_EXCEPT_A do { _STORE(B); } while (0);
#define RESTORE_EXCEPT_A do { _RESTORE(B); } while (0);


#define DISPATCH                             \
    {                                        \
        MEM_VERIFY;                          \
        goto *op2label[OPCODE];              \
    }

    DISPATCH;

    op_loadM_im: /* loadM_im, 2 */
        TRACE_OP("loadM_im");
        {
            M = ARGIM1;
        }
        pc += 3;
        DISPATCH;
    op_loadN_im: /* loadN_im, 2 */
        TRACE_OP("loadN_im");
        {
            N = ARGIM1;
        }
        pc += 3;
        DISPATCH;
    op_loadX_im: /* loadX_im, 2 */
        TRACE_OP("loadX_im");
        {
            X = ARGIM1;
        }
        pc += 3;
        DISPATCH;
    op_loadY_im: /* loadY_im, 2 */
        TRACE_OP("loadY_im");
        {
            Y = ARGIM1;
        }
        pc += 3;
        DISPATCH;
    op_loadA_im: /* loadA_im, 2 */
        TRACE_OP("loadA_im");
        {
            A = ARGIM1;
        }
        pc += 3;
        DISPATCH;
    op_loadB_im: /* loadB_im, 2 */
        TRACE_OP("loadB_im");
        {
            B = ARGIM1;
        }
        pc += 3;
        DISPATCH;
    op_pushA: /* pushA, 0 */
        TRACE_OP("pushA");
        {
            PUSH(A);
        }
        pc += 1;
        DISPATCH;
    op_pushB: /* pushB, 0 */
        TRACE_OP("pushB");
        {
            PUSH(B);
        }
        pc += 1;
        DISPATCH;
    op_popA: /* popA, 0 */
        TRACE_OP("popA");
        {
            LET_POP(x);
            A = x;
        }
        pc += 1;
        DISPATCH;
    op_popB: /* popB, 0 */
        TRACE_OP("popB");
        {
            LET_POP(x);
            B = x;
        }
        pc += 1;
        DISPATCH;
    op_TAB: /* TAB, 0 */
        TRACE_OP("TAB");
        {
            B = A;
        }
        pc += 1;
        DISPATCH;
    op_TBA: /* TBA, 0 */
        TRACE_OP("TBA");
        {
            A = B;
        }
        pc += 1;
        DISPATCH;
    op_swapA: /* swapA, 0 */
        TRACE_OP("swapA");
        {
            STACK_ENSURE_HAS(1);
            val x = STACK_UNSAFE_REF(0);
            STACK_UNSAFE_SET(0, A);
            A = x;
        }
        pc += 1;
        DISPATCH;
    op_pushM: /* pushM, 0 */
        TRACE_OP("pushM");
        {
            PUSH(FIX(M)); // unsafe
        }
        pc += 1;
        DISPATCH;
    op_pushN: /* pushN, 0 */
        TRACE_OP("pushN");
        {
            PUSH(FIX(N)); // unsafe
        }
        pc += 1;
        DISPATCH;
    op_swapN: /* swapN, 0 */
        TRACE_OP("swapN");
        {
            STACK_ENSURE_HAS(1);
            val x = STACK_UNSAFE_REF(0);
            STACK_UNSAFE_SET(0, FIX(N)); // unsafe
            N = INT(x); // unsafe
        }
        pc += 1;
        DISPATCH;
    op_popN__pushA: /* popN__pushA, 0 */
        TRACE_OP("popN__pushA");
        {
            STACK_ENSURE_HAS(1);
            N = INT(STACK_UNSAFE_REF(0)); // unsafe
            STACK_UNSAFE_SET(0, A);
        }
        pc += 1;
        DISPATCH;
    op_push_im: /* push_im, 2 */
        TRACE_OP("push_im");
        {
            PUSH(ARGIM1);
        }
        pc += 3;
        DISPATCH;
    op_drop1: /* drop1, 0 */
        TRACE_OP("drop1");
        {
            STACK_DROP1;
        }
        pc += 1;
        DISPATCH;
    op_pick_b: /* pick_b, 1 */
        TRACE_OP("pick_b");
        {
            // copy the nth-last argument (0 meaning the last, i.e. top element
            // of the stack) from the stack
            LET_STACK_REF(x, ARGB1);
            PUSH(x);
        }
        pc += 2;
        DISPATCH;
    op_swap: /* swap, 0 */
        TRACE_OP("swap");
        {
            STACK_SWAP;
        }
        pc += 1;
        DISPATCH;
    op_dup: /* dup, 0 */
        TRACE_OP("dup");
        {
            STACK_DUP;
        }
        pc += 1;
        DISPATCH;
    op_inc: /* inc, 0 */
        TRACE_OP("inc");
        {
            LET_STACK_LAST(x);
            STORE_ALL;
            STACK_UNSAFE_SET_LAST(SCM_INC(x));
            RESTORE_ALL;
            
        }
        pc += 1;
        DISPATCH;
    op_inc_: /* inc_, 1 */
        TRACE_OP("inc_");
        {
            LET_STACK_REF(x, ARGB1);
            STORE_ALL;
            PUSH(SCM_INC(x));
            RESTORE_ALL;
            
        }
        pc += 2;
        DISPATCH;
    op_incA: /* incA, 0 */
        TRACE_OP("incA");
        {
            STORE_EXCEPT_A;
            A = SCM_INC(A);
            RESTORE_EXCEPT_A;
            
        }
        pc += 1;
        DISPATCH;
    op_dec: /* dec, 0 */
        TRACE_OP("dec");
        {
            LET_STACK_LAST(x);
            STORE_ALL;
            STACK_UNSAFE_SET_LAST(SCM_DEC(x));
            RESTORE_ALL;
            
        }
        pc += 1;
        DISPATCH;
    op_decA: /* decA, 0 */
        TRACE_OP("decA");
        {
            DO_SCM_DEC(STORE_EXCEPT_A, RESTORE_EXCEPT_A, A=, A);
        }
        pc += 1;
        DISPATCH;
    op_decN: /* decN, 0 */
        TRACE_OP("decN");
        {
            N = ((signed_word_t)N) - 1; // unsafe (no overflow/UB check)!
            
        }
        pc += 1;
        DISPATCH;
    op_add: /* add, 0 */
        TRACE_OP("add");
        {
            STACK_ENSURE_HAS(2);
            val res;
            DO_SCM_ADD(STORE_ALL, RESTORE_ALL, res=, STACK_UNSAFE_REF(1), STACK_UNSAFE_REF(0))
            STACK_UNSAFE_SET(1, res);
            STACK_UNSAFE_REMOVE(1);
        }
        pc += 1;
        DISPATCH;
    op_add_im: /* add_im, 2 */
        TRACE_OP("add_im");
        {
            STACK_ENSURE_HAS(1);
            val res;
            DO_SCM_ADD(STORE_ALL, RESTORE_ALL, res=, STACK_UNSAFE_REF(0), ARGIM1);
            STACK_UNSAFE_SET(0, res);
        }
        pc += 3;
        DISPATCH;
    op_add__: /* add__, 2 */
        TRACE_OP("add__");
        {
            uint8_t i = ARGB1;
            LET_STACK_REF(a, i);
            {
                LET_STACK_REF(b, ARGB2);
                STORE_ALL;
                STACK_SET(i, SCM_ADD(a, b));
                RESTORE_ALL;
            }
            
        }
        pc += 3;
        DISPATCH;
    op_addA: /* addA, 0 */
        TRACE_OP("addA");
        {
            STACK_ENSURE_HAS(1);
            DO_SCM_ADD(STORE_EXCEPT_A, RESTORE_EXCEPT_A, A=, A, STACK_UNSAFE_REF(0));
            STACK_UNSAFE_REMOVE(1);
        }
        pc += 1;
        DISPATCH;
    op_addM: /* addM, 0 */
        TRACE_OP("addM");
        {
            STACK_ENSURE_HAS(1);
            M = (signed_dword_t)M + (signed_dword_t)INT(STACK_UNSAFE_REF(0)); // unsafe
            STACK_UNSAFE_REMOVE(1);
            
        }
        pc += 1;
        DISPATCH;
    op_mul__: /* mul__, 2 */
        TRACE_OP("mul__");
        {
            uint8_t i = ARGB1;
            LET_STACK_REF(a, i);
            {
                LET_STACK_REF(b, ARGB2);
                STORE_ALL;
                STACK_SET(i, SCM_MUL(a, b));
                RESTORE_ALL;
            }
            
        }
        pc += 3;
        DISPATCH;
    op_bitwise_and: /* bitwise_and, 0 */
        TRACE_OP("bitwise_and");
        {
            LET_POP(b);
            {
                LET_POP(a);
                STORE_ALL;
                PUSH(SCM_BITWISE_AND(a, b));
                RESTORE_ALL;
            }
            
        }
        pc += 1;
        DISPATCH;
    op_unsafe_bitwise_and: /* unsafe_bitwise_and, 0 */
        TRACE_OP("unsafe_bitwise_and");
        {
            LET_POP(b);
            {
                LET_POP(a);
                PUSH(a & b); // since non-data bits should be identical.
            }
            
        }
        pc += 1;
        DISPATCH;
    op_unsafe_bitwise_or: /* unsafe_bitwise_or, 0 */
        TRACE_OP("unsafe_bitwise_or");
        {
            LET_POP(b);
            {
                LET_POP(a);
                PUSH(a | b); // since non-data bits should be identical.
            }
            
        }
        pc += 1;
        DISPATCH;
    op_jmp_rel8: /* jmp_rel8, 1 */
        TRACE_OP("jmp_rel8");
        {
            pc += (int8_t)ARGB1;
        }
        DISPATCH;
    op_jmp_rel16: /* jmp_rel16, 2 */
        TRACE_OP("jmp_rel16");
        {
            pc += (int16_t)ARGIM1;
        }
        DISPATCH;
    op_jsr_rel8: /* jsr_rel8, 1 */
        TRACE_OP("jsr_rel8");
        {
            PUSH(PCNUM(PC+2)); //XX make safe
            pc += (int8_t)ARGB1;
        }
        DISPATCH;
    op_ret: /* ret, 0 */
        TRACE_OP("ret");
        {
            LET_POP(origpc);
            SET_PC(PCNUM_TO_WORD(origpc));
            
        }
        DISPATCH;
    op_ret_im: /* ret_im, 2 */
        TRACE_OP("ret_im");
        {
            LET_POP(origpc);
            PUSH(ARGIM1);
            SET_PC(PCNUM_TO_WORD(origpc));
            
        }
        DISPATCH;
    op_ret_pop: /* ret_pop, 0 */
        TRACE_OP("ret_pop");
        {
            /*
            LET_POP(v);
            LET_POP(origpc);
            PUSH(v);
            */
            STACK_ENSURE_HAS(2);
            {
                val origpc = STACK_UNSAFE_REF(1);
                STACK_UNSAFE_SET(1, STACK_UNSAFE_REF(0));
                STACK_UNSAFE_REMOVE(1);
                SET_PC(PCNUM_TO_WORD(origpc));
            }
            
        }
        DISPATCH;
    op_beq_im_rel16: /* beq_im_rel16, 4 */
        TRACE_OP("beq_im_rel16");
        {
            LET_POP(x);
            if (x == ARGIM1) {
                pc += (uint16_t)ARGIM2;
            } else {
                pc += 5;
            }
        }
        DISPATCH;
    op_bpos_keep_rel16: /* bpos_keep_rel16, 2 */
        TRACE_OP("bpos_keep_rel16");
        {
            // does *not* claim the value on the stack, but peek at it only
            LET_STACK_REF(x, 0);
            if (INT(x) > 0) {
                pc += (uint16_t)ARGIM1;
            } else {
                pc += 3;
            }
        }
        DISPATCH;
    op_bneg0_keep_rel16: /* bneg0_keep_rel16, 2 */
        TRACE_OP("bneg0_keep_rel16");
        {
            // does *not* claim the value on the stack, but peek at it only
            LET_STACK_LAST(x);
            if (INT(x) <= 0) {
                pc += (uint16_t)ARGIM1;
            } else {
                pc += 3;
            }
        }
        DISPATCH;
    op_bneg_keep_rel16: /* bneg_keep_rel16, 2 */
        TRACE_OP("bneg_keep_rel16");
        {
            // does *not* claim the value on the stack, but peek at it only
            LET_STACK_LAST(x);
            if (fixnum_is_negative(x)) {
                pc += (uint16_t)ARGIM1;
            } else {
                pc += 3;
            }
        }
        DISPATCH;
    op_bz_keep_rel16: /* bz_keep_rel16, 2 */
        TRACE_OP("bz_keep_rel16");
        {
            // does *not* claim the value on the stack, but peek at it only
            LET_STACK_LAST(x);
            if (x == FIX(0)) {
                pc += (uint16_t)ARGIM1;
            } else {
                pc += 3;
            }
        }
        DISPATCH;
    op_bz_rel16: /* bz_rel16, 2 */
        TRACE_OP("bz_rel16");
        {
            LET_POP(x);
            if (x == FIX(0)) {
                pc += (uint16_t)ARGIM1;
            } else {
                pc += 3;
            }
        }
        DISPATCH;
    op_cmpbr_keep_lt_im_rel8: /* cmpbr_keep_lt_im_rel8, 3 */
        TRACE_OP("cmpbr_keep_lt_im_rel8");
        {
            // compare with literal and branch if smaller; (if (< (stack-last) 1234) lbl).
            LET_STACK_LAST(x);
            // (Optim by checking for fixnum and then doing C-level comparison?
            //  Didn't help.)
            if (SCM_NUMBER_CMP(x, ARGIM1) == LT) {
                pc += (uint8_t)ARGB3;
            } else {
                pc += 4;
            }
        }
        DISPATCH;
    op_cmpbr_A_lt_im_rel8: /* cmpbr_A_lt_im_rel8, 3 */
        TRACE_OP("cmpbr_A_lt_im_rel8");
        {
            // compare with literal and branch if smaller; (if (< registerA 1234) lbl).
            if (SCM_NUMBER_CMP(A, ARGIM1) == LT) {
                pc += (uint8_t)ARGB3;
            } else {
                pc += 4;
            }
        }
        DISPATCH;
    op_cmpbr_N_lt_im_rel8: /* cmpbr_N_lt_im_rel8, 3 */
        TRACE_OP("cmpbr_N_lt_im_rel8");
        {
            // compare with *binary* literal and branch if smaller; (if (< registerA 1234) lbl).
            if (((signed_word_t)N) < ((signed_word_t)ARGIM1)) {
                pc += (uint8_t)ARGB3;
            } else {
                pc += 4;
            }
        }
        DISPATCH;
    op_dec__dup: /* dec__dup, 0 */
        TRACE_OP("dec__dup");
        {
            STACK_ALLOC(1);
            STORE_ALL;
            val x = SCM_DEC(STACK_UNSAFE_REF(1));
            STACK_UNSAFE_SET(1, x);
            STACK_UNSAFE_SET(0, x);
            RESTORE_ALL;
            
        }
        pc += 1;
        DISPATCH;
    op_swap__dec: /* swap__dec, 0 */
        TRACE_OP("swap__dec");
        {
            if (1) { // actually faster??
                STACK_SWAP;
                LET_STACK_LAST(x);
                STORE_ALL;
                STACK_UNSAFE_SET_LAST(SCM_DEC(x));
                RESTORE_ALL;
            } else {
                STACK_ENSURE_HAS(2);
                STORE_ALL;
                val tmp = SCM_DEC(STACK_UNSAFE_REF(1));
                STACK_UNSAFE_SET(1, STACK_UNSAFE_REF(0));
                STACK_UNSAFE_SET(0, tmp);
                RESTORE_ALL;
            }
            
        }
        pc += 1;
        DISPATCH;
    op_jsr_rel8__swap: /* jsr_rel8__swap, 1 */
        TRACE_OP("jsr_rel8__swap");
        {
            STACK_ENSURE_HAS(1);
            val x = STACK_UNSAFE_REF(0);
            STACK_UNSAFE_SET(0, PCNUM(PC+2)); //XX make safe
            STACK_ALLOC(1);
            STACK_UNSAFE_SET(0, x);
            pc += (int8_t)ARGB1;
        }
        DISPATCH;
    op_trace_on: /* trace_on, 0 */
        TRACE_OP("trace_on");
        {
            #ifdef VM_TRACE
              vm_process_trace_on(process, true);
            #else
              // XX add WARN_ONCE
              WARN("trace_on: tracing not supported (please recompile with VM_TRACE)");
            #endif
            
        }
        pc += 1;
        DISPATCH;
    op_trace_off: /* trace_off, 0 */
        TRACE_OP("trace_off");
        {
            #ifdef VM_TRACE
              vm_process_trace_on(process, false);
            #endif
            
        }
        pc += 1;
        DISPATCH;
    op_nop: /* nop, 0 */
        TRACE_OP("nop");
        {
            
        }
        pc += 1;
        DISPATCH;
    op_halt: /* halt, 0 */
        TRACE_OP("halt");
        {
            goto halt;
        }
        pc += 1;
        DISPATCH;
    op_fib: /* fib, 0 */
        TRACE_OP("fib");
        {
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
            // fib_end:  see 'inlined' above
        }
        DISPATCH;
    op_fib_with_registers: /* fib_with_registers, 0 */
        TRACE_OP("fib_with_registers");
        {
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
            }
        }
        DISPATCH;
    
    invalid_op:
        DIE_("invalid opcode %i at PC %" PRIuPTR, OPCODE, PC);
}
