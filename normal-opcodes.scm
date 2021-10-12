(define normal-opcodes
  '(
    ;; opnum, name, numargbytes, needspcinc, C-code

    (1 loadM_im 2 #t "
M = ARGIM1;")
    (2 loadN_im 2 #t "
N = ARGIM1;")
    (3 loadX_im 2 #t "
X = ARGIM1;")
    (4 loadY_im 2 #t "
Y = ARGIM1;")

    (5 loadA_im 2 #t "
A = ARGIM1;")
    (6 loadB_im 2 #t "
B = ARGIM1;")
    (7 pushA 0 #t "
PUSH(A);")
    (8 pushB 0 #t "
PUSH(B);")
    (9 popA 0 #t "
LET_POP(x);
A = x;")
    (10 popB 0 #t "
LET_POP(x);
B = x;")
    (11 TAB 0 #t "
B = A;")
    (12 TBA 0 #t "
A = B;")
    (13 swapA 0 #t "
STACK_ENSURE_HAS(1);
val x = STACK_UNSAFE_REF(0);
STACK_UNSAFE_SET(0, A);
A = x;")

    (19 pushM 0 #t "
PUSH(FIX(M)); // unsafe")
    (23 pushN 0 #t "
PUSH(FIX(N)); // unsafe")
    (24 swapN 0 #t "
STACK_ENSURE_HAS(1);
val x = STACK_UNSAFE_REF(0);
STACK_UNSAFE_SET(0, FIX(N)); // unsafe
N = INT(x); // unsafe")
    ;; pop into N, push A (call it swapAN ?)
    (130 popN__pushA 0 #t "
STACK_ENSURE_HAS(1);
N = INT(STACK_UNSAFE_REF(0)); // unsafe
STACK_UNSAFE_SET(0, A);")

    
    (14 push_im 2 #t "
PUSH(ARGIM1);")
    (15 drop1 0 #t "
STACK_DROP1;")
    (16 pick_b 1 #t "
// copy the nth-last argument (0 meaning the last, i.e. top element
// of the stack) from the stack
LET_STACK_REF(x, ARGB1);
PUSH(x);")
    (17 swap 0 #t "
STACK_SWAP;")
    (18 dup 0 #t "
STACK_DUP;")
    
    (20 inc 0 #t "
LET_STACK_LAST(x);
STORE_ALL;
STACK_UNSAFE_SET_LAST(SCM_INC(x));
RESTORE_ALL;
")
    (21 inc_ 1 #t "
LET_STACK_REF(x, ARGB1);
STORE_ALL;
PUSH(SCM_INC(x));
RESTORE_ALL;
")
    (22 incA 0 #t "
STORE_EXCEPT_A;
A = SCM_INC(A);
RESTORE_EXCEPT_A;
")
    (122 incX 0 #t "
X++; // XX detect overflows
")
    (123 incY 0 #t "
Y++; // XX detect overflows
")
    (124 TAX 0 #t "
X = INT(A); // XX fixnum check?
")
    (29 TAY 0 #t "
Y = INT(A); // XX fixnum check?
")

    (25 dec 0 #t "
LET_STACK_LAST(x);
STORE_ALL;
STACK_UNSAFE_SET_LAST(SCM_DEC(x));
RESTORE_ALL;
")
    (26 decA 0 #t "
DO_SCM_DEC(STORE_EXCEPT_A, RESTORE_EXCEPT_A, A=, A);")
    (27 decN 0 #t "
N = ((signed_word_t)N) - 1; // unsafe (no overflow/UB check)!
")

    (30 add 0 #t "
STACK_ENSURE_HAS(2);
val res;
DO_SCM_ADD(STORE_ALL, RESTORE_ALL, res=, STACK_UNSAFE_REF(1), STACK_UNSAFE_REF(0))
STACK_UNSAFE_SET(1, res);
STACK_UNSAFE_REMOVE(1);")
    (31 add_im 2 #t "
STACK_ENSURE_HAS(1);
val res;
DO_SCM_ADD(STORE_ALL, RESTORE_ALL, res=, STACK_UNSAFE_REF(0), ARGIM1);
STACK_UNSAFE_SET(0, res);")

    ;; add x y  means stack[x] += stack[y]
    (32 add__ 2 #t "
uint8_t i = ARGB1;
LET_STACK_REF(a, i);
{
    LET_STACK_REF(b, ARGB2);
    STORE_ALL;
    STACK_SET(i, SCM_ADD(a, b));
    RESTORE_ALL;
}
")
    ;; A += pop
    (33 addA 0 #t "
STACK_ENSURE_HAS(1);
DO_SCM_ADD(STORE_EXCEPT_A, RESTORE_EXCEPT_A, A=, A, STACK_UNSAFE_REF(0));
STACK_UNSAFE_REMOVE(1);")

    ;; M += INT(pop) -- unsafe!
    (34 addM 0 #t "
STACK_ENSURE_HAS(1);
M = (signed_dword_t)M + (signed_dword_t)INT(STACK_UNSAFE_REF(0)); // unsafe
STACK_UNSAFE_REMOVE(1);
")

    (35 mul__ 2 #t "
uint8_t i = ARGB1;
LET_STACK_REF(a, i);
{
    LET_STACK_REF(b, ARGB2);
    STORE_ALL;
    STACK_SET(i, SCM_MUL(a, b));
    RESTORE_ALL;
}
")

    (40 bitwise_and 0 #t "
LET_POP(b);
{
    LET_POP(a);
    STORE_ALL;
    PUSH(SCM_BITWISE_AND(a, b));
    RESTORE_ALL;
}
")
    ;; Can be used for booleans and fixnums (both parameters *must* be
    ;; of the same type)
    (41 unsafe_bitwise_and 0 #t "
LET_POP(b);
{
    LET_POP(a);
    PUSH(a & b); // since non-data bits should be identical.
}
")
    (42 unsafe_bitwise_or 0 #t "
LET_POP(b);
{
    LET_POP(a);
    PUSH(a | b); // since non-data bits should be identical.
}
")
    
    (80 jmp_rel8 1 #f "
pc += (int8_t)ARGB1;")
    (81 jmp_rel16 2 #f "
pc += (int16_t)ARGIM1;")
    (85 jsr_rel8 1 #f "
PUSH(PCNUM(PC+2)); //XX make safe
pc += (int8_t)ARGB1;")
    (86 ret 0 #f "
LET_POP(origpc);
SET_PC(PCNUM_TO_WORD(origpc));
")
    (87 ret_im 2 #f "
LET_POP(origpc);
PUSH(ARGIM1);
SET_PC(PCNUM_TO_WORD(origpc));
")
    (88 ret_pop 0 #f "
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
")

    (91 beq_im_rel16 4 #f "
LET_POP(x);
if (x == ARGIM1) {
    pc += (uint16_t)ARGIM2;
} else {
    pc += 5;
}")
    (92 bpos_keep_rel16 2 #f "
// does *not* claim the value on the stack, but peek at it only
LET_STACK_REF(x, 0);
if (INT(x) > 0) {
    pc += (uint16_t)ARGIM1;
} else {
    pc += 3;
}")
    (93 bneg0_keep_rel16 2 #f "
// does *not* claim the value on the stack, but peek at it only
LET_STACK_LAST(x);
if (INT(x) <= 0) {
    pc += (uint16_t)ARGIM1;
} else {
    pc += 3;
}")
    (94 bneg_keep_rel16 2 #f "
// does *not* claim the value on the stack, but peek at it only
LET_STACK_LAST(x);
if (fixnum_is_negative(x)) {
    pc += (uint16_t)ARGIM1;
} else {
    pc += 3;
}")
    (95 bz_keep_rel16 2 #f "
// does *not* claim the value on the stack, but peek at it only
LET_STACK_LAST(x);
if (x == FIX(0)) {
    pc += (uint16_t)ARGIM1;
} else {
    pc += 3;
}")
    (96 bz_rel16 2 #f "
LET_POP(x);
if (x == FIX(0)) {
    pc += (uint16_t)ARGIM1;
} else {
    pc += 3;
}")

    (100 cmpbr_keep_lt_im_rel8 3 #f "
// compare with literal and branch if smaller; (if (< (stack-last) 1234) lbl).
LET_STACK_LAST(x);
// (Optim by checking for fixnum and then doing C-level comparison?
//  Didn't help.)
if (SCM_NUMBER_CMP(x, ARGIM1) == LT) {
    pc += (uint8_t)ARGB3;
} else {
    pc += 4;
}")
    (101 cmpbr_A_lt_im_rel8 3 #f "
// compare with literal and branch if smaller; (if (< registerA 1234) lbl).
if (SCM_NUMBER_CMP(A, ARGIM1) == LT) {
    pc += (uint8_t)ARGB3;
} else {
    pc += 4;
}")
    (102 cmpbr_N_lt_im_rel8 3 #f "
// compare with *binary* literal and branch if smaller; (if (< registerA 1234) lbl).
if (((signed_word_t)N) < ((signed_word_t)ARGIM1)) {
    pc += (uint8_t)ARGB3;
} else {
    pc += 4;
}")

    ;; Some combined opcodes (to test for the performance advantage):
    (200 dec__dup 0 #t "
STACK_ALLOC(1);
STORE_ALL;
val x = SCM_DEC(STACK_UNSAFE_REF(1));
STACK_UNSAFE_SET(1, x);
STACK_UNSAFE_SET(0, x);
RESTORE_ALL;
")
    (201 swap__dec 0 #t "
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
")
    ;; (The following really didn't change ~anything at all [when using gcc]: )
    (202 jsr_rel8__swap 1 #f "
STACK_ENSURE_HAS(1);
val x = STACK_UNSAFE_REF(0);
STACK_UNSAFE_SET(0, PCNUM(PC+2)); //XX make safe
STACK_ALLOC(1);
STACK_UNSAFE_SET(0, x);
pc += (int8_t)ARGB1;")
    ;; / combined opcodes

    ;; Some rarely used instructions:
    (252 trace_on 0 #t "
#ifdef VM_TRACE
  vm_process_trace_on(process, true);
#else
  // XX add WARN_ONCE
  WARN(\"trace_on: tracing not supported (please recompile with VM_TRACE)\");
#endif
")
    (253 trace_off 0 #t "
#ifdef VM_TRACE
  vm_process_trace_on(process, false);
#endif
")
    (254 nop 0 #t "")
    (255 halt 0 #t "
STORE_ALL;
goto halt;"
         ;; ^ can't 'break' out of the while (and computed goto
         ;; variant requires the same anyway)

         ;; In computed goto variant, could optim by putting halt
         ;; label directly, but that would forgo the tracing (and
         ;; perhaps other things in the future).
         )))

