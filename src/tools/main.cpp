/*******************************************************************************
File:
Author: 		Josh Siva
Date:
Class:			CS460
Assignment:
Purpose:
*******************************************************************************/

#include "../../include/nacho/NachoNet.h"
#include "../../include/tools/DataCollectAdmin.h"
#include "../../include/tools/DistMeasureAdmin.h"
#include "../../include/tools/DataExAdmin.h"
#include "../../include/tools/LocalizationAdmin.h"
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

const int BACK = -1;

typedef enum Menus { MAIN = 0,
									 TEST,
									 MANAGE
								 } Menus;

typedef enum MainMenu { EXIT = -1,
												TEST_MENU,
												MANAGE_MENU,
												START,
												STOP,
												LIST_DEVICES,
												UPDATE_MAP
											} MainMenu;

typedef enum TestMenu { DATA_COLLECTION = 0,
												DIST_MEASUREMENT,
												DATA_EXCHANGE,
												LOCALIZATION
											} TestMenu;

typedef enum ManageMenu { ADD_NODE = 0,
													DROP_NODE,
													SET_DIST_EQN,
													CONFIGURE_DIST_EQN,
													SET_NODE_LOC
											} ManageMenu;

/*******************************************************************************
 * Function:
 *
 * Description:
 *
 * Parameters:
 *
 * Returned:
 ******************************************************************************/
void clearScreen ()
{
	std::cout << std::string (100, '\n');
}

/*******************************************************************************
 * Function:
 *
 * Description:
 *
 * Parameters:
 *
 * Returned:
 ******************************************************************************/
int menuSelection (int menuID)
{
	int selection;

	switch(menuID)
	{
		case MAIN:

			std::cout << "Main Menu\n";
			std::cout << "---------\n\n";

			do
			{
				std::cout << "-1. Exit\n";
				std::cout << " 0. Test\n";
				std::cout << " 1. Node Management\n";
				std::cout << " 2. Start\n";
				std::cout << " 3. Stop\n";
				std::cout << " 4. List Devices\n";
				std::cout << " 5. Update Map\n\n";

				std::cout << "Enter Selection: ";
				std::cin >> selection;
			} while (EXIT > selection || UPDATE_MAP < selection);

			break;

		case TEST:

			std::cout << "Test Menu\n";
			std::cout << "---------\n\n";

			do
			{
				std::cout << "-1. Back\n";
				std::cout << " 0. Data Collection\n";
				std::cout << " 1. Distance Measurement\n";
				std::cout << " 2. Data Exchange\n";
				std::cout << " 3. Localization\n\n";

				std::cout << "Enter Selection: ";
				std::cin >> selection;
			}while (BACK > selection || LOCALIZATION < selection);

			break;

		case MANAGE:

			std::cout << "Manage Menu\n";
			std::cout << "-----------\n\n";

			do
			{
				std::cout << "-1. Back\n";
				std::cout << " 0. Add Node\n";
				std::cout << " 1. Drop Node\n";
				std::cout << " 2. Set Distance Equation\n";
				std::cout << " 3. Configure Distance Measure\n";
				std::cout << " 4. Set Node Location\n\n";

				std::cout << "Enter Selection: ";
				std::cin >> selection;
			}while (BACK > selection || SET_NODE_LOC < selection);

			break;
	}


	return selection;
}

/*******************************************************************************
 * Function:
 *
 * Description:
 *
 * Parameters:
 *
 * Returned:
 ******************************************************************************/
int main (int argc, char** argv)
{
	int menuID = MAIN;
	NachoNet *pMyNachos;
	Admin *pMyAdmin;
	int optionChar;
	std::string fileName = "";
	bool debug = false, verbose = false, feeder = false, done = false;

	while ((optionChar = getopt (argc, argv, "dvf:")) != -1)
	{
		switch (optionChar)
		{
			case 'd':
				debug = true;
				break;
			case 'v':
			 verbose = true;
			 break;
			case 'f':
				feeder = true;
				fileName = optarg;
				break;
			case '?':
			 std::cout << "\nHELP\n";
			 std::cout << "Due to the libpcap integration, this must be run with "
								 <<	"sudo\n";
			 std::cout << "Options\n";
			 std::cout << "d - turn debug mode on\n"
					       << "v - turn verbose mode on\n"
					       << "f - use a DataFeeder for data collection\n";

			 done = true;
			 break;
		 }
	}

	pMyNachos = new NachoNet (debug, verbose, feeder, fileName);

	while (!done || MAIN != menuID)
	{
		switch(menuID)
		{
			case MAIN:

				switch (menuSelection (menuID))
				{
					case TEST_MENU:
						menuID = TEST;
						break;

					case MANAGE_MENU:
						menuID = MANAGE;
						break;

					case START:
						pMyNachos->start ();
						break;

					case STOP:
						pMyNachos->stop ();
						break;

					case LIST_DEVICES:
						pMyNachos->listDevices ();
						break;

					case UPDATE_MAP:
						pMyNachos->updateMap ();
						break;

					case EXIT:
						done = true;
						break;
				}

				break;

			case TEST:

				switch (menuSelection (menuID))
				{
					case DATA_COLLECTION:
						pMyAdmin = new DataCollectAdmin (pMyNachos);
						break;

					case DIST_MEASUREMENT:
						pMyAdmin = new DistMeasureAdmin (pMyNachos);
						break;

					case DATA_EXCHANGE:
						pMyAdmin = new DataExAdmin (pMyNachos);
						break;

					case LOCALIZATION:
						pMyAdmin = new LocalizationAdmin (pMyNachos);
						break;

					case BACK:
						menuID = MAIN;
						break;
				}

				if (MAIN != menuID)
				{
					pMyAdmin->test ();
					delete pMyAdmin;
					pMyAdmin = NULL;
				}

				break;

			case MANAGE:

				switch (menuSelection (menuID))
				{
					case ADD_NODE:
						pMyNachos->addNode ();
						break;

					case DROP_NODE:
						pMyNachos->dropNode ();
						break;

					case SET_DIST_EQN:
						pMyAdmin = new DistMeasureAdmin (pMyNachos);
						pMyAdmin->options ();
						break;

					case CONFIGURE_DIST_EQN:
						pMyAdmin = new DistMeasureAdmin (pMyNachos);
						pMyAdmin->configure ();
						break;

					case SET_NODE_LOC:
						pMyAdmin = new DataExAdmin (pMyNachos);
						pMyAdmin->configure ();
						break;

					case BACK:
						menuID = MAIN;
						break;

				}

				if (NULL != pMyAdmin)
				{
					delete pMyAdmin;
					pMyAdmin = NULL;
				}
				break;
		}

		//std::cout << "==========================================\n";
		//std::cout << "\n\n\n\n\n\n";
		clearScreen ();

	}

	delete pMyNachos;

	return 0;
}

