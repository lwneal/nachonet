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
	int interval = 0, networkReads = 0;

	if (NULL != pNacho->pDataEx && pNacho->pDataEx->alive ())
	{
		std::cout << "You can't do that while NachoNet is running!\n";
	}
	else
	{
		Admin::test ();

		do
		{
			interval++;
			networkReads++;

			pNacho->pDataCollect->readFromNetwork();

			if (NachoNet::GARBAGE_COLLECTION_INTERVAL < interval)
			{
				pNacho->pDataCollect->garbageCollect ();
				interval = 0;
			}
		} while (!pNacho->pDataCollect->isReadyToRead()
						 || NachoNet::MIN_NETWORK_READS > networkReads);


		values = pNacho->pDataCollect->getSS ();

		for (auto item : values)
		{
			*pOut << item.devID << "  " << item.ss << "\n";
		}
	}
}


