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


#ifndef TYPES_H
#define TYPES_H

/* TODO: Add config.h here */


typedef unsigned long l_object;
#define L_OBJECT_BITS (sizeof(l_object) * 8)
#define UPPERBYTE(x)  ((unsigned char) ((l_object) (x) >> (L_OBJECT_BITS - 8)))
#define TO_UPPERBYTE(x) ((l_object) (x) << (L_OBJECT_BITS - 8))


typedef struct {
        l_object car;
        l_object cdr;
} l_cons;


struct l_cfun {
        unsigned char minargs, maxargs;
        void *fun;
};



struct l_fbound {
        unsigned char type;
        union {
                struct l_cfun cfun;
                l_object lfun;
        } u;
};


typedef struct l_symbol {
        unsigned char type;
        const char *name;
        struct l_symbol *hash;
        l_object bound;
        struct l_fbound fbound;
} l_symbol;


#define CONS_MAX   400
#define IOBUF_SIZE 30            /* Size of io buffer */

#endif
