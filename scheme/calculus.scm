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

(define printn
  (lambda (n)
    ((n (lambda (x) (+ x 1))) 0)))

(define c+
  (lambda (n)
    (lambda (m)
      (lambda (f)
        (lambda (x)
          ((n f) ((m f) x)))))))

(define c#t
  (lambda (x)
    (lambda (y)
      x)))

(define c#f
  (lambda (x)
    (lambda (y)
      y)))

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

(define pushBack
  (lambda (l)
    (lambda (x)
      (lambda (f)
        (lambda (e)
          ((f ((l f) e)) x))))))

(define list
  (lambda (f)
    (lambda (e)
      e)))

(define list1
  (lambda (x)
        ((pushBack list) x)))

(define list2
  (lambda (x)
    (lambda (y)
      ((pushBack (list1 x)) y))))

(define depthCount
  (lambda (a)
    (lambda (b)
      ((c+ c1) a))))

(define length
  (lambda (l)
    ((l depthCount) c0)))

(define foldl
  (lambda (l)
    (lambda (f)
      (lambda (e)
        ((l f) e)))))

(define pushFront
  (lambda (l)
    (lambda (x)
      (lambda (f)
        (lambda (e)
          ((l f) (((list1 x) f) e)))))))

(define revList
  (lambda (l)
    ((l (lambda (a) (lambda (b) ((pushFront a) b) ))) list)))

(define invertBinaryFunction
  (lambda (f)
    (lambda (a)
      (lambda (b)
        ((f b) a)))))

(define foldr
  (lambda (l)
    (lambda (f)
      (lambda (e)
        ((l (invertBinaryFunction f)) e)))))

(define map
  (lambda(l)
    (lambda (f)
      ((l (lambda (a) (lambda (b) ((pushBack a) (f b))))) list))))