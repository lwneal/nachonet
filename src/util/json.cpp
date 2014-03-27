/*******************************************************************************
File:				json.cpp
Author: 		Josh Siva
Date:				2/22/14
Project:		NachoNet
Purpose:		Implements the behavior of the JSON object
*******************************************************************************/

#include "../../include/util/json.h"
#include <string>

/*******************************************************************************
 * Constructor:	JSON
 *
 * Description: Does nothing
 *
 * Parameters:  None
 *
 * Returned:		None
 ******************************************************************************/
JSON::JSON ()
{

}

/*******************************************************************************
 * Destroyer!:	~JSON
 *
 * Description: Does nothing. There is a damn good reason why clear is not
 * 							called here. JSON objects jump out of scope during parsing so
 * 							when we parse a JSON object as a value we want to keep that
 * 							bit of dynamic memory hanging around. When getObject() is called
 * 							on the jsonParser, a copy of the JSON object is created, and if
 * 							clear() is in the destroyer! this copy will fall out of scope,
 * 							clear() will be called, and we'll end up deleting our dynamic
 * 							memory.
 *
 * Parameters:  None
 *
 * Returned:		None
 ******************************************************************************/
JSON::~JSON ()
{

}

/*******************************************************************************
 * Method:			getData
 *
 * Description: Return the value for the given key
 *
 * Parameters:  key - the key to search for
 *
 * Returned:		jsonData
 ******************************************************************************/
jsonData JSON::getData (std::string key)
{
	return keyVal[key];
}

/*******************************************************************************
 * Method:			setValue
 *
 * Description: Set the value for a given key. If the key exists it is updated,
 *  						and if it doesn't exist it is added.
 *
 * Parameters:  key - the key to search for
 * 							value - the new json data for the key
 *
 * Returned:		None
 ******************************************************************************/
void JSON::setValue (std::string key, jsonData value)
{
	keyVal[key] = value;
}

/*******************************************************************************
 * Method:			clear
 *
 * Description: Clear out the key-values and dispose of any dynamic data.
 *
 * 							***************************************************************
 * 							*IF YOU FILLED THIS OBJECT WITH PARSED JSON THAT MAY INCLUDE  *
 * 							*AN OBJECT AS A VALUE YOU MUST CALL THIS FUNCTION TO FREE THE *
 * 							*DYNAMIC MEMORY.                                              *
 * 							***************************************************************
 *
 * Parameters:  None
 *
 * Returned:		jsonData
 ******************************************************************************/
void JSON::clear ()
{
	for (auto entry : keyVal)
	{
		if (jsonParser::OBJ_TYPE == entry.second.type
				&& NULL != entry.second.value.pObject)
		{
			delete entry.second.value.pObject;
			entry.second.value.pObject = NULL;
		}
	}

	this->keyVal.clear ();
}

/*******************************************************************************
 * Method:			writeJSON
 *
 * Description: Turns the map into proper JSON.
 *
 * Parameters:	prevData - used for recursive calls to writeJSON
 *
 * Returned:		string - JSON string
 ******************************************************************************/
std::string JSON::writeJSON (std::string prevData)
{
	std::string returnString;

	returnString += prevData;

	returnString.push_back ('{');

	for (auto & entry : keyVal)
	{
		returnString.push_back ('\"');
		returnString += entry.first;
		returnString.push_back ('\"');
		returnString.append (" : ");

		returnString += writeValue (entry.second);

		returnString.push_back (',');
	}

	if (0 != returnString.compare ("{"))
	{
		returnString.pop_back ();
	}

	returnString.push_back ('}');

	return returnString;
}

/*******************************************************************************
 * Method:			writeValue
 *
 * Description:	Outputs the correct JSON value given a piece of data
 *
 * Parameters:	data - the data to be turned into JSON
 *
 * Returned:		string - the value as JSON
 ******************************************************************************/
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

				if (1 != returnString.length ())
				{
					returnString.pop_back ();
				}

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



