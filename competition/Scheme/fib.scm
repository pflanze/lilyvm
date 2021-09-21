'(declare (block)
         (standard-bindings)
         (extended-bindings))

(define (fib n)
  (if (< n 2)
      1
      (+ (fib (- n 1))
         (fib (- n 2)))))

(write (fib 35))
(newline)
