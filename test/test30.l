(cond)
(cond nil)
(cond (nil))
(cond (nil t))
(cond (nil 1) (nil 2) (t))
(cond (nil 1) (nil 2) (t 1))
(cond ((car nil) 1) (nil 2) (t 3))
;; Returns
;nil
;nil
;nil
;nil
;t
;1
;3
