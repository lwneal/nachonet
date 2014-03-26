/*******************************************************************************
File:				MapOnTheCouch.h
Author: 		Josh Siva
Date:				3/25/14
Project:		NachoNet
Purpose:		The interface of the Map objected implemented through CouchDB
*******************************************************************************/

#pragma once
#include "Map.h"
#include "../util/json.h"
#include "../util/jsonParser.h"
#include "../../extern/CImg-1.5.8/CImg.h"
#include <curl/curl.h>
#include <sstream>
#include <sys/stat.h>
#include <fcntl.h>

class MapOnTheCouch : public Map
{
	public:
		virtual void save ();
		virtual void load ();

		static const std::string RES_LOCATION;
		static const long CURL_TIMEOUT = 30;

		//JSON keys
		static const std::string FILE_NAME;
		static const std::string IMG_DIM;
		static const std::string MAX_DIM;
		static const std::string X;
		static const std::string Y;
		static const std::string WIDTH;
		static const std::string HEIGHT;
		static const std::string REVISION;
		static const std::string RESPONSE_REV; //the key for a rev on a response is
																					 //different

	private:
		//from http://www.cplusplus.com/forum/unices/45878/
    CURLcode curlRead(const std::string& url, std::ostream& os,
    										long timeout = CURL_TIMEOUT);
    CURLcode curlPutImage(const std::string& url);
    CURLcode curlPutJSON(const std::string& url, const std::string& json,
    										 std::ostream& os);
};
