/*
  Copyright (C) 2003, 2006-2009 Free Software Foundation, Inc.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/


#ifndef LISP_H
#define LISP_H

#include "types.h"

/*
 * lobject it is only a short, where we can store:
 *
 * ** pointers alignment to 2, because upper bit to 0 means that lobject is
 *    a pointer shifted to right 1 position.
 *
 * ** integers when 2 upper bits are 10, so this let us have integers of 14
 *    bits.
 *
 * ** cons when 3 upper bits are 110, so this let us have 2^13 different
 *    cons cell (8192, that means 32K of memory)
 *
 * In other case we have a char (there is 10 bits for this ...)
 */


#include <stddef.h>

#define PTR_MASK     ((l_object) -1 >> 1)
#define INT_MASK     ((l_object) -1 >> 2)
#define CONS_MASK    ((l_object) -1 >> 3)
#define CHAR_MASK    ((l_object) -1 >> 4)

#define SYM_PTYPE    '\x81'
#define STR_PTYPE    '\x82'
#define VEC_PTYPE    '\x83'
#define PTR_TYPE     0
#define INT_TYPE     0x80
#define CONS_TYPE    0xc0


#define XPTR(x)      ((unsigned char *) ((l_object) (x) << 1))
#define MAKE_PTR(x)   ((l_object) (x) >> 1)
#define MAKE_INT(x)   ((l_object) (x) | TO_UPPERBYTE(INT_TYPE))
#define MAKE_CONS(x)  ((l_object) (x) | TO_UPPERBYTE(CONS_TYPE))

#define XPTYPE(x)    (*XPTR(x))
#define XINT(x)      ((x) & INT_MASK)
#define XCONS(x)     (cons_array[(x) & CONS_MASK])
#define XSTRING(x)   (XPTR(x) + 1)
#define XSYMBOL(x)   ((l_symbol *) XPTR(x))
#define XCHAR(x)     ((x) & CHAR_MASK)
#define XCAR(x)      (XCONS(x).car)
#define XCDR(x)      (XCONS(x).cdr)

#define POINTERP(x)  ((UPPERBYTE(x) & ~UPPERBYTE(PTR_MASK)) == PTR_TYPE)
#define INTEGERP(x)  ((UPPERBYTE(x) & ~UPPERBYTE(INT_MASK)) == INT_TYPE)
#define CONSP(x)     ((UPPERBYTE(x) & ~UPPERBYTE(CONS_MASK)) == CONS_TYPE)
#define TYPEP(x,y)   (POINTERP(x) && XPTYPE(x) == (unsigned char) (y))
#define SYMBOLP(x)   TYPEP(x, SYM_PTYPE)
#define STRINGP(x)   TYPEP(x, STR_PTYPE)
#define VECTORP(x)   TYPEP(x, VEC_PTYPE)
#define NILP(x)      ((x) == nil)
#define LISTP(x)     (NILP(x) || CONSP(x))
#define ATOM(x)      (!CONSP(x))


#define FBOUND_MACRO            0x80
#define FBOUND_NO               0
#define FBOUND_BUILTIN          1
#define FBOUND_LISP             2
#define FBOUND_MACROBUILTIN     (FBOUND_MACRO | FBOUND_BUILTIN)
#define FBOUND_MACROLISP        (FBOUND_MACRO | FBOUND_LISP)

#define EQ(x,y)     ((x) == (y))

#define XFBOUND(x)  (&XSYMBOL(x)->fbound)
#define XBOUND(x)   (XSYMBOL(x)->bound)
#define FBOUNDP(x)  (XFBOUND(x)->type != FBOUND_NO)
#define BOUNDP(x)   (!EQ(XBOUND(x), nil))

extern l_object nil, tee, quote;
extern l_cons cons_array[];

extern l_object cons(l_object car, l_object cdr);
extern l_object prin1(l_object obj, l_object stream);
extern l_object print(l_object obj, l_object stream);
extern l_object read(l_object stream);
extern l_object eval(l_object obj);
extern l_object fquote(l_object obj);

extern l_symbol *make_symbol(const char *name);
extern l_symbol *intern_static(const char *name);
extern l_symbol *intern(const char *name);
#endif
