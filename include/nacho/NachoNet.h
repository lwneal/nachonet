/*******************************************************************************
File:				NachoNet.h
Author: 		Josh Siva
Date:				3/12/14
Project:		NachoNet
Purpose:		This is the main object that brings all of the pieces of NachoNet
						together.
*******************************************************************************/

#pragma once
#include "../collect/stdCollect.h"
#include "../collect/DataFeeder.h"
#include "../dist/fsPathLoss.h"
#include "../dist/logNormalShadow.h"
#include "../dist/pathLoss.h"
#include "../exch/dataExOnTheCouch.h"
#include "../loc/loc.h"
#include "../tools/Admin.h"
#include "../exch/dataEx.h"
#include "../exch/dataExOnTheCouch.h"
#include "../map/MapOnTheCouch.h"


class NachoNet
{
	friend class Admin;
	friend class DataCollectAdmin;
	friend class DistMeasureAdmin;
	friend class DataExAdmin;
	friend class LocalizationAdmin;
	public:
		NachoNet (bool debug, bool verbose, bool feeder,
							std::string feederFile = "");
		~NachoNet ();
		void start ();
		void stop ();
		void worker ();
		void listener ();
		void listDevices ();
		void addNode ();
		void dropNode ();
		void updateMap ();

		bool isDebug ();
		bool isVerbose ();

		static const std::string DIST_CONFIG_FILE;
		static const unsigned int REQUIRED_MEASUREMENTS = 3;
		static const int GARBAGE_COLLECTION_INTERVAL = 10;
		static const int MIN_NETWORK_READS = 5;
		static const int SS_THRESHOLD = -68;

	private:
		dataCollect * pDataCollect;
		distMeasure * pDistMeasure;
		localization * pLocalization;
		dataEx *pDataEx;
		Map *pMap;
		std::thread * pWorker;
		std::thread * pListener;

		bool feeder;
		bool debug;
		bool verbose;
		bool active;

};
