;;   Copyright (C) 2021 Christian Jaeger, <ch@christianjaeger.ch>
;;   See the LICENSE file that comes bundled with this file.

;; This generates the *opcode_*.h files

;; Config:

(define computed-goto? #t)

;; When using computed-goto, might want to renumber, to keep the table
;; more compact? Benchmarking didn't show any difference, though.
(define renumber? #f)

;; /Config.

;; Size of the dispatch table--tied to the numbers used in
;; normal-opcodes, and would need to add verification of OPCODE if not
;; fitting exactly.
(define num-opcodes 256)


(define (opcode.opnum o) (list-ref o 0))
(define (opcode.name o) (list-ref o 1))
(define (opcode.numargbytes o) (list-ref o 2))
(define (opcode.needspcinc o) (list-ref o 3))
(define (opcode.C-code o) (list-ref o 4))

(load "normal-opcodes.scm")
(load "optim-opcodes.scm")

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

(define registerdecls "
// Registers for (not registered, but copied to process struct and
// back when needed):
val A = process->A;
val B = process->B;
val C = process->C;
val D = process->D;

// Registers for binary data (not registered with GC):
word_t M = 0;
word_t N = 0;

// Stack pointer:
stacksize_t SP = process->stack.sp;

#ifdef DEBUG_SP
# define SPDEBUG0(msg) printf(msg \", now \" FPRI_stacksize \" at %s line %i\\n\", SP, __FILE__, __LINE__)
# define SPDEBUG1(msg, x) printf(msg FPRI_stacksize \", now \" FPRI_stacksize \" at %s line %i\\n\", x, SP, __FILE__, __LINE__)
#else
# define SPDEBUG0(msg)
# define SPDEBUG1(msg, x)
#endif

#define SP_SET(x) do { SP = x; SPDEBUG1(\"SP = \", x); } while (0)
#define SP_INC do { SP++; SPDEBUG0(\"SP++\"); } while (0)
#define SP_DEC do { SP--; SPDEBUG0(\"SP--\"); } while (0)
#define SP_ADD(x) do { SP += x; SPDEBUG1(\"SP += \", x); } while (0)
#define SP_SUB(x) do { SP -= x; SPDEBUG1(\"SP -= \", x); } while (0)


#define _STORE(X) do { process->X = X; } while (0)
#define _RESTORE(X) do { X = process->X; } while (0)
#define _STORESP do { process->stack.sp = SP; } while (0)
#define _RESTORESP do { SP = process->stack.sp; } while (0)
#define STORE_ALL do { _STORESP; _STORE(A); _STORE(B); _STORE(C); _STORE(D); } while (0);
#define RESTORE_ALL do { _RESTORESP; _RESTORE(A); _RESTORE(B); _RESTORE(C); _RESTORE(D); } while (0);
#define STORE_EXCEPT_A do { _STORESP; _STORE(B); _STORE(C); _STORE(D); } while (0);
#define RESTORE_EXCEPT_A do { _RESTORESP; _RESTORE(B); _RESTORE(C); _RESTORE(D); } while (0);
")

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
                (display " };")
                (display registerdecls)
                (display "

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
                (display registerdecls)
                (display "

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
