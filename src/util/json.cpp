/*******************************************************************************
File:				json.cpp
Author: 		Josh Siva
Date:				2/22/14
Project:		NachoNet
Purpose:		Implements the behavior of the JSON object
*******************************************************************************/

#include "../../include/util/json.h"
#include <string>


JSON::JSON ()
{

}

JSON::~JSON ()
{

}

jsonData JSON::getData (std::string key)
{
	return keyVal[key];
}

void JSON::setValue (std::string key, jsonData value)
{
	keyVal[key] = value;
}

void JSON::clear ()
{
	this->keyVal.clear ();
}

std::string JSON::writeJSON (std::string prevData)
{
	std::string returnString;

	returnString += prevData;

	returnString.push_back ('{');

	for (auto entry : keyVal)
	{
		returnString.push_back ('\"');
		returnString += entry.first;
		returnString.push_back ('\"');
		returnString.append (" : ");

		returnString += writeValue (entry.second);

		returnString.push_back (',');
	}

	returnString.pop_back ();

	returnString.push_back ('}');

	return returnString;
}

std::string JSON::writeValue (jsonData data)
{
	std::string returnString;

	switch (data.type)
		{
			case jsonParser::INT_TYPE:
				returnString += std::to_string (data.value.intVal);
				break;

			case jsonParser::FLT_TYPE:
				returnString += std::to_string (data.value.floatVal);
				break;

			case jsonParser::STR_TYPE:
				returnString.push_back ('\"');
				returnString += data.value.strVal;
				returnString.push_back ('\"');
				break;

			case jsonParser::OBJ_TYPE:
				returnString += data.value.pObject->writeJSON (returnString);
				break;

			case jsonParser::VEC_TYPE:

				returnString.push_back ('[');

				for (auto value : data.value.array)
				{
					returnString += writeValue (value);
					returnString.push_back(',');
				}

				returnString.pop_back ();

				returnString.push_back (']');

				break;

			case jsonParser::BOOL_TYPE:
				if (data.value.boolVal)
				{
					returnString.append ("true");
				}
				else
				{
					returnString.append ("false");
				}
				break;
		}

	return returnString;
}



