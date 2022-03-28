#lang racket

;this implements the definition f^n(x) = f^(n-1)(f(x))
(define repeated
  (lambda (n)
    (lambda (f)
      (lambda (x)
        (if(= n 0) 
           x
           (((repeated (- n 1)) f) (f x)))))))

(define natural
  (lambda (n)
    (lambda (f)
      (lambda (x)
        (((repeated n) f) x)))))

(define printn
  (lambda (n)
    ((n (lambda (x) (+ x 1))) 0)))

(define numNext
  (lambda (n)
    (lambda (f)
      (lambda (x)
        (f ((n f) x))))))

(define num+
  (lambda (n)
    (lambda (m)
      (lambda (f)
        (lambda (x)
          ((n f) ((m f) x)))))))

(define num++
 (lambda (n)
   (lambda (m)
     ;(lambda (f)
     ;  (lambda (x)
         ((n numNext) m))))

(define numPlusShano
 (lambda (n)
   (lambda (m)
     (lambda (f)
       (lambda (x)
         (((repeated (printn n)) f) ((m f) x)))))))

(define num*
  (lambda (n)
    (lambda (m)
      (lambda (f)
        (lambda (x)
          ((n (m f)) x))))))

(define num**
  (lambda (n)
    (lambda (m)
          ((n (num+ m)) (natural 0)))))

(define num^
  (lambda (n)
    (lambda (m)
      ((n (num* m)) (natural 1)))))
