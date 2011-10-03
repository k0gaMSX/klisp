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

#include "lisp.h"

#include <stdio.h>
#define writestr(x) fputs(x, stdout)

static l_object print_stream;
static void print_object(l_object obj);


static void print_list(l_object obj)
{
        assert(CONSP(obj));

        for (;;) {
                if (ATOM(obj)) {
                        writestr(". ");
                        print_object(obj);
                        break;
                }

                print_object(XCAR(obj));
                obj = XCDR(obj);
                if (!NILP(obj))
                        writestr(" ");
                else
                        break;

        }
}


static void print_object(l_object obj)
{
        extern char iobuf[];

        if (NILP(obj)) {
                writestr("nil");
        } else if (INTEGERP(obj)) {
                snprintf(iobuf, IOBUF_SIZE, "%d", (int) XINT(obj));
                writestr(iobuf);
        } else if (SYMBOLP(obj)) {
                writestr(XSYMBOL(obj)->name);
        } else if (CONSP(obj)) {
                writestr("(");
                print_list(obj);
                writestr(")");
        } else {
                abort();
        }
}




l_object prin1(l_object obj, l_object stream)
{
        print_stream = stream;
        print_object(obj);
        writestr(" ");
        return obj;
}




l_object print(l_object obj, l_object stream)
{
        print_stream = stream;
        prin1(obj, stream);
        writestr("\n");
        return obj;
}
