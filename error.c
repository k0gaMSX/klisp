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

#include <setjmp.h>
#include <stdarg.h>

#include "lisp.h"
#include "error.h"




#include <stdio.h>

extern jmp_buf catch_error;


static void error(const char *fmt, ...)
{
        va_list va;
        va_start(va, fmt);
        vfprintf(stderr, fmt, va);
        va_end(va);
        longjmp(catch_error, 1);
}

/* TODO: change magic numbers by defines */

void end_of_file(void)
{
        longjmp(catch_error, 2);
}

void wrong_number_arguments(void)
{
        fprintf(stderr, "Wrong number of arguments\n");
        longjmp(catch_error, 1);
}

void void_function_error(void)
{
        fprintf(stderr, "void-function error\n");
        longjmp(catch_error, 1);
}


void huge_list_error(void)
{
        fprintf(stderr, "Too much elements in the list\n");
        longjmp(catch_error, 1);
}



void wrong_type_argument(char *type)
{
        error("wrong type argument %s\n", type);
}


void unbound_variable(l_object var)
{
        error("unbound-variable %s\n", XSYMBOL(var)->name);
}

void out_of_memory(void)
{
        fprintf(stderr, "Out of memory\n");
        longjmp(catch_error, 1);
}


void syntax_error(void)
{
        fprintf(stderr, "Syntax error\n");
        longjmp(catch_error, 1);
}
/* TODO: Change fprintf to error(..) */
