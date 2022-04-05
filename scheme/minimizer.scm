#lang racket
(require "naturalnums.scm")

(define Gamma
  (lambda (F)
    (lambda (f)
      (lambda (y)
        ((((c= (f y)) c0)
          y)
          (lambda (a) (((F f) (cNext y)) a)) )))))

(define Z
  (lambda (f)
    ((lambda (x) (f (lambda (y) ((x x) y))))
     (lambda (x) (f (lambda (y) ((x x) y)))))))

(define minimizer
  (lambda (f)
    (((Z Gamma) f) c0)))

(define f1
  (lambda(x)
    ((((c= x) c0) c3)
     ((((c= x) c1) c2)
      ((((c= x) c2) c0)
       c7)))))

(define f2
  (lambda (x)
    ((((c= x) c8) c0) c1)))

(define f3
  (lambda (x)
    c7))

;(minimizer f1) = c2
;(minimizer f2) = c8
;(minimizer f3) = infinite loop
