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

#include <alloca.h>
#include <assert.h>
#include <stdlib.h>

#include "defun.h"
#include "error.h"
#include "lisp.h"



l_object fquote(l_object obj)
{
        return obj;
}


l_object eval(l_object obj);


l_object funcall(l_object fun, l_object *args, unsigned char numargs)
{
        static struct l_fbound *fboundp;

        assert(args && SYMBOLP(fun) && FBOUNDP(fun));

        fboundp = XFBOUND(fun);

        if (fboundp->type & FBOUND_BUILTIN) {
                static l_object par1, par2, par3;
                static void *fun;
                static unsigned char max;

                assert(fboundp->u.cfun.minargs <= numargs);
                assert(fboundp->u.cfun.maxargs == numargs ||
                       fboundp->u.cfun.maxargs == MANY);

                fun = fboundp->u.cfun.fun;
                max = fboundp->u.cfun.maxargs;
                switch (max) {
                case 3:
                        par3 = args[-2];
                case 2:
                        par2 = args[-1];
                case 1:
                        par1 = args[0];
                case MANY:
                        break;
                default:
                        abort();    /* increment this if it is necessary */
                }


                switch (max) {
                case 1:
                        return TODEFUN(fun, 1)(par1);
                case 2:
                        return TODEFUN(fun, 2)(par1, par2);
                case 3:
                        return TODEFUN(fun, 3)(par1, par2, par3);
                case MANY:
                        return TODEFUN(fun, MANY)(args, numargs);
                }

        } else {
                abort();        /* TODO: implement lisp functions */
        }

        return nil;             /* Never reached */
}


/* TODO: Uses a safe alloca version */


static l_object eval_list(l_object list)
{
        l_object *args, fun;
        register l_object *argp;
        register unsigned char numargs;
        struct l_fbound *fboundp;
        unsigned char type;

        args = argp = alloca(sizeof(l_object));
        fun = *argp = XCAR(list);
        --args;

        if (!SYMBOLP(fun))
                void_function_error();

        fboundp = XFBOUND(fun);
        if ((type = fboundp->type) == FBOUND_NO)
                void_function_error();


        for (numargs = 0; list = XCDR(list), !NILP(list); ++numargs) {
                static l_object aux;
                if (numargs == 254)                     /* Don't wrap!!! */
                        huge_list_error();

                alloca(sizeof(l_object));
                aux = XCAR(list);
                if (type & FBOUND_MACRO)
                        *--argp = aux;
                else
                        *--argp = eval(aux);
        }

        if ((type & FBOUND_BUILTIN)) {
                static unsigned char maxargs, minargs;
                maxargs = fboundp->u.cfun.maxargs;       /* numargs < 254 */
                minargs = fboundp->u.cfun.minargs;       /* MANY = 255 */

                if (maxargs == MANY)           /* don't test number of */
                        goto funcall;          /* parameters */


                if (minargs > numargs || maxargs < numargs)
                        wrong_number_arguments();

                while (numargs < maxargs) {
                        alloca(sizeof(l_object));
                        *--argp = nil;
                        ++numargs;
                }
        }

funcall:
        return funcall(fun, args, numargs);
}


l_object eval(l_object obj)
{
        if (NILP(obj)) {
                return nil;
        } else if (INTEGERP(obj)) {
                return obj;
        } else if (SYMBOLP(obj)) {
                if (BOUNDP(obj))
                        return XBOUND(obj);
                else
                        unbound_variable(obj);
        } else if (CONSP(obj)) {
                return eval_list(obj);
        }

        abort();
        return nil;             /* never reached */
}
