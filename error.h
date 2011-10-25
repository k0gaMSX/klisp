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

#ifndef ERROR_H_
#define ERROR_H_

#include "types.h"

void out_of_memory(void);
void syntax_error(const char *msg);
void unbound_variable(l_object var);
void wrong_type_argument(char *type);
void huge_list_error(void);
void void_function_error(void);
void wrong_number_arguments(void);
void end_of_file(void);
extern void negative_number(void);
extern void eof_while_parsing(void);
#endif /* ERROR_H_ */


