##############################################################################
#File: 		Makefile
#Author: 	Joshua Siva
#Date:		10/21/13
#Project:	NachoNet
#Purpose:	This is the makefile for the NachoNet project
##############################################################################

CC=g++ -std=gnu++11

CFLAGS=-g -Wall

VALGRIND_OPTIONS=-v --leak-check=yes --track-origins=yes

.PHONY: all clean package zeus

all: bin/adminTools bin/prototype bin/distDriver bin/configDriver

########################ADMIN TOOLS###############################

bin/adminTools: bin/adminTools.o bin/adminToolsDriver.o \
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
	
bin/prototype.o: src/prototype.cpp include/tools/adminTools.h
	${CC} ${CFLAGS} -o bin/prototype.o -c src/prototype.cpp
	
########################DIST MEASURE###############################

bin/distDriver: bin/distDriver.o bin/pathLoss.o bin/fsPathLoss.o\
bin/logNormalShadow.o bin/config.o 
	${CC} ${CFLAGS} -o bin/distDriver bin/distDriver.o bin/pathLoss.o \
	bin/fsPathLoss.o bin/logNormalShadow.o bin/config.o
	
bin/distDriver.o: src/dist/distDriver.cpp include/dist/dist.h
	${CC} ${CFLAGS} -o bin/distDriver.o -c src/dist/distDriver.cpp
	
bin/pathLoss.o: include/dist/pathLoss.h include/dist/dist.h \
src/dist/pathLoss.cpp
	${CC} ${CFLAGS} -o bin/pathLoss.o -c src/dist/pathLoss.cpp -lm
	
bin/fsPathLoss.o: include/dist/fsPathLoss.h include/dist/dist.h \
src/dist/fsPathLoss.cpp
	${CC} ${CFLAGS} -o bin/fsPathLoss.o -c src/dist/fsPathLoss.cpp -lm
	
bin/logNormalShadow.o: include/dist/logNormalShadow.h include/dist/dist.h\
src/dist/logNormalShadow.cpp
	${CC} ${CFLAGS} -o bin/logNormalShadow.o -c src/dist/logNormalShadow.cpp \
	-lm

##########################Utilities###############################

bin/configDriver: bin/configDriver.o bin/config.o
	${CC} ${CFLAGS} -o bin/configDriver bin/configDriver.o bin/config.o
	
bin/configDriver.o: src/util/configDriver.cpp include/util/config.h
	${CC} ${CFLAGS} -o bin/configDriver.o -c src/util/configDriver.cpp

bin/config.o: include/util/config.h src/util/config.cpp
	${CC} ${CFLAGS} -o bin/config.o -c src/util/config.cpp

package:
	tar czf NachoNet.tar.gz bin/ include/ src/ Makefile;

zeus:
	cd ../ ; tar czf NachoNet.tar.gz NachoNet ; \
	scp NachoNet.tar.gz siva0070@zeus.cs.pacificu.edu:

clean:
	rm -rf bin/* ;
