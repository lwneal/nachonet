/*******************************************************************************
File:				DataFeeder.h
Author: 		Josh T Siva
Date:				4/1/14
Project:		NachoNet
Purpose:		Defines the interface to the object that uses the dataCollect API to
						mimic the behavior of data collection while actually drawing data
						from a file. This is to be used for testing/debugging of NachoNet
						without the unpredictable nature of monitoring the network.
*******************************************************************************/

#pragma once
#include "dataCollect.h"
#include <fstream>
#include <vector>

class DataFeeder : public dataCollect
{
	public:
		DataFeeder (std::string fileName, bool debug = false);
		~DataFeeder ();
		virtual void readFromNetwork ();

	private:
		int numDevices;
		std::ifstream dataFile;
		std::vector <ssMeasurement> data;
};
