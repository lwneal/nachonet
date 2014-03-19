/*******************************************************************************
File:				DataCollectAdmin.cpp
Author: 		Josh Siva
Date:				3/18/14
Project:		NachoNet
Purpose:		Implements the functionality required to test the Data Collection
						object
*******************************************************************************/

#include "../../include/tools/DataCollectAdmin.h"

/*******************************************************************************
 * Constructor:	DataCollectAdmin
 *
 * Description:	Initializes the local copy of the Data Collection object
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
DataCollectAdmin::DataCollectAdmin (dataCollect *pDataCollect)
{
	this->pDataCollect = pDataCollect;
}

/*******************************************************************************
 * Method:			test
 *
 * Description:	Calls the parent method and then collects a bunch of signal
 * 							strengths off of devices found in the network.
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void DataCollectAdmin::test ()
{
	std::vector<ssMeasurement> values;

	Admin::test ();

	do
	{
		pDataCollect->readFromNetwork();
	} while (!pDataCollect->isReadyToRead());


	values = pDataCollect->getSS ();

	for (auto item : values)
	{
		os << item.devID << "  " << item.ss << "\n";
	}
}


