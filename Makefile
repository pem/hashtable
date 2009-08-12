# Makefile
#
# Per-Erik Martin (pmartin@rsasecurity.com) 2001-05-12
#
#

CC=gcc -std=c99

CCOPTS=-pedantic -Wall

#CCDEFS=-D__EXTENSIONS__
CCDEFS=-D_XOPEN_SOURCE=500

CFLAGS=-g -DDEBUG $(CCOPTS) $(CCDEFS)
#CFLAGS=-O2 -fomit-frame-pointer $(CCOPTS) $(CCDEFS)
#CFLAGS=-O2 -pg $(CCOPTS) $(CCDEFS)

#LDFLAGS=-pg

PROG=hash-string wstats htabtest

SRC=hash-string.c wstats.c hashtable.c htabtest.c

OBJ=$(SRC:%.c=%.o)

all:	$(PROG)

hash-string:	hash-string.o

wstats:	wstats.o

htabtest:	htabtest.o hashtable.o

clean:
	$(RM) $(OBJ) core

cleanall:	clean
	$(RM) $(PROG) $(LIB) make.deps

make.deps:
	gcc -MM $(CFLAGS) $(SRC) > make.deps

include make.deps
