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
#include "multicast.h"
#include <curl/curl.h>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <thread>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <cstdio>
#include <ctime>


class dataExOnTheCouch : public dataEx
{
	public:
		dataExOnTheCouch ();
		virtual ~dataExOnTheCouch ();

		void setIP (std::string newIP);
		std::string getIP () const;

		void greetNewNode ();

    virtual void ping (Message message); //push msg
    virtual void checkMessages ();
    virtual void pushUpdates (int flag);
    virtual void pullUpdates (int flag);

    static const double TIMEOUT = 60;
    static const long CURL_TIMEOUT = 30;

    static const int DEFAULT_COUCH_PORT = 5984;
    static const std::string LOCALHOST = "127.0.0.1";

    static enum DB {ADMIN, NODES, DEVICES, ALL};
    static const std::string TARGET_DB[] = {"admin_db", "node_db", "dev_db"};
    static const std::string ALL_DOCS_Q = "_all_docs";

    //these are the keys for all of the JSON pairs we need to use
    static const std::string LOCATION = "location";
    static const std::string ID = "_id";
    static const std::string IP = "ip";
    static const std::string X_COOR = "x";
    static const std::string Y_COOR = "y";
    static const std::string MEASUREMENTS = "measurements";
    static const std::string DEV_ID = "devID";
    static const std::string DISTANCE = "dist";
    static const std::string REVISION = "_rev";
    static const std::string SOURCE = "source";
    static const std::string TARGET = "target";
    static const std::string REPLICATE = "_replicate";
    static const std::string DOC_IDS = "doc_ids";
    static const std::string STATE = "state";
    static const std::string MESSAGE = "message";
    static const std::string MSG_TEXT = "msg";
    static const std::string MSG_SRC = "src";
    static const std::string DELETED = "deleted";
    static const std::string TOTAL_ROWS = "total_rows"; //used to count docs
    static const std::string ROWS = "rows"; //used to look through docs in db

	private:
    void updateNodesFromCouch ();
    void updateDevsFromCouch ();
    void updateCouchFromNode ();
    void updateCouchFromDevs ();

    multicast nachoCast;
    bool stillGreetingNodes;
    std::thread * pGreeter;


    //from http://www.cplusplus.com/forum/unices/45878/
    CURLcode curlRead(const std::string& url, std::ostream& os,
    										long timeout = CURL_TIMEOUT);
    CURLcode curlPost(const std::string& url, const std::string& json);

    std::map<int, std::string> nodeIPAddr;

    //We need to track the current revision number of each document
    std::map<int, std::string> nodeDBRevisions;
    std::map<std::string, std::string> devDBRevisions;

    std::string myIP;



};
