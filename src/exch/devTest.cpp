/*******************************************************************************
File:				devTest.cpp
Author: 		Josh Siva
Date:				4/16/14
Project:		NachoNet
Purpose:		Tests operations on devices that involve CouchDB.
*******************************************************************************/

#include "../../include/exch/dataExOnTheCouch.h"

std::string getDevID ()
{
	std::string devID;

	for (int i = 0; i < 12; i++)
	{
		devID.push_back ((char)(rand % 6 + 65));
	}

	return devID;
}

int main ( )
{
	dataEx * pDataEx;
	location loc;
	device dev;

	srand (time (0));

	//add a device
	//wait to check other DBs for change

	return 0;
}

