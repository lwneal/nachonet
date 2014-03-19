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
		NachoNet ();
		~NachoNet ();
		void menu ();
		int menuSelect ();
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

		typedef enum Menus { MAIN = 0,
											 TEST,
											 MANAGE
										 } Menus;

		typedef enum MainMenu { EXIT = -1,
														TEST_MENU,
														MANAGE_MENU,
														START,
														STOP,
														LIST_DEVICES
													} MainMenu;

		typedef enum TestMenu { DATA_COLLECTION = 0,
														DIST_MEASUREMENT,
														DATA_EXCHANGE,
														LOCALIZATION
													} TestMenu;

		typedef enum ManageMenu { ADD_NODE = 0,
															DROP_NODE,
															CALIBRATE_DIST,
															SET_NODE_LOC
													} ManageMenu;

	private:
		dataCollect *pDataCollect;
		distMeasure *pDistMeasure;
		localization *pLocalizitation;
		dataEx *pDataEx;

		bool debug;
		bool verbose;

};
