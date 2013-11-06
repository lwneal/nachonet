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

all: bin/adminTools bin/prototype

bin/adminTools: bin/adminTools.o bin/adminToolsDriver include/adminTools.h 
	${CC} ${CFLAGS} -o bin/adminTools bin/adminTools.o bin/adminToolsDriver.o \
	-lcurl

bin/prototype: bin/prototype.o include/adminTools.h
	${CC} ${CLFAGS} -o bin/prototype bin/prototype.o -lcurl

bin/adminTools.o: src/adminTools.cpp include/adminTools.h 
	${CC} ${CFLAGS} -o bin/adminTools.o -c src/adminTools.cpp
	
bin/adminToolsDriver.o: src/adminToolsDriver.cpp include/adminTools.h
	${CC} ${CFLAGS} -o bin/adminToolsDriver.o -c src/adminToolsDriver.cpp
	
bin/prototype.o: src/prototype.cpp include/adminTools.h
	${CC} ${CFLAGS} -o bin/prototype.o -c src/prototype.cpp

package:
	tar czf NachoNet.tar.gz bin/ include/ src/ Makefile;

clean:
	rm -rf bin/* ;