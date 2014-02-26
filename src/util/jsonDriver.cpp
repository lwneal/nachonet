/*******************************************************************************
File:				jsonDriver.cpp
Author: 		Josh Siva
Date:				2/25/14
Project:		NachoNet
Purpose:		Tests the functionality of the json parser and writer
*******************************************************************************/

#include "../../include/util/json.h"
#include "../../include/util/jsonParser.h"
#include <iostream>

int main ()
{
	JSON obj;
	jsonParser *pParser;
	jsonData data, element;
	std::string jsonStr;

	std::cout << "Simple BUild:\n";

	data.type = jsonParser::STR_TYPE;
	data.value.strVal = "AA08FF22BB";
	obj.setValue ("_id", data);

	data.type = jsonParser::INT_TYPE;
	data.value.intVal = 123456;
	obj.setValue ("_rev", data);

	data.type = jsonParser::FLT_TYPE;
	data.value.floatVal = 3.45;
	obj.setValue ("x", data);

	data.value.floatVal = 5.233;
	obj.setValue ("y", data);

	std::cout << obj.writeJSON("") << "\n";
	obj.clear ();


	std::cout << "\nType Check Build:\n";

	data.type = jsonParser::INT_TYPE;
	data.value.intVal = 1;
	obj.setValue ("integer", data);

	data.type = jsonParser::FLT_TYPE;
	data.value.floatVal = 2.0;
	obj.setValue ("float", data);

	data.type = jsonParser::STR_TYPE;
	data.value.strVal = "stuff";
	obj.setValue ("string", data);

	data.type = jsonParser::VEC_TYPE;
	element.type = jsonParser::INT_TYPE;

	for (int i = 0; i < 5; i++)
	{
		element.value.intVal = i;
		data.value.array.push_back(element);
	}
	obj.setValue ("array", data);

	data.type = jsonParser::OBJ_TYPE;

	element.type = jsonParser::STR_TYPE;
	element.value.strVal = "Josh";

	data.value.pObject = new JSON;
	data.value.pObject->setValue("First_Name", element);

	element.value.strVal = "Siva";
	data.value.pObject->setValue("Last_Name", element);

	obj.setValue("object", data);

	std::cout << obj.writeJSON ("") << "\n";
	obj.clear ();


	jsonStr = "{\"processed\": false}";

	pParser = new jsonParser (jsonStr);
	obj = pParser->getObject ();

	std::cout << "\nSimple parse, change and print:\n";

	std::cout << obj.writeJSON("") << "\n";

	data.type = jsonParser::BOOL_TYPE;
	data.value.boolVal = true;
	obj.setValue ("processed", data);


	std::cout << obj.writeJSON("") << "\n";

	delete pParser;
	obj.clear ();

	std::cout << "\nParse object as value:\n";

	jsonStr = "{\"object\" : {\"the_answer\" : 42}}";
	pParser = new jsonParser (jsonStr);
	obj = pParser->getObject ();

	std::cout << obj.writeJSON("") << "\n";
	obj.clear ();
	delete pParser;

	std::cout << "\n";

	std::cout << "\nParse, add, print:\n";

	jsonStr =
			"{\"name\" : \"Josh Siva\", \"location\" : {\"x\" : -45.669, \"y\" : 22.33}}";

	std::cout << jsonStr << "\n";

	pParser = new jsonParser (jsonStr);
	obj = pParser->getObject();

	data.type = jsonParser::BOOL_TYPE;
	data.value.boolVal = true;
	obj.setValue ("processed", data);

	std::cout << obj.writeJSON("") << "\n";
	obj.clear ();
	delete pParser;

	std::cout << "\nType Check Parse:\n";

	jsonStr = "{\"int\" : 42, \"float\" : 22.22, \"string\" : \"ahoy! sup?\",";
	jsonStr += "\"array\" : [1.2, 2.3, 3.4], \"obj\" : {\"x\" : 1}, \"bool\" : true}";

	std::cout << jsonStr << "\n";

	pParser = new jsonParser (jsonStr);
	obj = pParser->getObject ();

	if (obj.getData("bool").value.boolVal)
	{
		std::cout << "int: " << obj.getData("int").value.intVal << "\n";
		std::cout << "float: " << obj.getData("float").value.floatVal << "\n";
		std::cout << "string: " << obj.getData("string").value.strVal << "\n";

		std::cout << "array: ";
		for (auto item : obj.getData("array").value.array)
		{
			std::cout << item.value.floatVal << " ";
		}

		std::cout << "\n";

		std::cout << "obj: ";
		std::cout << obj.getData("obj").value.pObject->getData("x").value.intVal
							<< "\n";
	}

	obj.clear ();
	delete pParser;

	return 0;
}


