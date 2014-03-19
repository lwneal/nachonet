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

class NachoNet
{
	public:
		NachoNet (bool debug, bool verbose);
		~NachoNet ();
		void start ();
		void stop ();
		void listDevices ();
		void addNode ();
		void dropNode ();

		bool isAlive ();
		bool isDebug ();
		bool isVerbose ();

		void setDebug (bool debug);
		void setVerbose (bool verbose);


	private:
		dataCollect *pDataCollect;
		distMeasure *pDistMeasure;
		localization *pLocalizitation;
		dataEx *pDataEx;

		bool debug;
		bool verbose;

};
