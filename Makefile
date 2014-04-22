##############################################################################
#File: 		Makefile
#Author: 	Joshua Siva
#Date:		2/25/14
#Project:	NachoNet
#Purpose:	This is the makefile for the NachoNet project
##############################################################################

CC=g++-4.7 -std=gnu++11

CFLAGS=-g -Wall

VALGRIND_OPTIONS=-v --leak-check=yes --leak-check=full --track-origins=yes \
--show-reachable=yes

.PHONY: all clean package zeus jsonDriver_valgrind nachonet_valgrind \
map_valgrind dataEx_valgrind addDrop_valgrind devTest_valgrind nodeTest_valgrind \
dataExTests

all: bin/nachonet bin/prototype bin/distDriver bin/configDriver \
bin/collectDriver bin/jsonDriver bin/dataExDriver bin/multicastDriver \
bin/mapDriver bin/configure bin/addDropTest bin/devTest bin/helloTest

########################CONFIGURATION####################################
bin/configure: bin/Configure.o bin/json.o bin/jsonParser.o
	${CC} ${CFLAGS} -o bin/configure bin/Configure.o bin/json.o \
	bin/jsonParser.o -lcurl
	
bin/Configure.o: src/tools/Configure.cpp
	${CC} ${CFLAGS} -o bin/Configure.o -c src/tools/Configure.cpp

########################NACHONET AND TOOLS###############################
bin/nachonet: bin/dataCollect.o bin/stdCollect.o bin/dist.o bin/pathLoss.o \
bin/fsPathLoss.o bin/logNormalShadow.o bin/dataEx.o bin/dataExOnTheCouch.o \
bin/node.o bin/device.o bin/multicast.o bin/loc.o bin/radiotap-parser.o \
bin/main.o bin/Admin.o bin/DataCollectAdmin.o bin/DistMeasureAdmin.o \
bin/DataExAdmin.o bin/LocalizationAdmin.o bin/EZConfig.o bin/json.o \
bin/jsonParser.o bin/Map.o bin/MapOnTheCouch.o bin/DataFeeder.o bin/NachoNet.o
	${CC} ${CFLAGS} -o bin/nachonet bin/dataCollect.o bin/stdCollect.o bin/dist.o\
	 bin/pathLoss.o bin/fsPathLoss.o bin/logNormalShadow.o bin/dataEx.o \
	 bin/dataExOnTheCouch.o bin/node.o bin/device.o bin/multicast.o bin/loc.o \
	 bin/radiotap-parser.o bin/main.o bin/Admin.o bin/DataCollectAdmin.o \
	 bin/DistMeasureAdmin.o bin/DataExAdmin.o bin/LocalizationAdmin.o \
	 bin/EZConfig.o bin/json.o bin/jsonParser.o bin/Map.o bin/MapOnTheCouch.o \
	 bin/DataFeeder.o bin/NachoNet.o -lpcap -lm -lcurl -lpthread -lX11
	 
nachonet_valgrind: bin/nachonet
	valgrind ${VALGRIND_OPTIONS} bin/nachonet
	
nachonet_feed_valgrind: bin/nachonet
	valgrind ${VALGRIND_OPTIONS} bin/nachonet -f testfiles/feed_0.txt
	
bin/main.o: src/tools/main.cpp 
	${CC} ${CFLAGS} -o bin/main.o -c src/tools/main.cpp

bin/NachoNet.o: include/nacho/NachoNet.h src/nacho/NachoNet.cpp
	${CC} ${CFLAGS} -o bin/NachoNet.o -c src/nacho/NachoNet.cpp

bin/Admin.o: include/tools/Admin.h src/tools/Admin.cpp
	${CC} ${CFLAGS} -o bin/Admin.o -c src/tools/Admin.cpp

bin/DataCollectAdmin.o: include/tools/DataCollectAdmin.h \
src/tools/DataCollectAdmin.cpp
	${CC} ${CFLAGS} -o bin/DataCollectAdmin.o -c src/tools/DataCollectAdmin.cpp

bin/DistMeasureAdmin.o: include/tools/DistMeasureAdmin.h \
src/tools/DistMeasureAdmin.cpp
	${CC} ${CFLAGS} -o bin/DistMeasureAdmin.o -c src/tools/DistMeasureAdmin.cpp

bin/DataExAdmin.o: include/tools/DataExAdmin.h src/tools/DataExAdmin.cpp
	${CC} ${CFLAGS} -o bin/DataExAdmin.o -c src/tools/DataExAdmin.cpp

bin/LocalizationAdmin.o: include/tools/LocalizationAdmin.h \
src/tools/LocalizationAdmin.cpp
	${CC} ${CFLAGS} -o bin/LocalizationAdmin.o -c src/tools/LocalizationAdmin.cpp
	
bin/prototype.o: src/prototype.cpp include/tools/adminTools.h
	${CC} ${CFLAGS} -o bin/prototype.o -c src/prototype.cpp
	
########################DIST MEASURE###############################

bin/distDriver: bin/distDriver.o bin/pathLoss.o bin/fsPathLoss.o\
bin/logNormalShadow.o bin/EZConfig.o 
	${CC} ${CFLAGS} -o bin/distDriver bin/distDriver.o bin/pathLoss.o \
	bin/fsPathLoss.o bin/logNormalShadow.o bin/EZConfig.o
	
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
	
bin/DataFeeder.o: src/collect/DataFeeder.cpp include/collect/DataFeeder.h
	${CC} ${CFLAGS} -o bin/DataFeeder.o -c src/collect/DataFeeder.cpp
	
	
##########################Data Exchange###############################	
bin/dataExDriver: bin/dataExDriver.o bin/node.o bin/device.o bin/loc.o \
bin/multicast.o bin/dataEx.o bin/dataExOnTheCouch.o bin/json.o bin/jsonParser.o
	${CC} ${CFLAGS} -o bin/dataExDriver bin/dataExDriver.o bin/node.o \
	bin/device.o bin/loc.o bin/multicast.o bin/dataEx.o bin/dataExOnTheCouch.o \
	bin/json.o bin/jsonParser.o -lcurl
	
bin/dataExDriver.o: src/exch/dataExDriver.cpp
	${CC} ${CFLAGS} -o bin/dataExDriver.o -c src/exch/dataExDriver.cpp

dataExTests: bin/configure bin/addDropTest bin/devTest bin/nodeTest \
bin/helloTest

addDropTest_valgrind: bin/addDropTest
	valgrind ${VALGRIND_OPTIONS} bin/addDropTest

bin/addDropTest: bin/addDropTest.o bin/node.o bin/device.o bin/loc.o \
bin/multicast.o bin/dataEx.o bin/dataExOnTheCouch.o bin/json.o bin/jsonParser.o
	${CC} ${CFLAGS} -o bin/addDropTest bin/addDropTest.o bin/node.o \
	bin/device.o bin/loc.o bin/multicast.o bin/dataEx.o bin/dataExOnTheCouch.o \
	bin/json.o bin/jsonParser.o -lcurl
	
bin/addDropTest.o: src/exch/addDropTest.cpp
	${CC} ${CFLAGS} -o bin/addDropTest.o -c src/exch/addDropTest.cpp
	
devTest_valgrind: bin/devTest
	valgrind ${VALGRIND_OPTIONS} bin/devTest	
	
bin/devTest: bin/devTest.o bin/node.o bin/device.o bin/loc.o \
bin/multicast.o bin/dataEx.o bin/dataExOnTheCouch.o bin/json.o bin/jsonParser.o
	${CC} ${CFLAGS} -o bin/devTest bin/devTest.o bin/node.o \
	bin/device.o bin/loc.o bin/multicast.o bin/dataEx.o bin/dataExOnTheCouch.o \
	bin/json.o bin/jsonParser.o -lcurl

bin/devTest.o: src/exch/devTest.cpp
	${CC} ${CFLAGS} -o bin/devTest.o -c src/exch/devTest.cpp
	
nodeTest_valgrind: bin/nodeTest
	valgrind ${VALGRIND_OPTIONS} bin/nodeTest		
	
bin/nodeTest: bin/nodeTest.o bin/node.o bin/device.o bin/loc.o \
bin/multicast.o bin/dataEx.o bin/dataExOnTheCouch.o bin/json.o bin/jsonParser.o
	${CC} ${CFLAGS} -o bin/nodeTest bin/nodeTest.o bin/node.o \
	bin/device.o bin/loc.o bin/multicast.o bin/dataEx.o bin/dataExOnTheCouch.o \
	bin/json.o bin/jsonParser.o -lcurl

bin/nodeTest.o: src/exch/nodeTest.cpp
	${CC} ${CFLAGS} -o bin/nodeTest.o -c src/exch/nodeTest.cpp
	
helloTest_valgrind: bin/helloTest
	valgrind ${VALGRIND_OPTIONS} bin/helloTest		
	
bin/helloTest: bin/helloTest.o bin/node.o bin/device.o bin/loc.o \
bin/multicast.o bin/dataEx.o bin/dataExOnTheCouch.o bin/json.o bin/jsonParser.o
	${CC} ${CFLAGS} -o bin/helloTest bin/helloTest.o bin/node.o \
	bin/device.o bin/loc.o bin/multicast.o bin/dataEx.o bin/dataExOnTheCouch.o \
	bin/json.o bin/jsonParser.o -lcurl

bin/helloTest.o: src/exch/helloTest.cpp
	${CC} ${CFLAGS} -o bin/helloTest.o -c src/exch/helloTest.cpp	
	
bin/node.o: include/exch/node.h src/exch/node.cpp include/loc/loc.h
	${CC} ${CFLAGS} -o bin/node.o -c src/exch/node.cpp
	
bin/device.o: include/exch/device.h src/exch/device.cpp
	${CC} ${CFLAGS} -o bin/device.o -c src/exch/device.cpp
	
bin/multicast.o: include/exch/multicast.h src/exch/multicast.cpp
	${CC} ${CFLAGS} -o bin/multicast.o -c src/exch/multicast.cpp
	
bin/dataEx.o: include/exch/dataEx.h src/exch/dataEx.cpp
	${CC} ${CFLAGS} -o bin/dataEx.o -c src/exch/dataEx.cpp
	
bin/dataExOnTheCouch.o: include/exch/dataExOnTheCouch.h \
src/exch/dataExOnTheCouch.cpp
	${CC} ${CFLAGS} -o bin/dataExOnTheCouch.o -c src/exch/dataExOnTheCouch.cpp
	
bin/multicastDriver: bin/MulticastDriver.o bin/multicast.o
	${CC} ${CFLAGS} -o bin/multicastDriver bin/MulticastDriver.o bin/multicast.o \
	-pthread

bin/MulticastDriver.o: src/exch/MulticastDriver.cpp include/exch/multicast.h
	${CC} ${CFLAGS} -o bin/MulticastDriver.o -c src/exch/MulticastDriver.cpp 
	
dataEx_valgrind: bin/dataExDriver
	valgrind ${VALGRIND_OPTIONS} bin/dataExDriver
	
dataExNet_valgrind: bin/dataExNetDriver
	valgrind ${VALGRIND_OPTIONS} bin/dataExNetDriver
	
#################################Map#####################################	

bin/mapDriver: bin/MapDriver.o bin/Map.o bin/MapOnTheCouch.o bin/json.o \
bin/jsonParser.o
	${CC} ${CFLAGS} -o bin/mapDriver bin/MapDriver.o bin/Map.o \
	bin/MapOnTheCouch.o bin/json.o bin/jsonParser.o -lcurl -lpthread -lX11 -lm

bin/MapDriver.o: src/map/MapDriver.cpp
	${CC} ${CFLAGS} -o bin/MapDriver.o -c src/map/MapDriver.cpp
	
bin/Map.o: include/map/Map.h src/map/Map.cpp
	${CC} ${CFLAGS} -o bin/Map.o -c src/map/Map.cpp

bin/MapOnTheCouch.o:include/map/MapOnTheCouch.h src/map/MapOnTheCouch.cpp \
extern/CImg-1.5.8/CImg.h
	${CC} ${CFLAGS} -o bin/MapOnTheCouch.o -c src/map/MapOnTheCouch.cpp

map_valgrind: bin/mapDriver
	valgrind ${VALGRIND_OPTIONS} bin/mapDriver

##########################Utilities###############################

bin/ezConfigDriver: bin/EZConfigDriver.o bin/EZConfig.o bin/json.o \
bin/jsonParser.o
	${CC} ${CFLAGS} -o bin/ezConfigDriver bin/EZConfigDriver.o bin/EZConfig.o \
	bin/json.o bin/jsonParser.o
	
bin/EZConfigDriver.o: src/util/EZConfigDriver.cpp include/util/EZConfig.h
	${CC} ${CFLAGS} -o bin/EZConfigDriver.o -c src/util/EZConfigDriver.cpp
	
bin/EZConfig.o: src/util/EZConfig.cpp include/util/EZConfig.h
	${CC} ${CFLAGS} -o bin/EZConfig.o -c src/util/EZConfig.cpp

bin/configDriver: bin/configDriver.o bin/config.o
	${CC} ${CFLAGS} -o bin/configDriver bin/configDriver.o bin/config.o
	
bin/configDriver.o: src/util/configDriver.cpp include/util/config.h
	${CC} ${CFLAGS} -o bin/configDriver.o -c src/util/configDriver.cpp

bin/config.o: include/util/config.h src/util/config.cpp
	${CC} ${CFLAGS} -o bin/config.o -c src/util/config.cpp


bin/jsonDriver: bin/jsonDriver.o bin/json.o bin/jsonParser.o
	${CC} ${CFLAGS} -o bin/jsonDriver bin/jsonDriver.o bin/json.o bin/jsonParser.o
 	
bin/jsonDriver.o: src/util/jsonDriver.cpp include/util/json.h \
include/util/jsonParser.h
	${CC} ${CFLAGS} -o bin/jsonDriver.o -c src/util/jsonDriver.cpp
	
bin/json.o: src/util/json.cpp include/util/json.h src/util/jsonParser.cpp \
include/util/jsonParser.h
	${CC} ${CFLAGS} -o bin/json.o -c src/util/json.cpp
	
bin/jsonParser.o: src/util/json.cpp include/util/json.h src/util/jsonParser.cpp \
include/util/jsonParser.h
	${CC} ${CFLAGS} -o bin/jsonParser.o -c src/util/jsonParser.cpp
	
jsonDriver_valgrind: bin/jsonDriver
	valgrind ${VALGRIND_OPTIONS} bin/jsonDriver
		
########################External Resources###################################

bin/radiotap-parser.o: extern/radiotapParser/radiotap-parser.c \
extern/radiotapParser/radiotap-parser.h
	gcc ${CFLAGS} -o bin/radiotap-parser.o -c \
	extern/radiotapParser/radiotap-parser.c


package:
	tar czf NachoNet.tar.gz bin/ include/ src/ extern/ web/ Makefile dist.json \
	testfiles/;

zeus:
	cd ../ ; tar czf NachoNet.tar.gz NachoNet ; \
	scp NachoNet.tar.gz siva0070@zeus.cs.pacificu.edu:

clean:
	rm -rf bin/* ;
