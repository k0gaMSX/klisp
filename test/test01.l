;; quote
(quote 1)
(quote hola)
'1
'hola
'(hola)
''hola
(quote 'hola)
(quote ('hola adios))
(quote ('hola 'adios))
'(hola . adios)
;; Return
;1
;hola
;1
;hola
;(hola)
;(quote hola)
;(quote hola)
;((quote hola) adios)
;((quote hola) (quote adios))
;(hola . adios)
