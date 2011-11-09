(defvar hola 1)
(defvar cont 0)
(defvar lista '(1 2 3 4 5))
(while (consp lista)
  (setq hola (+ hola (car lista)))
  (setq lista (cdr lista)))
hola
(while nil)
(setq hola 5)
(while (not (zerop (setq hola (1- hola))))
  (setq cont (1+ cont)))
cont
;; Returns
;hola
;cont
;lista
;nil
;16
;nil
;5
;4
;4
