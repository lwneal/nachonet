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

bin/adminTools: bin/adminTools.o bin/adminToolsDriver \
include/tools/adminTools.h 
	${CC} ${CFLAGS} -o bin/adminTools bin/adminTools.o bin/adminToolsDriver.o \
	-lcurl

bin/prototype: bin/prototype.o include/tools/adminTools.h
	${CC} ${CLFAGS} -o bin/prototype bin/prototype.o -lcurl

bin/adminTools.o: src/tools/adminTools.cpp include/tools/adminTools.h 
	${CC} ${CFLAGS} -o bin/adminTools.o -c src/tools/adminTools.cpp
	
bin/adminToolsDriver.o: src/tools/adminToolsDriver.cpp \
include/tools/adminTools.h
	${CC} ${CFLAGS} -o bin/adminToolsDriver.o -c src/tools/adminToolsDriver.cpp
	
bin/prototype.o: src/prototype.cpp include/adminTools.h
	${CC} ${CFLAGS} -o bin/prototype.o -c src/prototype.cpp

package:
	tar czf NachoNet.tar.gz bin/ include/ src/ Makefile;

clean:
	rm -rf bin/* ;