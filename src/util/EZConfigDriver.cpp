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
	EZConfig myConfig ("dist.json"), otherConfig ("dist.json");
	EZConfig badConfig ("dist.conf");
	jsonData myData;
	JSON data;

	data = myConfig.getSection ("pathLoss");

	if (-38 != data.getData ("P_d0").value.intVal)
	{
		failure ("did not get correct value");
	}

	data = myConfig.getSection ("fsPathLoss");

	if (1 != data.getData ("channel").value.intVal)
	{
		failure ("did not get correct value");
	}


	myConfig.getSection ("pathLoss").writeJSON ("");
	myConfig.getSection ("thing").writeJSON ("");

	if (!myConfig.sectionExists ("thing"))
	{

	}


	myData.type = jsonParser::INT_TYPE;
	myData.value.intVal = 25;
	otherConfig.write ("fsPathLoss", "thing", myData);



	return 0;
}



