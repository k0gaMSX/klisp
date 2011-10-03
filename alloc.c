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

#include <assert.h>
#include <stdlib.h>

#include "error.h"
#include "lisp.h"



l_object cons(l_object car, l_object cdr)
{
        extern l_cons cons_array[];
        static unsigned short cnt;
        static l_cons *consp = cons_array;

        if (++consp == cons_array + CONS_MAX)
                out_of_memory();

        consp->car = car;
        consp->cdr = cdr;

        return MAKE_CONS(++cnt);
}



/* malloc returns memory align to 2 */

l_symbol *make_symbol(const char *name)
{
        static l_symbol *symp;

        assert(name);

        if ((symp = malloc(sizeof(l_symbol))) == NULL)
                return NULL;

        assert(!((l_object) symp & 1)); /* be sure of align */

        symp->type = SYM_PTYPE;
        symp->name = name;
        symp->hash = NULL;
        symp->bound = nil;
        symp->fbound.type = FBOUND_NO;

        return symp;
}
