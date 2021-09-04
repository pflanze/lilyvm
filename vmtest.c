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

    OP_B_B(PUSH_IM, 7, 0); // heh, add PUSH_IM_8 etc. for smaller code?
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
    //(starting at 1 because I previously had a DROP1 instruction here)
    /*1*/  OP_IM(PUSH_IM, FIX(2));
    /*4*/  OP_B(JSR_REL8, 3); //2
    /*6*/  OP(HALT); //1

    // fib: n is last on stack before ret addr
    /*7*/  OP(SWAP); //1
    /*8*/  OP_IM_B(CMPBR_KEEP_LT_IM_REL8, FIX(2), 14); //4   end:
    /*12*/ OP(DEC); //1
    /*13*/ OP(DUP); //1
    /*14*/ OP_B(JSR_REL8, -7); //2 fib
    /*16*/ OP(SWAP); //1
    /*17*/ OP(DEC); //1
    /*18*/ OP_B(JSR_REL8, -11); //2 fib
    /*20*/ OP(ADD); //1
    /*21*/ OP(RET_POP);//1
    // end:
    /*22*/ OP(DROP1);
    /*23*/ OP_IM(RET_IM, FIX(1));
    // ---
    program_end = pc;
    vm_process_stack_clear(process);
    vm_process_run(process, program);
    ASSERT_EQ_(uint16_t, process->stack.sp, 1);
    ASSERT_EQ_(val, process->stack.vals[0], FIX(2));

    pc = program;
    /*1*/  OP_IM(PUSH_IM, FIX(16));
    vm_process_stack_clear(process);
    vm_process_run(process, program);
    ASSERT_EQ_(uint16_t, process->stack.sp, 1);
    ASSERT_EQ_(val, process->stack.vals[0], FIX(1597));

    if (0) {
        pc = program;
        /*1*/  OP_IM(PUSH_IM, FIX(25));
        vm_process_stack_clear(process);
        vm_process_run(process, program);
        ASSERT_EQ_(uint16_t, process->stack.sp, 1);
        WRITELN(process->stack.vals[0]);
        ASSERT_EQ(SCM_NUMBER_EQUAL(process->stack.vals[0],
                                   FIXMULINT_TO_SCM(121393)),
                  TRU);
    }

    if (0) {
        pc = program;
        /*1*/  OP_IM(PUSH_IM, FIX(30));
        vm_process_stack_clear(process);
        vm_process_run(process, program);
        ASSERT_EQ_(uint16_t, process->stack.sp, 1);
        WRITELN(process->stack.vals[0]);
        ASSERT_EQ(SCM_NUMBER_EQUAL(process->stack.vals[0],
                                   FIXMULINT_TO_SCM(1346269)),
                  TRU);
    }

    pc = program;
    /*1*/  OP_IM(PUSH_IM, FIX(35));
    bytecode_write_file(program, program_end-program, "fib_35.bytecode");
    if (1) {
        vm_process_stack_clear(process);
        vm_process_run(process, program);
        ASSERT_EQ_(uint16_t, process->stack.sp, 1);
        WRITELN(process->stack.vals[0]);
        ASSERT_EQ(SCM_NUMBER_EQUAL(process->stack.vals[0],
                                   FIXMULINT_TO_SCM(14930352)),
                  TRU);
    }

    // Naive fibonacci again, but with combined opcodes:
    pc = program;
    //(starting at 1 because I previously had a DROP1 instruction here)
    /*1*/  OP_IM(PUSH_IM, FIX(2));
    /*4*/  OP_B(JSR_REL8, 3); //2
    /*6*/  OP(HALT); //1

    // fib: n is last on stack before ret addr
    /*7*/  OP(SWAP); //1
    /*8*/  OP_IM_B(CMPBR_KEEP_LT_IM_REL8, FIX(2), 12); //4   end:
    /*12*/ OP(DEC__DUP); //1
    /*13*/ OP_B(JSR_REL8, -6); //2 fib
    /*15*/ OP(SWAP__DEC); //1
    /*16*/ OP_B(JSR_REL8, -9); //2 fib
    /*18*/ OP(ADD); //1
    /*19*/ OP(RET_POP);//1
    // end:
    /*20*/ OP(DROP1);
    /*21*/ OP_IM(RET_IM, FIX(1));
    // ---
    program_end = pc;
    vm_process_stack_clear(process);
    vm_process_run(process, program);
    ASSERT_EQ_(uint16_t, process->stack.sp, 1);
    ASSERT_EQ_(val, process->stack.vals[0], FIX(2));

    pc = program;
    /*1*/  OP_IM(PUSH_IM, FIX(35));
    bytecode_write_file(program, program_end-program, "fib_combinedop_35.bytecode");
    if (0) {
        vm_process_stack_clear(process);
        vm_process_run(process, program);
        ASSERT_EQ_(uint16_t, process->stack.sp, 1);
        WRITELN(process->stack.vals[0]);
        ASSERT_EQ(SCM_NUMBER_EQUAL(process->stack.vals[0],
                                   FIXMULINT_TO_SCM(14930352)),
                  TRU);
    }


    // Naive fibonacci again, but with "full program compilation as
    // opcode":
    pc = program;
    /*0*/  OP_IM(PUSH_IM, FIX(35));
    /*3*/  OP_B(JSR_REL8, 3); //2
    /*5*/  OP(HALT); //1
    /*6*/  OP(FIB); //1
    program_end = pc;
    bytecode_write_file(program, program_end-program, "fib_compiled_35.bytecode");
    
    if (0) {
        vm_process_stack_clear(process);
        vm_process_run(process, program);
        ASSERT_EQ_(uint16_t, process->stack.sp, 1);
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
    bytecode_write_file(program, program_end-program, "fib_register_35.bytecode");
    
    if (0) {
        vm_process_stack_clear(process);
        vm_process_run(process, program);
        ASSERT_EQ_(uint16_t, process->stack.sp, 1);
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
