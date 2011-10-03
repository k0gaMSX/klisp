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
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "lisp.h"



enum tokens {
        ERRORTOK = 0, ERRORLEN, NUMBERTOK, STRINGTOK, SYMBOLTOK, EOFTOK
};



static char *yylex;
static unsigned char yytok;
static unsigned char yyerror;

#include <stdio.h>
#define readchar()     getchar()
#define unreadchar(x)   ungetc(x, stdin)






static l_symbol *obarray[16];         /* hash where stay intern symbols */

/* Global  variables   for  dealing  with  lookup/insert   routines  in  the
   hash. This variables improve performance  of the insert, because it needs
   do a previous lookup, that it is always done by usual procedure. */

static l_symbol **hash_base;          /* address where insert */
static l_symbol *hashp, *prevp;       /* hashp = base,prevp = previou */



static char hashfun(const char *str)
{
     register char val = 0;

     assert(str);
     while (*str)
             val = (val + *str++) & 15;

     return val;
}



static l_symbol *lookup(const char *name)
{
        assert(name);

        hash_base = &obarray[hashfun(name)];
        hashp = *hash_base;

        for (prevp = hashp; hashp; prevp = hashp, hashp = hashp->hash) {
                signed char cmp = strcmp(hashp->name, name);

                if (cmp < 0)  continue; /* follow searching */
                if (cmp > 0)  break;    /* not found */

                return hashp;              /* found */
        }

        return NULL;
}



static l_symbol *insert(l_symbol *symp)
{
        assert(symp);
        /* If hashp == prevp means that loop end before of ending of first
         * iteration, so there isn't collision. In other case prevp it is
         * pointing to the last valid item in the collision list, so we
         * have to insert in hash of it */

        if (hashp == prevp) {
                symp->hash = *hash_base;
                *hash_base = symp;
        } else {
                symp->hash = prevp->hash;
                prevp->hash = symp;
        }

        return symp;
}




l_symbol *intern_static(const char *name)
{
        static l_symbol *symp;
        assert(name);

        if ((symp = lookup(name)) != NULL)
                return symp;

        if ((symp = make_symbol(name)) == NULL)
                return NULL;

        return insert(symp);
}



l_symbol *intern(const char *name)
{
        static char *dname;
        static l_symbol *symp;

        assert(name);
        if ((symp = lookup(name)) != NULL)
                return symp;

        if ((dname = strdup(name)) == NULL)
                goto error_dup;

        if ((symp = make_symbol(dname)) == NULL)
                goto error_make_symbol;

        insert(symp);
        return symp;

error_make_symbol:
        free(dname);
error_dup:
        out_of_memory();
        return NULL;            /* Never reached */
}







unsigned char gettoken(void)
{
        extern char iobuf[];
        static int ch;
        static unsigned char cnt;

        cnt = 0;
        yytok = NUMBERTOK;               /* we can be optimist and think it
                                          * will be a number. */
        for (;;) {
                while (isspace(ch = readchar())) /* skip blanks */
                        ;
                if (ch == EOF)
                        return EOFTOK;
                if (ch == ';') {                 /* skip comments */
                        while ((ch = readchar()) != '\n' && ch != EOF)
                                ;       /* nothing */
                        continue;
                }
                break;
        }
        if (!isprint(ch))      /* non printable character, ERROR!!! */
                return ERRORTOK;
        if (strchr("()'\\[]\".#", ch))  /* non atom. */
                return yytok = ch;

        yylex = iobuf;
        /* begin an atom */
        for (;;) {
                if (cnt == IOBUF_SIZE)             /* atom bigger, ERROR!!!! */
                        return ERRORTOK;

                yylex[cnt++] = tolower(ch);
                if (!isdigit(ch))                  /* at least there is a */
                        yytok = SYMBOLTOK;         /* non digit, so isn't a
                                                    * number  */
                ch   = readchar();
                if (isspace(ch) || strchr("\"()'\\.#", ch) || !isprint(ch)) {
                        unreadchar(ch);
                        break;
                }
        }

        yylex[cnt] = '\0';
        return yytok;
}






static l_object read_stream;    /* stream used by read functions */


static l_object readobj(char *dot);

static l_object readlist(void)
{
        l_object first, prev, cur;
        char dot = 0;

        if (gettoken() == ')')
                return nil;

        first = cons(readobj(&dot), nil);

        for (prev = first; gettoken() != ')'; prev = cur) {
                cur = cons(readobj(&dot), nil);
                XCDR(prev) = cur;
        }

        return first;
}



static l_object readobj(char *dot)
{
        assert(dot);

        switch (yytok) {
        case '(':
                return readlist();
        case SYMBOLTOK:
                return MAKE_PTR(intern(yylex));
        case '\'':
                gettoken();
                return cons(quote, cons(readobj(dot), nil));
        case NUMBERTOK:
                return MAKE_INT(atoi(yylex));

        case STRINGTOK:
        case '\\':
        case '.':
                *dot = 1;
                return nil;
        }

        syntax_error();
        return nil;             /* Never reached */
}



/* Read an object lisp as text from stream */
l_object read(l_object stream)
{
        char dot;
        read_stream = stream;

        if (gettoken() == EOFTOK)
                end_of_file();

        return readobj(&dot);
}
