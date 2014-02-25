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

class JSON
{
	public:
    JSON ();
    ~JSON ();
    jsonVal getData(std::string key);
    void setValue (std::string key, jsonVal value);

    std::string writeJSON ();

  private:

    std::map<std::string, jsonVal> keyVal;

};
