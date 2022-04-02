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

(define c0 (natural 0))
(define c1 (natural 1))
(define c2 (natural 2))
(define c3 (natural 3))
(define c4 (natural 4))
(define c5 (natural 5))
(define c6 (natural 6))
(define c7 (natural 7))
(define c8 (natural 8))
(define c9 (natural 9))
(define c10 (natural 10))
(define c11 (natural 11))

(define c#t
  (lambda (x)
    (lambda (y)
      x)))

(define c#f
  (lambda (x)
    (lambda (y)
      y)))

(define lOR
  (lambda (p)
    (lambda (q)
      ((p c#t) q))))

(define lAND
  (lambda (p)
    (lambda (q)
      ((p q) c#f))))

(define lNOT
  (lambda (p)
    ((p c#f) c#t)))

(define cPair
  (lambda (x)
    (lambda (y)
      (lambda (z)
        ((z x) y)))))

(define cFirst
  (lambda (p)
    (p c#t)))

(define cSecond
  (lambda (p)
    (p c#f)))

(define printn
  (lambda (n)
    ((n (lambda (x) (+ x 1))) 0)))

(define cNext
  (lambda (n)
    (lambda (f)
      (lambda (x)
        (f ((n f) x))))))

(define c+
  (lambda (n)
    (lambda (m)
      (lambda (f)
        (lambda (x)
          ((n f) ((m f) x)))))))

(define c++
 (lambda (n)
   (lambda (m)
     ;(lambda (f)
     ;  (lambda (x)
         ((n cNext) m))))

(define cPlusShano
 (lambda (n)
   (lambda (m)
     (lambda (f)
       (lambda (x)
         (((repeated (printn n)) f) ((m f) x)))))))

(define c*
  (lambda (n)
    (lambda (m)
      (lambda (f)
        (lambda (x)
          ((n (m f)) x))))))

(define c**
  (lambda (n)
    (lambda (m)
          ((n (c+ m)) (natural 0)))))

(define c^
  (lambda (n)
    (lambda (m)
      ((n (c* m)) (natural 1)))))

(define cPredecessor
  (lambda (n)
    (cSecond ((n (lambda (x) ((cPair (cNext (cFirst x))) (cFirst x)))) ((cPair c0) c0)))))

(define c-
  (lambda (n)
    (lambda (m)
      ((m cPredecessor) n))))

(define equals0
  (lambda (n)
    ((n (lambda (x) c#f)) c#t)))

(define c=
  (lambda (n)
    (lambda (m)
      ((lAND (equals0 ((c- n) m))) (equals0 ((c- m) n))))))

(define c<
  (lambda (n)
    (lambda (m)
      (lNOT (equals0 ((c- m) n))))))

(define c<=
  (lambda (n)
    (lambda (m)
      ((lOR ((c= n) m)) (lNOT (equals0 ((c- m) n)))))))

(define cQuot
  (lambda (m)
    (lambda (n)
      (cFirst
        ((m
          (lambda (p)
            ((((c<= ((c+ (cSecond p)) n)) m)
              ((cPair (cNext (cFirst p))) ((c+ n) (cSecond p))))
              p)))
         ((cPair c0) c0))))))

(define cRem
  (lambda (m)
    (lambda (n)
     ((c- m)  
      (cSecond
        ((m
          (lambda (p)
            ((((c<= ((c+ (cSecond p)) n)) m)
              ((cPair (cNext (cFirst p))) ((c+ n) (cSecond p))))
              p)))
         ((cPair c0) c0)))))))

(define cPrime
  (lambda (n)
    (cSecond
     ((((c- n) c2)
       (lambda (p)
         ((cPair (cNext (cFirst p))) ((lAND (cSecond p)) (lNOT (equals0 ((cRem n) (cFirst p))))) )))
       ((cPair c2) c#t)))))