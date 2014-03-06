/*******************************************************************************
File:				dataExOnTheCouch.h
Author: 		Josh Siva
Date:				3/3/14
Project:		NachoNet
Purpose:		Defines the interface to the data exchange module on the couch. This
						extends the data exchange module to work with CouchDB
*******************************************************************************/

#pragma once
#include "dataEx.h"
#include "../util/json.h"
#include "../util/jsonParser.h"
#include <curl/curl.h>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <fstream>

#define IP_ADDR_SIZE 4

typedef struct ip
{
  unsigned char addr[IP_ADDR_SIZE];
} ip;


class dataExOnTheCouch : public dataEx
{
	public:
		dataExOnTheCouch ();
		virtual ~dataExOnTheCouch ();

    virtual void pingAll (Message message); //send msg to everyone in net
    virtual void ping (Message message); //push msg and pull to check
    virtual void pushUpdates (int flag);
    virtual void pullUpdates (int flag);

    static const int DEFAULT_COUCH_PORT = 5984;

    static enum DB {ADMIN, NODES, DEVICES, ALL};
    static const std::string TARGET_DB[] = {"admin_db", "node_db", "dev_db"};

    static const std::string LOCATION = "location";
    static const std::string ID = "_id";
    static const std::string X_COOR = "x";
    static const std::string Y_COOR = "y";
    static const std::string MEASUREMENTS = "measurements";
    static const std::string DEV_ID = "devID";
    static const std::string DISTANCE = "dist";
    static const std::string REVISION = "_rev";
    static const std::string SOURCE = "source";
    static const std::string TARGET = "target";
    static const std::string REPLICATE = "_replicate";

	private:
    void updateNodeFromCouch ();
    void updateDevFromCouch ();
    void updateCouchFromNode ();
    void updateCouchFromDev ();

    //from http://www.cplusplus.com/forum/unices/45878/
    CURLcode curlRead(const std::string& url, std::ostream& os,
    										long timeout = 30);
    CURLcode curlPost(const std::string& url, const std::string& json);

    std::map<int, ip> nodeIPAddr;
    ip myIP;



};
