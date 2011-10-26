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
#include <string.h>

#include "error.h"
#include "lisp.h"
#include "defun.h"


/************************************************************************/
/* Type predicates*/

/*
 * (listp OBJECT)
 *
 * Return t if OBJECT is a list, that is, a cons cell or nil.
 * Otherwise, return nil.
 */
static l_object listp(register l_object obj)
{
	return (NILP(obj) || CONSP(obj)) ? tee : nil;
}



/*
 * (numberp OBJECT)
 *
 * Return t if OBJECT is a number (floating point or integer).
 */
static l_object numberp(register l_object obj)
{
        return (INTEGERP(obj)) ? tee : nil;
}



/*
 * (symbolp OBJECT)
 *
 * Return t if OBJECT is a symbol.
 */
static l_object symbolp(register l_object obj)
{
        return (SYMBOLP(obj)) ? tee : nil;
}



/*
 * (atom OBJECT)
 *
 * Return t if OBJECT is not a cons cell.  This includes nil.
 */
static l_object atom(register l_object obj)
{
        return (!CONSP(obj)) ? tee : nil;
}




/*
 * (null OBJECT)
 *
 * Return t if OBJECT is nil.
 */

static l_object null(register l_object obj)
{
        return (NILP(obj)) ? tee : nil;
}




/************************************************************************/
/* Relational operators */


/*
 * (>= NUM1 NUM2)
 *
 * Return t if first arg is greater than or equal to second arg.
 * Both must be numbers.
 */
static l_object cmp_ge(register l_object obj1, register l_object obj2)
{
        static l_object num1, num2;

        if (!INTEGERP(obj1) || !INTEGERP(obj2))
                wrong_type_argument("integer");

        num1 = XINT(obj1), num2 = XINT(obj2);
        return (num1 > num2 || num1 == num2) ? tee : nil;
}


/*
 * (<= NUM1 NUM2)
 *
 * Return t if first arg is less than or equal to second arg.
 * Both must be numbers.
 */
static l_object cmp_le(register l_object obj1, register l_object obj2)
{
        static l_object num1, num2;

        if (!INTEGERP(obj1) || !INTEGERP(obj2))
                wrong_type_argument("integer");

        num1 = XINT(obj1), num2 = XINT(obj2);
        return (num1 < num2 || num1 == num2) ? tee : nil;
}


/*
 * (> NUM1 NUM2)
 *
 * Return t if first arg is greater than second arg.  Both must be numbers.
 */
static l_object cmp_gt(register l_object obj1, register l_object obj2)
{
        if (!INTEGERP(obj1) || !INTEGERP(obj2))
                wrong_type_argument("integer");

        return (XINT(obj1) > XINT(obj2)) ? tee : nil;
}

/*
 * (< NUM1 NUM2)
 *
 * Return t if first arg is less than second arg.  Both must be numbers.
 */
static l_object cmp_lt(register l_object obj1, register l_object obj2)
{
        if (!INTEGERP(obj1) || !INTEGERP(obj2))
                wrong_type_argument("integer");

        return (XINT(obj1) < XINT(obj2)) ? tee : nil;
}


/*
 * (equal O1 O2)
 *
 * Return t if two Lisp objects have similar structure and contents.
 * They must have the same data type.
 * Conses are compared by comparing the cars and the cdrs.
 * Numbers are compared by value.
 * Symbols must match exactly.
 */
static l_object equal(register l_object obj1, register l_object obj2)
{
        static char ret;
        if (CONSP(obj1) && CONSP(obj2))
                ret = XCAR(obj1) == XCAR(obj2) && XCDR(obj1) == XCDR(obj2);
        else
                ret = EQ(obj1, obj2);

        /* TODO: Implement equal for strings */
        return (ret) ? tee : nil;
}

/*
 * (= NUM1 NUM2)
 *
 * Return t if two args, both numbers or markers, are equal.
 */
static l_object eq_int(register l_object obj1, register l_object obj2)
{
        if (!INTEGERP(obj1) || !INTEGERP(obj2))
                wrong_type_argument("integer");

        return (EQ(obj1, obj2)) ? tee : nil;
}


/*
 * (eq OBJ1 OBJ2)
 *
 * Return t if the two args are the same Lisp object.
*/
static l_object eq(register l_object obj1, register l_object obj2)
{
        return (EQ(obj1, obj2)) ? tee : nil;
}


/*
 * (zerop NUMBER)
 *
 * Return t if NUMBER is zero.
 */
static l_object zerop(register l_object obj)
{
        return eq_int(obj, MAKE_INT(0));
}



/************************************************************************/
/* List manipulations functions */

/*
 * (cdr LIST)
 * Return the cdr of LIST.  If arg is nil, return nil.
 * Error if arg is not nil and not a cons cell.  See also `cdr-safe'.
 *
 * See Info node `(elisp)Cons Cells' for a discussion of related basic
 * Lisp concepts such as cdr, car, cons cell and list.
*/
static l_object cdr(register l_object obj)
{
        if (CONSP(obj))
                return XCDR(obj);
        if (!NILP(obj))
                wrong_type_argument("cons");
        return nil;
}


/*
 * (car LIST)
 *
 * Return the car of LIST.  If arg is nil, return nil.
 * Error if arg is not nil and not a cons cell.  See also `car-safe'.
 *
 * See Info node `(elisp)Cons Cells' for a discussion of related basic
 * Lisp concepts such as car, cdr, cons cell and list.
 */
static l_object car(register l_object obj)
{
        if (CONSP(obj))
                return XCAR(obj);
        if (!NILP(obj))
                wrong_type_argument("cons");
        return nil;
}

/*
 * (* &rest NUMBERS-OR-MARKERS)
 *
 * Return product of any number of arguments, which are numbers or markers.
 */
static l_object product(register l_object *args, unsigned char numargs)
{
        register l_object cnt;

        for (cnt = 1, ++numargs; --numargs; --args) {
                if (!INTEGERP(*args))
                    wrong_type_argument("integer");
                cnt *= XINT(*args);
        }

        return MAKE_INT(cnt & INT_MASK);
}


/*
 * (- &optional NUMBER-OR-MARKER &rest MORE-NUMBERS-OR-MARKERS)
 *
 * Negate number or subtract numbers or markers and return the result.
 * With one arg, negates it.  With more than one arg,
 * subtracts all but the first from the first.
 */
static l_object minus(register l_object *args, unsigned char numargs)
{
        register l_sobject cnt = 0;

        if (numargs < 2)
		wrong_number_arguments();
	if (!INTEGERP(*args))
		wrong_type_argument("integer");

        for (cnt = XINT(*args--) ; --numargs; --args) {
                if (!INTEGERP(*args))
                    wrong_type_argument("integer");
                cnt -= XINT(*args);
        }

	if (cnt < 0)
		negative_number();
        /* TODO: We can't represent negative numbers!!!! */
        return MAKE_INT(cnt);
}



/*
 * (+ &rest NUMBERS-OR-MARKERS)
 *
 * Return sum of any number of arguments, which are numbers or markers.
 */
static l_object plus(register l_object *args, unsigned char numargs)
{
        register l_object cnt;

        for (cnt = 0, ++numargs; --numargs; --args) {
                if (!INTEGERP(*args))
                    wrong_type_argument("integer");
                cnt += XINT(*args);
        }


        return MAKE_INT(cnt & INT_MASK);
}


/*
 * (1- NUMBER)
 *
 * Return NUMBER minus one.  NUMBER may be a number or a marker.
 * Markers are converted to integers.
 */
static l_object minus1(register l_object obj)
{
        if (!INTEGERP(obj))
                wrong_type_argument("integer");

        return MAKE_INT((XINT(obj) - 1) & INT_MASK);
}


/*
 * (1+ NUMBER)
 *
 * Return NUMBER plus one.  NUMBER may be a number or a marker.
 * Markers are converted to integers.
*/
static l_object plus1(register l_object obj)
{
        if (!INTEGERP(obj))
                wrong_type_argument("integer");

        return MAKE_INT((XINT(obj) + 1) & INT_MASK);
}




struct l_builtin data_funs[] = {
        DEFMACRO("quote", fquote, 1, 1),
        DEFUN("cons", cons, 2, 2),
        DEFUN("+", plus, 0, MANY),
        DEFUN("1+", plus1, 1, 1),
        DEFUN("1-", minus1, 1, 1),
        DEFUN("-", minus, 0, MANY),
        DEFUN("*", product, 0, MANY),
        DEFUN("cdr", cdr, 1, 1),
        DEFUN("car", car, 1, 1),
        DEFUN("eq", eq, 2, 2),
        DEFUN("zerop", zerop, 1, 1),
        DEFUN("=", eq_int, 2, 2),
        DEFUN("eql", eq, 2, 2),
        DEFUN("equal", equal, 2, 2),
        DEFUN(">=", cmp_ge, 2, 2),
        DEFUN("<=", cmp_le, 2, 2),
        DEFUN(">", cmp_gt, 2, 2),
        DEFUN("<", cmp_lt, 2, 2),
        DEFUN("listp", listp, 1, 1),
        DEFUN("numberp", numberp, 1, 1),
        DEFUN("symbolp", symbolp, 1, 1),
        DEFUN("atom", atom, 1, 1),
        DEFUN("null", null, 1, 1),
        DEFUN("not", null, 1, 1),
        DEFUN(NULL, NULL, 0, 0)
};



