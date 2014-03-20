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
#include "../dist/fsPathLoss.h"
#include "../dist/logNormalShadow.h"
#include "../dist/pathLoss.h"
#include "../exch/dataExOnTheCouch.h"
#include "../loc/loc.h"
#include "../tools/Admin.h"

class NachoNet
{
	friend class Admin;
	public:
		NachoNet (bool debug, bool verbose);
		~NachoNet ();
		void start ();
		void stop ();
		void worker ();
		void listener ();
		void listDevices ();
		void addNode ();
		void dropNode ();

		bool isDebug ();
		bool isVerbose ();

		static const std::string DIST_CONFIG_FILE;

	private:
		dataCollect * pDataCollect;
		distMeasure * pDistMeasure;
		localization * pLocalization;
		dataEx *pDataEx;
		std::thread * pWorker;
		std::thread * pListener;

		bool debug;
		bool verbose;
		bool active;

};
