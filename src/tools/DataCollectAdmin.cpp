/*******************************************************************************
File:				DataCollectAdmin.cpp
Author: 		Josh Siva
Date:				3/18/14
Project:		NachoNet
Purpose:		Implements the functionality required to test the Data Collection
						object
*******************************************************************************/

#include "../../include/tools/DataCollectAdmin.h"

DataCollectAdmin::DataCollectAdmin (NachoNet *pNacho) : Admin (pNacho)
{
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

	if (NULL != pNacho->pDataEx && pNacho->pDataEx->alive ())
	{
		std::cout << "You can't do that while NachoNet is running!\n";
	}
	else
	{
		Admin::test ();

		do
		{
			pNacho->pDataCollect->readFromNetwork();
		} while (!pNacho->pDataCollect->isReadyToRead());


		values = pNacho->pDataCollect->getSS ();

		for (auto item : values)
		{
			*pOut << item.devID << "  " << item.ss << "\n";
		}
	}
}


