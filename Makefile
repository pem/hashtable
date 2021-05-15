# Makefile
#
# pem 2001-05-12
#
#

CC=gcc -std=c11

#CCOPTS=-Wpedantic -Wall -Wextra
CCOPTS=-Wpedantic -Wall -Wextra -Werror

#CCDEFS=-D__EXTENSIONS__
CCDEFS=-D_XOPEN_SOURCE=500

#CFLAGS=-g -DDEBUG $(CCOPTS) $(CCDEFS)
CFLAGS=-O2 -fomit-frame-pointer $(CCOPTS) $(CCDEFS)
#CFLAGS=-O2 -pg $(CCOPTS) $(CCDEFS)

#LDFLAGS=-pg

PROG=htabtest htabunit

LIB=libhashtable.a

SRC=hashtable.c htabtest.c htabunit.c

OBJ=$(SRC:%.c=%.o)

all:	$(PROG) $(LIB)

htabtest:	htabtest.o $(LIB)

htabunit:	htabunit.o $(LIB)

$(LIB):	hashtable.o
	rm -f $(LIB)
	$(AR) qc $(LIB) hashtable.o
	ranlib $(LIB)

clean:
	$(RM) $(OBJ) core

cleanall:	clean
	$(RM) $(PROG) $(LIB) make.deps

make.deps:
	gcc -MM $(CFLAGS) $(SRC) > make.deps

include make.deps
