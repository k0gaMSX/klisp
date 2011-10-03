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
#include <stdlib.h>

#include "lisp.h"

/* TODO: change magic numbers by defines */

int main(int argc, char *argv[])
{
        extern void initsyms(void);
        extern jmp_buf catch_error;

        switch (setjmp(catch_error)) { /* set return point for errors */
        case 0:                        /* handling */
                initsyms();            /* sequential case (setjmp call) */
                break;
        case 1:                        /* error calling */
                return EXIT_FAILURE;
        case 2:                        /* correct EOF */
                return EXIT_SUCCESS;
        }

        for (;;)
                print(eval(read(nil)), nil);

        return EXIT_SUCCESS;    /* Never reached */
}
