CC			= gcc
CCPP		= g++
CO			= -o
COPC		= -ansi -pedantic -Wall
COPT		= -O3
CDEBUG	=

LIB1		= -lz

lzw: lzw.cpp
	$(CCPP) $(COPC) $(COPT) $(CDEBUG) lzw.cpp $(CO) lzw

zlib: zlib.c
	$(CC) $(COPC) $(COPT) $(CDEBUG) $(LIB1) zlib.c $(CO) zlib

block_sort: block_sort.cpp
	$(CCPP) $(COPC) $(COPT) $(CDEBUG) block_sort.cpp $(CO) block_sort

clean: 
	rm -f *~ *.o *.gch
