/*
  Copyright (C) 2021 Christian Jaeger, <ch@christianjaeger.ch>
  See the LICENSE file that comes bundled with this file.
*/

#include "chj-ctest/test.h"
#include "chj-ctest/run_tests.h"
#include "vm.h"
#include "bytecode.h"



TEST(basics) {
    /*
      Create some simple bytecode sequences and run them, as well as
      save some of them to *.bytecode files.

      Uses the OP* macros from bytecode.h, which, via the OP_*
      constants defined in opcode_constants.h, push to the array at
      `pc`.
     */
    uint8_t program[50];
    uint8_t *program_end;
    uint8_t *pc = program;
    LET_NEW_VM_PROCESS(process, 200, /* stacklen */ 28 /* heaplen */);

    OP_IM(PUSH_IM, FIX(3));
    OP_IM(PUSH_IM, FIX(123));
    OP(INC);
    OP(HALT);
    vm_process_run(process, program);
    ASSERT_EQ(process->stack.sp, 2);
    ASSERT_EQ(process->stack.vals[0], FIX(3));
    ASSERT_EQ(process->stack.vals[1], FIX(124));

    pc = program;
    OP(ADD);
    OP(HALT);
    vm_process_run(process, program);
    ASSERT_EQ(process->stack.sp, 1);
    ASSERT_EQ(process->stack.vals[0], FIX(127));

    pc = program;
    OP_IM(ADD_IM, FIX(-3333));
    OP(HALT);
    vm_process_run(process, program);
    ASSERT_EQ(process->stack.sp, 1);
    ASSERT_EQ(process->stack.vals[0], FIX(-3206));
    
    pc = program;
    OP(DROP1);
    OP(HALT);
    vm_process_run(process, program);
    ASSERT_EQ(process->stack.sp, 0);

    //vm_process_run(process, program); XX error handling check

    pc = program;
    OP_IM(PUSH_IM, FIX(0));
    OP_IM_IM(BEQ_IM_REL16, FIX(0), 9); // 5
    OP_IM(PUSH_IM, FAL); // 3
    OP(HALT); // 1
    OP_IM(PUSH_IM, TRU);
    OP(HALT);
    vm_process_run(process, program);
    ASSERT_EQ(process->stack.sp, 1);
    ASSERT_EQ(process->stack.vals[0], TRU);

    pc = program;
    OP_IM(PUSH_IM, FAL);
    OP(UNSAFE_BITWISE_AND);
    OP(HALT);
    vm_process_run(process, program);
    ASSERT_EQ(process->stack.sp, 1);
    ASSERT_EQ(process->stack.vals[0], FAL);

    pc = program;
    OP_IM(PUSH_IM, TRU);
    OP(UNSAFE_BITWISE_OR);
    OP(HALT);
    vm_process_run(process, program);
    ASSERT_EQ(process->stack.sp, 1);
    ASSERT_EQ(process->stack.vals[0], TRU);

    /*
         fperl> sub fact($tot, $x) { $x > 0 ? fact($tot*$x, $x-1) : $tot }
         fperl> fact(1, 10)
         $VAR1 = 3628800;
    */
#if 0
    pc = program;
    OP_IM(PUSH_IM, FIX(1));
    OP_IM(PUSH_IM, FIX(10));
    // And yes, changing pointer ONCE then referring by index just may
    // be faster. Will be. Since stack access is 'by index'
    // anyway. (Correct?)
    // (Also then probably still should add registers, too?...)
    OP_IM(BNEG0_KEEP_REL16, 9); // 3
    // tot += x
    OP_B_B(MUL__, 0, 1); // 3
    // x--
    OP(DEC); // 1
    OP_B(JMP_REL8, -7); // 2
    OP(HALT);
    vm_process_stack_clear(process);
    vm_process_run(process, program);
    ASSERT_EQ(process->stack.sp, 2);
    /* ASSERT_EQ(scm_integer_to_int32(process->stack.vals[0]), (3628800)); */
#endif
    
    /*
       (define (fib n)
         (if (< n 2)
             1
             (+ (fib (- n 1))
                (fib (- n 2)))))
       (fib 2) => 2
       (fib 3) => 3
       (fib 4) => 5
       (fib 6) => 13
       (fib 16) => 1597
       (fib 20) => 10946
       (fib 30) => 1346269
    */
    pc = program;
    /*0*/  OP_IM(PUSH_IM, FIX(2));
    /*3*/  OP_B(JSR_REL8, 3); //2
    /*5*/  OP(HALT); //1

    // fib: n is last on stack before ret addr
    /*6*/  OP(SWAP); //1
    /*7*/  OP_IM_B(CMPBR_KEEP_LT_IM_REL8, FIX(2), 14); //4   end:
    /*11*/ OP(DEC); //1
    /*12*/ OP(DUP); //1
    /*13*/ OP_B(JSR_REL8, -7); //2 fib
    /*15*/ OP(SWAP); //1
    /*16*/ OP(DEC); //1
    /*17*/ OP_B(JSR_REL8, -11); //2 fib
    /*19*/ OP(ADD); //1
    /*20*/ OP(RET_POP);//1
    // end:
    /*21*/ OP(DROP1);
    /*22*/ OP_IM(RET_IM, FIX(1));
    // ---
    program_end = pc;
    vm_process_stack_clear(process);
    vm_process_run(process, program);
    ASSERT_EQ_(stacksize_t, process->stack.sp, 1);
    ASSERT_EQ_(val, process->stack.vals[0], FIX(2));

    pc = program;
    /*0*/  OP_IM(PUSH_IM, FIX(16));
    vm_process_stack_clear(process);
    vm_process_run(process, program);
    ASSERT_EQ_(stacksize_t, process->stack.sp, 1);
    ASSERT_EQ_(val, process->stack.vals[0], FIX(1597));

    if (0) {
        pc = program;
        /*0*/  OP_IM(PUSH_IM, FIX(25));
        vm_process_stack_clear(process);
        vm_process_run(process, program);
        ASSERT_EQ_(stacksize_t, process->stack.sp, 1);
        WRITELN(process->stack.vals[0]);
        ASSERT_EQ(SCM_NUMBER_EQUAL(process->stack.vals[0],
                                   FIXMULINT_TO_SCM(121393)),
                  TRU);
    }

    if (0) {
        pc = program;
        /*0*/  OP_IM(PUSH_IM, FIX(30));
        vm_process_stack_clear(process);
        vm_process_run(process, program);
        ASSERT_EQ_(stacksize_t, process->stack.sp, 1);
        WRITELN(process->stack.vals[0]);
        ASSERT_EQ(SCM_NUMBER_EQUAL(process->stack.vals[0],
                                   FIXMULINT_TO_SCM(1346269)),
                  TRU);
    }

    pc = program;
    /*0*/  OP_IM(PUSH_IM, FIX(35));
    bytecode_write_file(program, program_end-program, "fib_35.bytecode");
    if (0) {
        vm_process_stack_clear(process);
        vm_process_run(process, program);
        ASSERT_EQ_(stacksize_t, process->stack.sp, 1);
        WRITELN(process->stack.vals[0]);
        ASSERT_EQ(SCM_NUMBER_EQUAL(process->stack.vals[0],
                                   FIXMULINT_TO_SCM(14930352)),
                  TRU);
    }

    // Naive fibonacci again, but with combined opcodes:
    pc = program;
    /*0*/  OP_IM(PUSH_IM, FIX(2));
    /*3*/  OP_B(JSR_REL8__SWAP, 3); //2
    /*5*/  OP(HALT); //1

    // fib: n is last on stack (after ret addr)
    /*6*/  OP_IM_B(CMPBR_KEEP_LT_IM_REL8, FIX(2), 12); //4   end:
    /*10*/ OP(DEC__DUP); //1
    /*11*/ OP_B(JSR_REL8__SWAP, -5); //2 fib
    /*13*/ OP(SWAP__DEC); //1
    /*14*/ OP_B(JSR_REL8__SWAP, -8); //2 fib
    /*16*/ OP(ADD); //1
    /*17*/ OP(RET_POP);//1
    // end:
    /*18*/ OP(DROP1);
    /*19*/ OP_IM(RET_IM, FIX(1));
    // ---
    program_end = pc;
    vm_process_stack_clear(process);
    vm_process_run(process, program);
    ASSERT_EQ_(stacksize_t, process->stack.sp, 1);
    ASSERT_EQ_(val, process->stack.vals[0], FIX(2));

    pc = program;
    /*0*/  OP_IM(PUSH_IM, FIX(35));
    bytecode_write_file(program, program_end-program,
                        "fib_combinedop_35.bytecode");
    if (0) {
        vm_process_stack_clear(process);
        vm_process_run(process, program);
        ASSERT_EQ_(stacksize_t, process->stack.sp, 1);
        WRITELN(process->stack.vals[0]);
        ASSERT_EQ(SCM_NUMBER_EQUAL(process->stack.vals[0],
                                   FIXMULINT_TO_SCM(14930352)),
                  TRU);
    }


    // Naive fibonacci again, but using registers (not using combined
    // opcodes):
    pc = program;
    /*0*/  OP_IM(LOADA_IM, FIX(16));//3
    /*3*/  OP(NOP /* TRACE_ON */); //1
    /*4*/  OP(NOP); // 1 -- had GC registering and unregistering ops here
    /*5*/  OP_IM(LOADB_IM, FAL); //3  -- not actually used now, though
    /*8*/  OP(NOP); // 1
    /*9*/  OP_B(JSR_REL8, 6); //2
    /*11*/ OP(PUSHA);//1
    /*12*/ OP(NOP); // 1
    /*13*/ OP(TRACE_OFF);//1
    /*14*/ OP(HALT); //1

    // fib: n is in register A; return result in register A
    /*15*/ OP_IM_B(CMPBR_A_LT_IM_REL8, FIX(2), 14); //4   end:
    /*19*/ OP(DECA); //1
    /*20*/ OP(PUSHA); //1
    /*21*/ OP_B(JSR_REL8, -6); //2 fib
    /*23*/ OP(SWAPA); //1
    /*24*/ OP(DECA); //1
    /*25*/ OP_B(JSR_REL8, -10); //2 fib
    /*27*/ OP(ADDA); //1
    /*28*/ OP(RET);//1
    // end:
    /*29*/ OP_IM(LOADA_IM, FIX(1));//3
    /*32*/ OP(RET);
    // ---
    program_end = pc;
    vm_process_stack_clear(process);
    vm_process_run(process, program);
    ASSERT_EQ_(stacksize_t, process->stack.sp, 1);
    ASSERT_EQ_(val, process->stack.vals[0], FIX(1597));

    pc = program;
    /*0*/  OP_IM(LOADA_IM, FIX(35));
    bytecode_write_file(program, program_end-program,
                        "fib_registers_35.bytecode");
    if (0) {
        vm_process_stack_clear(process);
        vm_process_run(process, program);
        ASSERT_EQ_(stacksize_t, process->stack.sp, 1);
        WRITELN(process->stack.vals[0]);
        ASSERT_EQ(SCM_NUMBER_EQUAL(process->stack.vals[0],
                                   FIXMULINT_TO_SCM(14930352)),
                  TRU);
    }
    pc = program;
    /*0*/  OP_IM(LOADA_IM, FIX(40));
    bytecode_write_file(program, program_end-program,
                        "fib_registers_40.bytecode");

    // Naive fibonacci using registers again, but taking advantage of
    // static typing for the input register (unsafe, assumes a
    // bytecode verifier that can check types). Values also need to be
    // kept on the stack in which case they need to be converted to
    // fixnums, hence can't do the same optimization for the result
    // type (since it goes into bignums, even though the M register
    // would be large enough).
    pc = program;
    /*0*/  OP_IM(LOADN_IM, 16);//3
    /*3*/  OP(TRACE_ON); //1
    /*4*/  OP(NOP); // 1 -- had GC registering and unregistering ops here
    /*5*/  OP_IM(LOADB_IM, FAL); //3  -- not actually used now, though
    /*8*/  OP(NOP); // 1
    /*9*/  OP_B(JSR_REL8, 6); //2
    /*11*/ OP(PUSHA);//1
    /*12*/ OP(NOP); // 1
    /*13*/ OP(TRACE_OFF);//1
    /*14*/ OP(HALT); //1

    // fib: n is in register N; return result in register A
    /*15*/ OP_IM_B(CMPBR_N_LT_IM_REL8, 2, 14); //4   end:
    /*19*/ OP(DECN); //1
    /*20*/ OP(PUSHN); //1
    /*21*/ OP_B(JSR_REL8, -6); //2 fib
    /*23*/ OP(POPN__PUSHA); //1 -- ok this is a combined op
    /*24*/ OP(DECN); //1
    /*25*/ OP_B(JSR_REL8, -10); //2 fib
    /*27*/ OP(ADDA); //1
    /*28*/ OP(RET);//1
    // end:
    /*29*/ OP_IM(LOADA_IM, FIX(1));//3
    /*32*/ OP(RET);
    // ---
    program_end = pc;
    vm_process_stack_clear(process);
    vm_process_run(process, program);
    ASSERT_EQ_(stacksize_t, process->stack.sp, 1);
    ASSERT_EQ_(val, process->stack.vals[0], FIX(1597));

    pc = program;
    /*0*/  OP_IM(LOADN_IM, 35);
    bytecode_write_file(program, program_end-program,
                        "fib_binaryregisters_35.bytecode");
    if (0) {
        vm_process_stack_clear(process);
        vm_process_run(process, program);
        ASSERT_EQ_(stacksize_t, process->stack.sp, 1);
        WRITELN(process->stack.vals[0]);
        ASSERT_EQ(SCM_NUMBER_EQUAL(process->stack.vals[0],
                                   FIXMULINT_TO_SCM(14930352)),
                  TRU);
    }
    pc = program;
    /*0*/  OP_IM(LOADN_IM, 40);
    bytecode_write_file(program, program_end-program,
                        "fib_binaryregisters_40.bytecode");


    // Same again (binary registers), but use stack *frames* instead
    // of individual push and pop. (Well, there are only 2 of those to
    // coalesce into one, so not expecting much.)
    pc = program;
    /*0*/  OP_IM(LOADN_IM, 16);//3
    /*3*/  OP(TRACE_ON); //1
    /*4*/  OP(NOP); // 1 -- had GC registering and unregistering ops here
    /*5*/  OP_IM(LOADB_IM, FAL); //3  -- not actually used now, though
    /*8*/  OP(NOP); // 1
    /*9*/  OP_B(FRAME_JSR_REL8_1, 6); //2
    /*11*/ OP(PUSHA);//1
    /*12*/ OP(NOP); // 1
    /*13*/ OP(TRACE_OFF);//1
    /*14*/ OP(HALT); //1

    // fib: n is in register N; return result in register A.
    // Allocates stack frames with 1 slot to store the intermediary N
    // and then A values at position 1 (position 0 is the return
    // address).
    /*15*/ OP_IM_B(CMPBR_N_LT_IM_REL8, 2, 20); //4   end:
    /*19*/ OP(DECN); //1
    /*20*/ OP_B(UNSAFE_STN_FIX_, 1); //2
    /*22*/ OP_B(FRAME_JSR_REL8_1, -7); //2 fib
    /*24*/ OP_B(UNSAFE_LDN_INT_, 1);//2
    /*26*/ OP_B(UNSAFE_STA_, 1);//2
    /*28*/ OP(DECN); //1
    /*29*/ OP_B(FRAME_JSR_REL8_1, -14); //2 fib
    /*31*/ OP_B(UNSAFE_ADDA_, 1); //2
    /*33*/ OP_B(UNSAFE_FRAME_RET, 2);//2
    // end:
    /*35*/ OP_IM(LOADA_IM, FIX(1));//3
    /*36*/ OP_B(UNSAFE_FRAME_RET, 2);
    // ---
    program_end = pc;
    vm_process_stack_clear(process);
    vm_process_run(process, program);
    ASSERT_EQ_(stacksize_t, process->stack.sp, 1);
    ASSERT_EQ_(val, process->stack.vals[0], FIX(1597));

    pc = program;
    /*0*/  OP_IM(LOADN_IM, 35);
    bytecode_write_file(program, program_end-program,
                        "fib_binaryregisters_frame_35.bytecode");
    if (0) {
        vm_process_stack_clear(process);
        vm_process_run(process, program);
        ASSERT_EQ_(stacksize_t, process->stack.sp, 1);
        WRITELN(process->stack.vals[0]);
        ASSERT_EQ(SCM_NUMBER_EQUAL(process->stack.vals[0],
                                   FIXMULINT_TO_SCM(14930352)),
                  TRU);
    }
    pc = program;
    /*0*/  OP_IM(LOADN_IM, 40);
    bytecode_write_file(program, program_end-program,
                        "fib_binaryregisters_frame_40.bytecode");


    // Naive fibonacci again, but with "full program compilation as
    // opcode":
    pc = program;
    /*0*/  OP_IM(PUSH_IM, FIX(35));
    /*3*/  OP_B(JSR_REL8, 3); //2
    /*5*/  OP(HALT); //1
    /*6*/  OP(FIB); //1
    program_end = pc;
    bytecode_write_file(program, program_end-program,
                        "fib_compiled_35.bytecode");
    
    if (0) {
        vm_process_stack_clear(process);
        vm_process_run(process, program);
        ASSERT_EQ_(stacksize_t, process->stack.sp, 1);
        WRITELN(process->stack.vals[0]);
        ASSERT_EQ(SCM_NUMBER_EQUAL(process->stack.vals[0],
                                   FIXMULINT_TO_SCM(14930352)),
                  TRU);
    }


    // And same thing again, but now in a form that uses some
    // registers to avoid stack usage:
    pc = program;
    /*0*/  OP_IM(PUSH_IM, FIX(35));
    /*3*/  OP_B(JSR_REL8, 3); //2
    /*5*/  OP(HALT); //1
    /*6*/  OP(FIB_WITH_REGISTERS); //1
    program_end = pc;
    bytecode_write_file(program, program_end-program,
                        "fib_compiled_register_35.bytecode");
    
    if (0) {
        vm_process_stack_clear(process);
        vm_process_run(process, program);
        ASSERT_EQ_(stacksize_t, process->stack.sp, 1);
        WRITELN(process->stack.vals[0]);
        ASSERT_EQ(SCM_NUMBER_EQUAL(process->stack.vals[0],
                                   FIXMULINT_TO_SCM(14930352)),
                  TRU);
    }

    
    printf("gc_count = %" PRIu32 ", gc_moves = %" PRIu64 "\n",
           process->gc_count, process->gc_moves);
    // BREAK();
    FREE_VM_PROCESS(process);
}


#ifdef __C64__
void main(void)
#else
int main(UNUSED int _argc, UNUSED char **_argv)
#endif
{
    // benchmark_init();
    run_tests();
}
