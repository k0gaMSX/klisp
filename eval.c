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


/*
 * eval is a built-in function
 *
 * (eval FORM)
 *
 * Evaluate FORM and return its value.
 */
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





l_object assign(register l_object sym, register l_object var)
{
	assert(SYMBOLP(sym));
	if (!SYMBOLP(var))
		XBOUND(sym) = var;
	else if (BOUNDP(sym))
		XBOUND(sym) = XBOUND(var);
	else
		unbound_variable(var);

	/* TODO: Deal with functional values */
	return sym;
}

/*
 * defvar is a special form
 *
 * (defvar SYMBOL &optional INITVALUE)
 *
 * Define SYMBOL as a variable, and return SYMBOL.
 * You are not required to define a variable in order to use it,
 * but the definition can supply documentation and an initial value
 * in a way that tags can recognize.
 *
 * INITVALUE is evaluated, and used to set SYMBOL, only if SYMBOL's
 * value is void.
 * INITVALUE is optional.
 * If INITVALUE is missing, SYMBOL's value is not set.
 *
 * If SYMBOL has a local binding, then this form affects the local
 * binding.  This is usually not what you want.
 */

l_object defvar(l_object *args, register unsigned char numargs)
{
	register l_object name;

	if (numargs == 0 || numargs > 2)
		wrong_number_arguments();
	name = *args--;
	if (!SYMBOLP(name))
		wrong_type_argument("symbol");
	else if (numargs == 2 && !BOUNDP(name))
		assign(name, eval(*args));

	return name;
}



/***********************************************************/
/* block operators                                         */
/***********************************************************/
/*
 * progn is a special form
 *
 * (progn BODY...)
 *
 *Eval BODY forms sequentially and return value of last one.
 */
static l_object progn(l_object *args, unsigned char numargs)
{
	l_object r = nil;

	while (numargs--)
		r = eval(*args--);
	return r;
}

static l_object prog_list(register l_object list)
{
	l_object r;

	assert(LISTP(list));

	for (r = nil; CONSP(list); list = XCDR(list))
		r = eval(XCAR(list));
	return r;
}




/*
 * cond is a special form
 *
 * (cond CLAUSES...)
 *
 * Try each clause until one succeeds.
 * Each clause looks like (CONDITION BODY...).  CONDITION is evaluated
 * and, if the value is non-nil, this clause succeeds:
 * then the expressions in BODY are evaluated and the last one's
 * value is the value of the cond-form.
 * If no clause succeeds, cond returns nil.
 * If a clause has one element, as in (CONDITION),
 * CONDITION's value if non-nil is returned from the cond-form.
 */
static l_object cond(l_object *args, unsigned char numargs)
{
	l_object r = nil;	/* returned value */

	while (numargs--) {
		register l_object clause = *args--;

		if (NILP(clause)) continue;
		if (!CONSP(clause)) {
			wrong_type_argument("list");
		} else if (!NILP(eval(XCAR(clause)))) {
			r = prog_list(clause);
			break;
		}
	}
	return r;
}



/********************************************************/
/* Logic operators */
/********************************************************/
/*
 * or is a special form
 *
 * (or CONDITIONS...)
 *
 * Eval args until one of them yields non-nil, then return that value.
 * The remaining args are not evalled at all.
 * If all args return nil, return nil.
 */
static l_object
or_fun(register l_object *args, unsigned char numargs)
{
	register l_object r = nil;

	while (numargs--) {
		r = eval(*args--);
		if (!NILP(r))
			break;
	}
	return r;
}



/*
 * and is a special form
 *
 * (and CONDITIONS...)
 *
 * Eval args until one of them yields nil, then return nil.
 * The remaining args are not evalled at all.
 * If no arg yields nil, return the last arg's value.
 *
 */
static l_object
and_fun(register l_object *args, unsigned char numargs)
{
	register l_object r = tee;

	while (numargs--) {
		r = eval(*args--);
		if (NILP(r))
			break;
	}
	return r;
}



struct l_builtin eval_funs[] = {
        DEFMACRO("progn", progn, 0, MANY),
        DEFMACRO("cond", cond, 0, MANY),
        DEFMACRO("or", or_fun, 0, MANY),
	DEFMACRO("and", and_fun, 0, MANY),
	DEFMACRO("defvar", defvar, 0, MANY),
	DEFUN(NULL, NULL, 0, 0)
};
