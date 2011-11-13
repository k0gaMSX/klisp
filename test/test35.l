(let ((x 1)) x)
(let ((x (+ 1 2))) x)
(defvar x 3)
x
(let ((x 2)) (+ x 2))
x
(let ((x 1)
      (y 2)
      (z 3))
  (* (+ x y) z))
;; Returns
;1
;3
;x
;3
;4
;3
;9
