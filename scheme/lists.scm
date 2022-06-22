#lang racket
(require "naturalnums.scm")

(define pushBack
  (lambda (l)
    (lambda (x)
      (lambda (f)
        (lambda (e)
          ((f ((l f) e)) x))))))

(define list0
  (lambda (f)
    (lambda (e)
      e)))

(define list1
  (lambda (x)
        ((pushBack list0) x)))

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
    ((l (lambda (a) (lambda (b) ((pushFront a) b) ))) list0)))

(define invertBinaryFunction
  (lambda (f)
    (lambda (a)
      (lambda (b)
        ((f b) a)))))

(define foldr
  (lambda (l)
    (lambda (f)
      (lambda (e)
        (((revList l) (invertBinaryFunction f)) e)))))

(define map
  (lambda(l)
    (lambda (f)
      ((l (lambda (a) (lambda (b) ((pushBack a) (f b))))) list0))))

(define filter
  (lambda(l)
    (lambda (f)
      ((l
        (lambda (a) (lambda (b) (if (f b) ((pushBack a) b) a) )))
        list0))))

(define member
  (lambda (l)
    (lambda (x)
      ((l (lambda(r) (lambda (y) ((r c#t) ((c= x) y)))))
       c#f))))  
