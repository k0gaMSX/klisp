(listp nil)
(listp t)
(listp 1)
(listp 'hola)
(listp '(1 2))
(listp '(1 . 2))
(listp '(1 2 3 4))
(listp '(1 (2 3) 4))
;; Returns
;t
;nil
;nil
;nil
;t
;t
;t
;t
