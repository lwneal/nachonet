/*******************************************************************************
File:				EZConfigDriver.cpp
Author: 		Josh Siva
Date:				3/21/14
Project:		NachoNet
Purpose:		Tests some of the features of the EZConfig object.
*******************************************************************************/

#include "../../include/util/EZConfig.h"

void failure(const char * message)
{
	std::cout << "FAIL: " << message << std::endl;
}

int main ()
{
	EZConfig myConfig ("testfiles/ezTest.json"),
					 otherConfig ("testfiles/ezTest.json");
	EZConfig badConfig ("testfiles/ezTest.conf");
	jsonData myData;
	JSON * pData;

	pData = myConfig.getSection ("myVal");

	if (!myConfig.sectionExists ("myVal"))
	{
		failure ("did not return true on existent section");
	}

	if (-22 != pData->getData ("int").value.intVal)
	{
		failure ("did not get correct value");
	}

	pData = myConfig.getSection ("otherVal");

	if (!pData->getData ("bool").value.boolVal)
	{
		failure ("did not get correct value");
	}

	pData = myConfig.getSection ("empty");

	if (NULL == pData)
	{
		failure ("returned null on empty json object");
	}

	if (myConfig.sectionExists ("thing"))
	{
		failure ("returned true on nonexistent section");
	}

	myData.type = jsonParser::STR_TYPE;
	myData.value.strVal = "text";
	myConfig.write ("myVal", "string", myData);

	myConfig.save ();

	pData = myConfig.getSection ("myVal");

	if (-22 != pData->getData ("int").value.intVal)
	{
		failure ("did not get correct value");
	}

	pData = myConfig.getSection ("otherVal");

	if (!pData->getData ("bool").value.boolVal)
	{
		failure ("did not get correct value");
	}

	pData = myConfig.getSection ("myVal");

	if (0 != pData->getData ("string").value.strVal.compare ("text"))
	{
		failure ("did not return correct value");
	}

	myData.type = jsonParser::INT_TYPE;
	myData.value.intVal = myConfig.getSection ("myVal")->getData("int").value.intVal - 1;
	myConfig.write ("myVal", "int", myData);

	if (-23 != pData->getData ("int").value.intVal)
	{
		failure ("did not get correct value");
	}


	myConfig.save ();


	return 0;
}



