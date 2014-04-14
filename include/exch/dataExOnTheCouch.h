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
    virtual void setState (std::string state);

    static const double TIMEOUT;
    static const long CURL_TIMEOUT = 30;

    static const int DEFAULT_COUCH_PORT = 5984;
    static const std::string LOCALHOST;

    static enum DB {ADMIN, NODES, DEVICES, ALL} DB;
    static const std::string TARGET_DB[];
    static const std::string ALL_DOCS_Q;

    //these are the keys for all of the JSON pairs we need to use
    static const std::string LOCATION;
    static const std::string ID;
    static const std::string ALT_ID;
    static const std::string IP;
    static const std::string X_COOR;
    static const std::string Y_COOR;
    static const std::string MEASUREMENTS;
    static const std::string DEV_ID;
    static const std::string DISTANCE;
    static const std::string REVISION;
    static const std::string SOURCE;
    static const std::string TARGET;
    static const std::string REPLICATE;
    static const std::string DOC_IDS;
    static const std::string STATE;
    static const std::string MESSAGE;
    static const std::string MSG_TEXT;
    static const std::string MSG_SRC;
    static const std::string DELETED;
    static const std::string TOTAL_ROWS; //used to count docs
    static const std::string ROWS; //used to look through docs in db

	private:
    void updateNodesFromCouch ();
    void updateDevsFromCouch ();
    void updateCouchFromNode ();
    void updateCouchFromDevs ();

    multicast * pNachoCast;
    bool stillGreetingNodes;
    std::thread * pGreeter;


    //from http://www.cplusplus.com/forum/unices/45878/
    CURLcode curlRead(const std::string& url, std::ostream& os,
    										long timeout = CURL_TIMEOUT);
    CURLcode curlPut(const std::string& url, const std::string& json,
    									std::ostream& os);
    CURLcode curlPost(const std::string& url, const std::string& json,
    									std::ostream& os);

    void clearDB (std::string url, std::ostream& os);

    std::map<int, std::string> nodeIPAddr;

    //We need to track the current revision number of each document
    std::map<int, std::string> nodeDBRevisions;
    std::map<std::string, std::string> devDBRevisions;

    std::string myIP;



};
