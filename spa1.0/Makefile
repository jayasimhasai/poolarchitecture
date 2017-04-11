CC = gcc
CCXEON = icc
CFLAGSX86 = -g -pipe -O3 -ftree-vectorize -ftree-vectorizer-verbose=0 -msse3 -march=native -mtune=native -finline-functions -pthread -std=gnu99 
CFLAGSXEON = -mmic -pipe -O0 -ftree-vectorize -march=native -mtune=native -finline-functions -pthread -std=gnu99
OS =$(shell uname -s)

X86:

	$(CC) $(CFLAGSX86) spa.c -o spa

xeon:

	$(CCXEON) $(CFLAGSXEON) spa.c -o spaxeon
