##############################################################################
#File: 		Makefile
#Author: 	Joshua Siva
#Date:		10/21/13
#Project:	NachoNet
#Purpose:	This is the makefile for the NachoNet project
##############################################################################

CC=g++

CFLAGS=-g -Wall

VALGRIND_OPTIONS=-v --leak-check=yes --track-origins=yes

.PHONY: all clean package

all: bin/adminTools

bin/adminTools: bin/adminTools.o include/adminTools.h 
	${CC} ${CFLAGS} -o bin/adminTools bin/adminTools.o -lcurl
	
bin/adminTools.o: src/adminTools.cpp include/adminTools.h 
	${CC} ${CFLAGS} -o bin/adminTools.o -c src/adminTools.cpp

package:
	tar czf NachoNet.tar.gz bin/ include/ src/ Makefile;

clean:
	rm -rf bin/* ;