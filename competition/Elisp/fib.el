(defun fib (n)
  (if (< n 2)
      1
      (+ (fib (- n 1))
         (fib (- n 2)))))

;; (benchmark-run-compiled (fib 35))
;; (25.889063921 372 16.041082739999993)
;; (benchmark-run (fib 35))
;; (26.924896678 373 16.882963493999995)
