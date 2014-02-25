/*******************************************************************************
File:				json.cpp
Author: 		Josh Siva
Date:				2/22/14
Project:		NachoNet
Purpose:		Implements the behavior of the JSON object
*******************************************************************************/

#include "../../include/util/json.h"



void JSON::init (std::string rawJSON)
{
	char ch;
	int strIndex = 0;
	int curlCount = 0; //add 1 for L brace, subtract for R brace
	std::string key = '\0', val = '\0'; //each time one is added, reset to null




}
bool JSON::getBool (std::string key)
{

}
int JSON::getInt (std::string key)
{

}
float JSON::getFloat (std::string key)
{

}
std::string JSON::getString (std::string key)
{

}
std::vector<jsonVal> JSON::getVector (std::string key)
{

}
void JSON::setValue (std::string key, jsonVal value)
{

}

std::string JSON::writeJSON ()
{

}



