/*******************************************************************************
File:				adminTools.h
Author: 		Josh Siva
Date:				10/21/13
Project:		NachoNet
Purpose:		Defines all of the useful constants and structures used by the
						adminTools module.
*******************************************************************************/

#pragma once

struct DeviceEntry
{
	int id;
	int xCoordinate;
	int yCoordinate;
}DeviceEntry;

class AdminTools
{
public:
	~AdminTools();

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
														CALIBRATE
												} ManageMenu;

	int startNachoNet();
	int stopNachoNet();
	void listDevices();

	int testDataCollect();
	int testDist();
	int testDataEx();
	int testLocalize();

private:
	//DataCollectionTest dataCollectTest;
	//DistMeasurementTest	distTest;
	//DataExchangeTest dataExTest;
	//LocalizationTest localizeTest;

};
