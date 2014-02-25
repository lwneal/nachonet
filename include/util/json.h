/*******************************************************************************
File:				json.h
Author: 		Josh Siva
Date:				2/22/14
Project:		NachoNet
Purpose:		Defines the interface for the json object which is used to parse
						and create json text.
*******************************************************************************/

#pragma once
#include "jsonParser.h"
#include <map>
#include <string>

struct jsonData;

class JSON
{
	public:
    JSON ();
    ~JSON ();
    jsonData getData(std::string key);
    void setValue (std::string key, jsonData value);
    void clear ();

    std::string writeJSON (std::string prevData);

  private:
    std::string writeValue (jsonData data);
    std::map<std::string, jsonData> keyVal;

};
