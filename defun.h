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


#ifndef DEFUN_H_
#define DEFUN_H_


#include "types.h"

struct l_builtin {
        const char *name;
        unsigned char type;
        struct l_cfun cfun;
};

#define DEFUN(name, namefun, min, max)                                  \
        {name,FBOUND_BUILTIN, {min, max, namefun}}                      \

#define DEFMACRO(name, namefun, min, max)                               \
        {name, FBOUND_MACROBUILTIN, {min, max, namefun}}                \

/* used for mark a rest parameter in lambda */
#define MANY       255

#define TODEFUN(x,n)  DEFUN_##n(x)
#define DEFUN_1(x) ((l_object (*)(l_object)) x)
#define DEFUN_2(x) ((l_object (*)(l_object, l_object)) x)
#define DEFUN_3(x) ((l_object (*)(l_object, l_object, l_object)) x)
#define DEFUN_MANY(x) ((l_object (*)(l_object *, unsigned char)) x)

extern void initfuncs(struct l_builtin *fp);
extern void initglobals(void);

#endif /* DEFUN_H_ */
