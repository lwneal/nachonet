/*******************************************************************************
File:				dataCollectDriver
Author: 		Josh Siva
Date:				1/27/14
Project:		NachoNet
Purpose:		Tests the functionality of the data collection modules
*******************************************************************************/

#include "../../include/collect/stdCollect.h"
#include <iostream>

int main (int argc, char ** argv)
{
	bool debug;
	int counter = 0;

	if (argc > 1)
	{
		debug = true;
	}
	else
	{
		debug = false;
	}

	stdCollect theCollector ("wlan0", debug);
	std::vector<ssMeasurement> values;

	do
	{
		counter++;
		theCollector.readFromNetwork();

		if (10 < counter)
		{
			theCollector.garbageCollect ();
			counter = 0;
		}

	} while (!theCollector.isReadyToRead());


	values = theCollector.getSS ();

	for (auto item : values)
	{
		std::cout << item.devID << " : " << item.ss << "\n";
	}

	return 0;
}
