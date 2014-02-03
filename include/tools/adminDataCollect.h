/*******************************************************************************
File:				adminDataCollect.h
Author: 		Josh Siva
Date:				2/2/14
Project:		NachoNet
Purpose:		The defines the interface to the admin wrapper around the data
						collection module
*******************************************************************************/

#pragma once
#include "../collect/stdCollect.h"

class adminDataCollect
{
	public:
		adminDataCollect ();
		~adminDataCollect ();
		void test(std::ostream & os);
		void setDebug (bool debug);
		void setCollectMode (int collectMode);

		const std::string DEFAULT_INTERFACE = "wlan0";
		const int TEST_PACKETS = 1024;

		typedef enum COLLECT_MODE
		{
			STD = 1,
			EXP
		} COLLECT_MODE;

	private:
		bool debug;
		dataCollect * pDataCollect;

};
