##############################################################################
#File: 		Makefile
#Author: 	Joshua Siva
#Date:		1/27/14
#Project:	NachoNet
#Purpose:	This is the makefile for the NachoNet project
##############################################################################

CC=g++-4.7 -std=gnu++11

CFLAGS=-g -Wall

VALGRIND_OPTIONS=-v --leak-check=yes --track-origins=yes

.PHONY: all clean package zeus

all: bin/adminTools bin/prototype bin/distDriver bin/configDriver \
bin/collectDriver

########################ADMIN TOOLS###############################

bin/adminTools: bin/adminTools.o bin/adminToolsDriver.o \
bin/adminDistMeasure.o bin/pathLoss.o bin/fsPathLoss.o bin/logNormalShadow.o \
bin/dist.o bin/config.o bin/loc.o bin/adminLocalization.o bin/dataCollect.o \
bin/stdCollect.o bin/radiotap-parser.o bin/adminDataCollect.o \
include/tools/adminTools.h 
	${CC} ${CFLAGS} -o bin/adminTools bin/adminTools.o bin/adminToolsDriver.o \
	bin/adminDistMeasure.o bin/pathLoss.o bin/fsPathLoss.o bin/logNormalShadow.o \
	bin/dist.o bin/config.o bin/loc.o bin/adminLocalization.o bin/dataCollect.o \
	bin/stdCollect.o bin/radiotap-parser.o bin/adminDataCollect.o -lcurl -lpcap

bin/prototype: bin/prototype.o include/tools/adminTools.h
	${CC} ${CLFAGS} -o bin/prototype bin/prototype.o -lcurl

bin/adminTools.o: src/tools/adminTools.cpp include/tools/adminTools.h \
src/tools/adminDistMeasure.cpp include/tools/adminDistMeasure.h 
	${CC} ${CFLAGS} -o bin/adminTools.o -c src/tools/adminTools.cpp
	
bin/adminToolsDriver.o: src/tools/adminToolsDriver.cpp \
include/tools/adminTools.h
	${CC} ${CFLAGS} -o bin/adminToolsDriver.o -c src/tools/adminToolsDriver.cpp

bin/adminDistMeasure.o: src/tools/adminDistMeasure.cpp \
include/tools/adminDistMeasure.h src/dist/pathLoss.cpp src/dist/fsPathLoss.cpp \
src/dist/logNormalShadow.cpp src/util/config.cpp include/util/config.h \
src/dist/dist.cpp include/dist/dist.h
	${CC} ${CFLAGS} -o bin/adminDistMeasure.o -c src/tools/adminDistMeasure.cpp
	
bin/adminLocalization.o: src/tools/adminLocalization.cpp \
include/tools/adminLocalization.h include/loc/loc.h src/loc/loc.cpp
	${CC} ${CFLAGS} -o bin/adminLocalization.o -c \
	src/tools/adminLocalization.cpp
	
bin/adminDataCollect.o: src/tools/adminDataCollect.cpp \
include/tools/adminDataCollect.h include/collect/dataCollect.h \
include/collect/stdCollect.h src/collect/dataCollect.cpp \
src/collect/stdCollect.cpp
	${CC} ${CFLAGS} -o bin/adminDataCollect.o -c src/tools/adminDataCollect.cpp
	
bin/prototype.o: src/prototype.cpp include/tools/adminTools.h
	${CC} ${CFLAGS} -o bin/prototype.o -c src/prototype.cpp
	
########################DIST MEASURE###############################

bin/distDriver: bin/distDriver.o bin/pathLoss.o bin/fsPathLoss.o\
bin/logNormalShadow.o bin/config.o 
	${CC} ${CFLAGS} -o bin/distDriver bin/distDriver.o bin/pathLoss.o \
	bin/fsPathLoss.o bin/logNormalShadow.o bin/config.o
	
bin/distDriver.o: src/dist/distDriver.cpp include/dist/dist.h
	${CC} ${CFLAGS} -o bin/distDriver.o -c src/dist/distDriver.cpp
	
bin/dist.o: src/dist/dist.cpp include/dist/dist.h
	${CC} ${CFLAGS} -o bin/dist.o -c src/dist/dist.cpp	
	
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

##########################Localization###############################
bin/loc.o: include/loc/loc.h src/loc/loc.cpp
	${CC} ${CFLAGS} -o bin/loc.o -c src/loc/loc.cpp -lm
	
##########################Data Collection###############################
bin/collectDriver: bin/dataCollectDriver.o bin/stdCollect.o bin/dataCollect.o \
bin/radiotap-parser.o
	${CC} ${CFLAGS} -o bin/collectDriver bin/dataCollectDriver.o bin/stdCollect.o\
	 bin/dataCollect.o bin/radiotap-parser.o -lpcap
	 
bin/dataCollectDriver.o: src/collect/dataCollectDriver.cpp \
include/collect/dataCollect.h include/collect/stdCollect.h
	${CC} ${CFLAGS} -o bin/dataCollectDriver.o -c \
	src/collect/dataCollectDriver.cpp
	
bin/stdCollect.o: src/collect/stdCollect.cpp include/collect/stdCollect.h \
include/collect/dataCollect.h extern/radiotapParser/radiotap-parser.h
	${CC} ${CFLAGS} -o bin/stdCollect.o -c src/collect/stdCollect.cpp
	
bin/dataCollect.o: src/collect/dataCollect.cpp include/collect/dataCollect.h
	${CC} ${CFLAGS} -o bin/dataCollect.o -c src/collect/dataCollect.cpp

##########################Utilities###############################

bin/configDriver: bin/configDriver.o bin/config.o
	${CC} ${CFLAGS} -o bin/configDriver bin/configDriver.o bin/config.o
	
bin/configDriver.o: src/util/configDriver.cpp include/util/config.h
	${CC} ${CFLAGS} -o bin/configDriver.o -c src/util/configDriver.cpp

bin/config.o: include/util/config.h src/util/config.cpp
	${CC} ${CFLAGS} -o bin/config.o -c src/util/config.cpp
	
########################External Resources###################################

bin/radiotap-parser.o: extern/radiotapParser/radiotap-parser.c \
extern/radiotapParser/radiotap-parser.h
	gcc ${CFLAGS} -o bin/radiotap-parser.o -c \
	extern/radiotapParser/radiotap-parser.c


package:
	tar czf NachoNet.tar.gz bin/ include/ src/ extern/ Makefile;

zeus:
	cd ../ ; tar czf NachoNet.tar.gz NachoNet ; \
	scp NachoNet.tar.gz siva0070@zeus.cs.pacificu.edu:

clean:
	rm -rf bin/* ;
