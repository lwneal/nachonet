/*******************************************************************************
File:				adminTools.cpp
Author: 		Josh Siva
Date:				10/21/13
Project:		NachoNet
Purpose:		Provides the structure for testing modules, managing nodes, and
						managing NachoNet
*******************************************************************************/

#include "../include/adminTools.h"
#include <string.h>
#include <iostream>
#include <stdlib.h>

//For prototype only (if used in later code it wouldn't be included here)
#include <curl/curl.h>
#include <fstream>
#include <sstream>

const int BACK = -1;

//from http://www.cplusplus.com/forum/unices/45878/
static size_t data_write(void* buf, size_t size, size_t nmemb, void* userp)
{
	if(userp)
	{
		std::ostream& os = *static_cast<std::ostream*>(userp);
		std::streamsize len = size * nmemb;
		if(os.write(static_cast<char*>(buf), len))
			return len;
	}

	return 0;
}

CURLcode curl_read(const std::string& url, std::ostream& os, long timeout = 30)
{
	CURLcode code(CURLE_FAILED_INIT);
	CURL* curl = curl_easy_init();

	if(curl)
	{
		if(CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_write))
		&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L))
		&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L))
		&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FILE, &os))
		&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout))
		&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str())))
		{
			code = curl_easy_perform(curl);
		}
		curl_easy_cleanup(curl);
	}
	return code;
}


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

int main()
{
	int menuID = AdminTools::MAIN;
	int menuSelection;
	curl_global_init(CURL_GLOBAL_ALL); //same location as other curl code
	std::string json;
	std::ostringstream oss;

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
						//FOR PROTOTYPE DEMO ONLY!!!
						//replicate database and print

						//from same place as other curl code
						if(CURLE_OK == curl_read("localhost:5984/NachoNet/node_2", oss))
						{
							// Web page successfully written to string
							json = oss.str();
						}

						std::cout << json << std::endl;

						break;

					case AdminTools::STOP:
						break;

					case AdminTools::LIST_DEVICES:
						//FOR PROTOTYPE DEMO ONLY!!!
						//pull data from database and print
						if(CURLE_OK == curl_read("localhost:5984/NachoNet/locations", oss))
						{
							// Web page successfully written to string
							json = oss.str();
						}

						std::cout << json << std::endl;

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
						//adminTools.testDataCollect()
						break;

					case AdminTools::DIST_MEASUREMENT:
						//adminTools.testDistMeasurement()
						break;

					case AdminTools::DATA_EXCHANGE:
						//adminTools.testDataEx()
						break;

					case AdminTools::LOCALIZATION:
						//adminTools.testLocalization()
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
						//adminTools.calibrate();
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
