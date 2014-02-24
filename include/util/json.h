/*******************************************************************************
File:				json.h
Author: 		Josh Siva
Date:				2/22/14
Project:		NachoNet
Purpose:		Defines the interface for the json object which is used to parse
						and create json text.
*******************************************************************************/

#pragma once
#include <map>

union jsonVal
{
	int intVal;
	std::string strVal;
	float floatVal;
	JSON object;
	std::vector<jsonVal> array;
	bool boolVal;
} jsonVal;

class JSON
{
	public:
    JSON (std::string rawJSON);
    ~JSON ();
    void init (std::string rawJSON);
    bool getBool (std::string key);
    int getInt (std::string key);
    float getFloat (std::string key);
    std::string getString (std::string key);
    std::vector<jsonVal> getVector (std::string key);
    void setValue (std::string key, jsonVal value);

    std::string writeJSON ();

    enum Type {NULL, INT, STRING, BOOL, FLOAT, ARRAY, OBJECT};

    const char L_CURL = '{';
    const char R_CURL = '}';
    const char L_BRKT = '[';
    const char R_BRKT = ']';
    const char COMMA = ',';
    const char COLON = ':';

  private:
    bool validJSON;
    std::map<std::string, jsonVal> keyVal;

};
