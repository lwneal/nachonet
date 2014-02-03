/*******************************************************************************
File:				adminDataCollect
Author: 		Josh Siva
Date:				2/2/14
Project:		NachoNet
Purpose:		Implements the behavior of the admin wrapper to the data collection
						module
*******************************************************************************/

#include "../../include/tools/adminDataCollect.h"
#include <iostream>

adminDataCollect::adminDataCollect ()
{
	debug = false;
	pDataCollect = NULL;
}

adminDataCollect::~adminDataCollect ()
{
	if (NULL != pDataCollect)
	{
		delete pDataCollect;
	}
}

void adminDataCollect::setDebug(bool debug)
{
	this->debug = debug;
}

void adminDataCollect::setCollectMode(int collectMode)
{
	if (NULL == pDataCollect)
	{
		switch (collectMode)
		{
			case STD:
				pDataCollect = new stdCollect (DEFAULT_INTERFACE, debug);
				break;

			case EXP:
			default:
				break;
		}
	}
}

void adminDataCollect::test(std::ostream & os)
{
	std::vector<ssMeasurement> values;

	do
	{
		pDataCollect->readFromNetwork();
	} while (pDataCollect->isReadyToRead());


	values = pDataCollect->getSS ();

	for (auto item : values)
	{
		os << item.devID << "  " << item.ss << "\n";
	}
}


