/*******************************************************************************
File:				adminToolsDriver.cpp
Author: 		Josh Siva
Date:				10/21/13
Project:		NachoNet
Purpose:		Provides the menu structure for testing modules, managing nodes, and
						managing NachoNet
*******************************************************************************/

#include "../../include/tools//adminTools.h"
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>


const int BACK = -1;

int menu(int menuID)
{
	int selection;

	switch(menuID)
	{
		case AdminTools::MAIN:

			std::cout << "Main Menu\n";
			std::cout << "---------\n\n";

			do
			{
				std::cout << "-1. Exit\n";
				std::cout << " 0. Test\n";
				std::cout << " 1. Node Management\n";
				std::cout << " 2. Start\n";
				std::cout << " 3. Stop\n";
				std::cout << " 4. List Devices\n\n";

				std::cout << "Enter Selection: ";
				std::cin >> selection;
			} while(AdminTools::EXIT > selection
							|| AdminTools::LIST_DEVICES < selection);

			break;

		case AdminTools::TEST:

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
			}while(BACK > selection
						 || AdminTools::LOCALIZATION < selection);

			break;

		case AdminTools::MANAGE:

			std::cout << "Manage Menu\n";
			std::cout << "-----------\n\n";

			do
			{
				std::cout << "-1. Back\n";
				std::cout << " 0. Add Node\n";
				std::cout << " 1. Drop Node\n";
				std::cout << " 2. Calibrate\n\n";

				std::cout << "Enter Selection: ";
				std::cin >> selection;
			}while(BACK > selection
						 || AdminTools::CALIBRATE < selection);

			break;
	}


	return selection;
}


int main(int argc, char** argv)
{
	int menuID = AdminTools::MAIN;
	int menuSelection;
	int option_char, eqnType, collectMode;
	AdminTools adminTools;

	while ((option_char = getopt (argc, argv, "d:c:")) != -1)
	{
		switch (option_char)
		{
			case 'c':
				collectMode = std::stoi(optarg);
				break;
			case 'd':
			 eqnType = std::stoi(optarg);
			 break;
			case '?':
			 std::cout << "\nHELP\n";
			 std::cout << "Due to the libpcap integration, this must be run with "
								 <<	"sudo\n";
			 std::cout << "Options\n";
			 std::cout << "d - specify the equation type\n";
			 std::cout << "    1 - path loss\n";
			 std::cout << "    2 - free space path loss\n";
			 std::cout << "    3 - log normal shadow model\n";
			 std::cout << "c - specify the collection mode\n";
			 std::cout << "    1 - standard\n";
			 std::cout << "    2 - experimental [not functional]\n";
			 break;
		 }
	}

	adminTools.setDistEqn(eqnType);
	adminTools.setCollectMode(collectMode);

	while (AdminTools::EXIT != menuSelection || AdminTools::MAIN != menuID)
	{
		switch(menuID)
		{
			case AdminTools::MAIN:

				switch(menu(menuID))
				{
					case AdminTools::TEST_MENU:
						menuID = AdminTools::TEST;
						break;

					case AdminTools::MANAGE_MENU:
						menuID = AdminTools::MANAGE;
						break;

					case AdminTools::START:

						break;

					case AdminTools::STOP:
						break;

					case AdminTools::LIST_DEVICES:

						break;

					case AdminTools::EXIT:
						menuSelection = AdminTools::EXIT;
						break;
				}

				break;

			case AdminTools::TEST:

				switch(menu(menuID))
				{
					case AdminTools::DATA_COLLECTION:
						adminTools.testCollect();
						break;

					case AdminTools::DIST_MEASUREMENT:
						adminTools.testDist();
						break;

					case AdminTools::DATA_EXCHANGE:
						//adminTools.testDataEx()
						break;

					case AdminTools::LOCALIZATION:
						adminTools.testLocalize();
						break;

					case BACK:
						menuID = AdminTools::MAIN;
						break;
				}

				break;

			case AdminTools::MANAGE:

				switch(menu(menuID))
				{
					case AdminTools::ADD_NODE:
						//adminTools.addNode();
						break;

					case AdminTools::DROP_NODE:
						//adminTools.dropNode();
						break;

					case AdminTools::CALIBRATE:
						adminTools.configDist();
						break;

					case BACK:
						menuID = AdminTools::MAIN;
						break;

				}
				break;
		}

		std::cout << "==========================================\n";
		std::cout << "\n\n\n\n\n\n";

	}


	return 0;
}


