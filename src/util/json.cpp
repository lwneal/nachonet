/*******************************************************************************
File:				json.cpp
Author: 		Josh Siva
Date:				2/22/14
Project:		NachoNet
Purpose:		Implements the behavior of the JSON object
*******************************************************************************/

#include "../../include/util/json.h"

JSON::JSON (std::string rawJSON)
{
	init (rawJSON);
}
JSON::~JSON ()
{

}
void JSON::init (std::string rawJSON)
{
	char ch;
	int strIndex = 0;
	int curlCount = 0; //add 1 for L brace, subtract for R brace
	std::string key = '\0', val = '\0'; //each time one is added, reset to null
	Type currentType = NULL;

	validJSON = false;

	ch = rawJSON[strIndex++];

	if (L_CURL == ch)
	{
		curlCount++;
		validJSON = true;
	}

	while (0 != curlCount && validJSON)
	{
		ch = rawJSON[strIndex++];

		switch (ch)
		{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '8':
			case '9':
				if (NULL == currentType)
				{
					currentType = INT;
				}
				else if (INT == currentType || FLOAT == currentType)
				{
					val.push_back(ch);
				}
				break;

			default:
				if (NULL == currentType)
				{
					currentType = STRING;
				}


				break;
		}
	}


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



