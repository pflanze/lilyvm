;;   Copyright (C) 2021 Christian Jaeger, <ch@christianjaeger.ch>
;;   See the LICENSE file that comes bundled with this file.

;; This generates the *opcode_*.h files

;; Config:

(define computed-goto? #t)

;; When using computed-goto, might want to renumber, to keep the table
;; more compact? Benchmarking didn't show any difference, though.
(define renumber? #f)

;; Including undefined ones
(define num-opcodes 256)

;; /Config.


(define (opcode.opnum o) (list-ref o 0))
(define (opcode.name o) (list-ref o 1))
(define (opcode.numargbytes o) (list-ref o 2))
(define (opcode.needspcinc o) (list-ref o 3))
(define (opcode.C-code o) (list-ref o 4))

(define normal-opcodes
  '(
    ;; opnum, name, numargbytes, needspcinc, C-code
    (10 push_im 2 #t "
PUSH(ARGIM1);")
    (11 drop1 0 #t "
STACK_DROP1;")
    (12 pick_b 1 #t "
// copy the nth-last argument (0 meaning the last, i.e. top element
// of the stack) from the stack
LET_STACK_REF(x, ARGB1);
PUSH(x);")
    (13 swap 0 #t "
STACK_SWAP;")
    (14 dup 0 #t "
STACK_DUP;")
    
    (20 inc 0 #t "
/*
LET_POP(x);
PUSH(SCM_INC(x));
*/
LET_STACK_LAST(x);
STACK_UNSAFE_SET_LAST(SCM_INC(x));")
    (21 inc_ 1 #t "
LET_STACK_REF(x, ARGB1);
PUSH(SCM_INC(x));")

    (25 dec 0 #t "
/*
LET_POP(x);
PUSH(SCM_DEC(x));
*/
LET_STACK_LAST(x);
STACK_UNSAFE_SET_LAST(SCM_DEC(x));")

    (30 add 0 #t "
/*
LET_POP(b);
{
    LET_POP(a);
    PUSH(SCM_ADD(a, b));
}
*/
STACK_ENSURE(2);
STACK_UNSAFE_SET(1, SCM_ADD(STACK_UNSAFE_REF(1), STACK_UNSAFE_REF(0)));
STACK_UNSAFE_REMOVE(1);
")
    (31 add_im 2 #t "
LET_POP(x);
PUSH(SCM_ADD(x, ARGIM1));")

    ;; add x y  means s[x] += s[y]
    (32 add__ 2 #t "
uint8_t i = ARGB1;
LET_STACK_REF(a, i);
{
    LET_STACK_REF(b, ARGB2);
    STACK_SET(i, SCM_ADD(a, b));
}
")
    (33 mul__ 2 #t "
uint8_t i = ARGB1;
LET_STACK_REF(a, i);
{
    LET_STACK_REF(b, ARGB2);
    STACK_SET(i, SCM_MUL(a, b));
}
")

    (40 bitwise_and 0 #t "
LET_POP(b);
{
    LET_POP(a);
    PUSH(SCM_BITWISE_AND(a, b));
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
STACK_ENSURE(2);
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
// compare and branch; (if (< x 1234) lbl).
// The literal must be a fixnum; x must be a number.
LET_STACK_LAST(x);
// (Optim by checking for fixnum and then doing C-level comparison?
//  Didn't help.)
if (SCM_NUMBER_CMP(x, ARGIM1) == LT) {
    pc += (uint8_t)ARGB3;
} else {
    pc += 4;
}")

    ;; Some combined opcodes (to test for the performance advantage):
    (200 dec__dup 0 #t "
/*
LET_STACK_LAST(x);
STACK_UNSAFE_SET_LAST(SCM_DEC(x));
STACK_DUP;
*/
STACK_ALLOC(1);
val x = SCM_DEC(STACK_UNSAFE_REF(1));
STACK_UNSAFE_SET(1, x);
STACK_UNSAFE_SET(0, x);")
    (201 swap__dec 0 #t "
if (1) { // actually faster??
    STACK_SWAP;
    LET_STACK_LAST(x);
    STACK_UNSAFE_SET_LAST(SCM_DEC(x));
} else {
    STACK_ENSURE(2);
    val tmp = SCM_DEC(STACK_UNSAFE_REF(1));
    STACK_UNSAFE_SET(1, STACK_UNSAFE_REF(0));
    STACK_UNSAFE_SET(0, tmp);
}
")
    ;; / combined opcodes

    ;; Some rarely used instructions:
    (252 trace_on 0 #t "
#ifdef VM_TRACE
  vm_process_trace_on(process, true);
#else
  // XX add WARN_ONCE
  WARN(\"tracing not supported (please recompile with VM_TRACE)\");
#endif
")
    (253 trace_off 0 #t "
#ifdef VM_TRACE
  vm_process_trace_on(process, false);
#endif
")
    (254 nop 0 #t "")
    (255 halt 0 #t "
goto halt;"
         ;; ^ can't 'break' out of the while (and computed goto
         ;; variant requires the same anyway)

         ;; In computed goto variant, could optim by putting halt
         ;; label directly, but that would forgo the tracing (and
         ;; perhaps other things in the future).
         )))

(define optim-opcodes
  ;; These depend on `computed-goto?` being #t (meaning,
  ;; fib_register_35.bytecode won't work and vmtest can't be compiled)
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
    STACK_ENSURE(2);
    STACK_UNSAFE_SET(1, SCM_ADD(STACK_UNSAFE_REF(1), STACK_UNSAFE_REF(0)));
    STACK_UNSAFE_REMOVE(1);
}
// RET_POP
STACK_ENSURE(2);
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


// Change calling conventions (reg1 is used for the first argument
// and the return value):
STACK_SWAP;
{
    LET_POP(n);
    reg1 = n;
}
VAL_REGISTER(reg1);
// jsr fib_with_registers_entry
PUSH(FIX((uintptr_t)&&fib_with_registers_end_calling_conventions
         - (uintptr_t)&&fib_with_registers_entry));
goto fib_with_registers_entry;
fib_with_registers_end_calling_conventions:
PUSH(reg1);
VAL_UNREGISTER(1);
STACK_SWAP;
goto op_ret;


// entry with register based calling conventions:
fib_with_registers_entry:
// (CMPBR_KEEP_LT_IM_REL8, FIX(2), 12)
{
#define x reg1
    if (SCM_NUMBER_CMP(x, FIX(2)) == LT) {
        // LET_POP(origpc);
        // STACK_ENSURE(1);
#define origpc tmp1
        origpc = STACK_UNSAFE_REF(0);
        STACK_UNSAFE_REMOVE(1);
        reg1 = FIX(1);
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
    reg1 = POSITIVEFIXNUM_UNSAFE_DEC(reg1);
#else
    reg1 = SCM_DEC(reg1);
#endif
    PUSH(reg1);
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
    STACK_UNSAFE_SET_LAST(reg1);
#ifdef FIXNUM_UNSAFE
    reg1 = POSITIVEFIXNUM_UNSAFE_DEC(oldx);
#else
    reg1 = SCM_DEC(oldx);
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
    // STACK_ENSURE(1); // optim: leave off
    reg1 = SCM_ADD(reg1, STACK_UNSAFE_REF(0));
    STACK_UNSAFE_REMOVE(1);
}
// RET_POP
//STACK_ENSURE(1); // optim: leave off
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

(define opcodes
  ;; can only use optim-opcodes with computed-goto
  (if computed-goto?
      (append normal-opcodes optim-opcodes)
      normal-opcodes))

(if (not renumber?)
    ;; check against duplicates
    (let ((v (make-vector num-opcodes #f)))
      (for-each (lambda (o)
                  (let ((n (opcode.opnum o)))
                    (if (vector-ref v n)
                        (error "opcode number used multiple times:" n))
                    (vector-set! v n #t)))
                opcodes)))


(define (display* . strs)
  (for-each display strs))

(define (string-upcase str)
  (list->string (map char-upcase (string->list str))))

(define (indent str n)
  (let* ((len (string-length str))
         (i0 (let skipwhitespace ((i 0))
               (if (< i (string-length str))
                   (let ((c (string-ref str i)))
                     (case c
                       ((#\newline #\tab #\space)
                        (skipwhitespace (+ i 1)))
                       (else
                        i)))
                   i)))
         (out (make-string (* len 2) #\space)))
    (let lp ((i i0)
             (j 0))
      (if (< i len)
          (let ((c (string-ref str i)))
            (string-set! out j c)
            (set! j (+ j 1))
            (case c
              ((#\newline)
               (let lp ((i n))
                 (if (> i 0)
                     (begin
                       (string-set! out j #\space)
                       (set! j (+ j 1))
                       (lp (- i 1)))
                     'end)))
              (else
               #f))
            (lp (+ i 1) j))
          (substring out 0 j)))))

(define (print-file-generated-by)
  (display "/* DO NOT EDIT. This file has been generated by opcodes.scm */\n\n"))

(define (name.computed-goto-label name)
  (string-append "op_" (symbol->string name)))

(define (print-opcodes_dispatch_h)
  (let lp ((init-code-strings '())
           (dispatch-code-strings '())
           (opcodes opcodes)
           (i 0))
    (if (not (null? opcodes))
        (let* ((opcode (car opcodes))

               (opnum (opcode.opnum opcode))
               (name (opcode.name opcode))
               (numargbytes (opcode.numargbytes opcode))
               (dopcinc (opcode.needspcinc opcode))
               (C (opcode.C-code opcode))

               (goto-label (and computed-goto?
                                (name.computed-goto-label name))))
          (lp
           (if computed-goto?
               (cons (cons (if renumber? i opnum)
                           (string-append "&&" goto-label))
                     init-code-strings)
               #f)
           (cons
            (indent
             (string-append
              (if computed-goto?
                  goto-label
                  (string-append
                   "case " (number->string (if renumber? i opnum))))
              ": /* "
              (symbol->string name)
              ", "
              (number->string numargbytes)
              " */\n"
              "    TRACE_OP(" (object->string (symbol->string name)) ");\n"
              "    {\n"
              "        " (indent C 8) "\n"
              "    }\n"
              (if dopcinc
                  (string-append
                   "    pc += " (number->string (+ numargbytes 1)) ";\n")
                  "")
              (if computed-goto?
                  "    DISPATCH;\n"
                  "    break;\n"))
             4)
            dispatch-code-strings)
           (cdr opcodes)
           (+ i 1)))

        ;; Print
        (begin
          (print-file-generated-by)
          (if computed-goto?

              ;; Label based dispatch
              (begin
                (display* "
{
    static void* op2label[" (number->string num-opcodes) "] = { ")
                (let ((a (make-vector num-opcodes "&&invalid_op")))
                  (for-each (lambda (i+str)
                              (vector-set! a (car i+str) (cdr i+str)))
                            init-code-strings)
                  (let lp ((i 0))
                    (if (< i num-opcodes)
                        (begin
                          (display (vector-ref a i))
                          (if (not (= i (- num-opcodes 1)))
                              (display ", "))
                          (lp (+ i 1))))))
                (display " };
    val reg1, reg2; // register variables, to be registered with VAL_REGISTER
    val tmp1; // not to be registered with VAL_REGISTER (to avoid local vars)

#define DISPATCH                             \\
    {                                        \\
        MEM_VERIFY;                          \\
        goto *op2label[OPCODE];              \\
    }

    DISPATCH;

    ")
                (for-each display (reverse dispatch-code-strings))
                (display "
    invalid_op:
        DIE_(\"invalid opcode %i at PC %\" PRIuPTR, OPCODE, PC);
}
"))
              
              ;; Switch based dispatch
              (begin
                (display "
val reg1, reg2; // register variables, to be registered with VAL_REGISTER
val tmp1; // not to be registered with VAL_REGISTER (to avoid local vars)

while (1) {
    MEM_VERIFY;
    switch (OPCODE) {

    ")
                (for-each display (reverse dispatch-code-strings))
                (display "
    default:
        DIE_(\"invalid opcode %i at PC %\" PRIuPTR, OPCODE, PC);
    }
}
")))))))

(define (roundup-log2 x)
  (integer-length (- x 1)))

(define (binary-rounded-length n)
  (arithmetic-shift 1 (roundup-log2 n)))

(define (number->hex-string n)
  (let* ((str (number->string n 16))
         (len (string-length str))
         (desired-len (max 2 (binary-rounded-length len))))
    (string-append (make-string (- desired-len len) #\0)
                   str)))

(define (print-opcodes_constants_h)
  (print-file-generated-by)
  (display "#ifndef _OPCODE_CONSTANTS_H_\n")
  (display "#define _OPCODE_CONSTANTS_H_\n\n")
  (display "//                          opcode  /* hex; numargbytes */\n")
  (let lp ((opcodes opcodes)
           (i 0))
    (if (null? opcodes)
        (display "\n#endif /* _OPCODE_CONSTANTS_H_ */\n")
        (let* ((opcode (car opcodes))

               (opnum (opcode.opnum opcode))
               (name (opcode.name opcode))
               (numargbytes (opcode.numargbytes opcode))
               (dopcinc (opcode.needspcinc opcode))
               (C (opcode.C-code opcode))

               (ucname (string-upcase (symbol->string name))))
          (let ((opnum* (if renumber? i opnum)))
            (display*
             "#define OP_" ucname (make-string (- 25 (string-length ucname))
                                               #\space)
             " " (number->string opnum*)
             " /* "
             "0x" (number->hex-string opnum*)
             "; "
             (number->string numargbytes)
             " */\n"))
          (lp (cdr opcodes) (+ i 1))))))

(with-output-to-file "_opcode_dispatch.h" print-opcodes_dispatch_h)
(with-output-to-file "opcode_constants.h" print-opcodes_constants_h)
