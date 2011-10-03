# Copyright (C) 2003, 2006-2009 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

SRC = alloc.c eval.c read.c data.c eval.c global.c main.c error.c print.c
OBJ = $(SRC:.c=.o)
DEP = $(SRC:.c=.d)
BIN = kemacs
LIBS = -lm
CFLAGS += -ggdb -g3 -Wall

all: $(BIN)

kemacs: $(OBJ)
	$(CC) $(CPPFALGS) $(CFLAGS) $(LDFLAGS) $(LIBS) $^  -o $@


PHONY: clean distclean check-syntax


check-syntax:
	$(CC) -Wall -Wextra -pedantic -fsyntax-only $(CHK_SOURCES)



clean:
	rm -f $(OBJ)
	rm -f $(BIN)

distclean: clean
	rm -f $(DEP)
	rm -f TAGS
	rm -f GPATH GRTAGS ID GTAGS GSYMS cscope.* TAGS
	rm -f config.status config.log
	rm -Rf autom4te.cache
	rm -f configure config.h config.h.in
	


%.d: %.c
	set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$


include $(DEP)
