/*******************************************************************************
File:				devTest.cpp
Author: 		Josh Siva
Date:				4/16/14
Project:		NachoNet
Purpose:		Tests operations on devices that involve CouchDB.
*******************************************************************************/

#include "../../include/exch/dataExOnTheCouch.h"

std::string getDevID ()
{
	std::string devID;

	for (int i = 0; i < 12; i++)
	{
		devID.push_back ((char)(rand () % 6 + 65));
	}

	return devID;
}

int main ( )
{
	dataEx * pDataEx;
	location loc;
	device dev;
	std::string id;
	std::vector<location> devLocations;
	char ch;

	srand (time (0));

	//add a device
	//wait to check other DBs for change

	pDataEx = new dataExOnTheCouch;

	id = getDevID ();
	dev.setID (id);

	std::cout << "add device: ";
	std::cin >> ch;

	pDataEx->addDevice (dev);
	pDataEx->pushUpdates (dataExOnTheCouch::DEVICES);

	std::cout << "I have " << pDataEx->getNumNodes () << " node(s) and "
						<< pDataEx->getNumDevs () << " device(s)\n";

	std::cout << "get other devices: ";
	std::cin >> ch;

	pDataEx->pullUpdates (dataExOnTheCouch::DEVICES);

	std::cout << "I have " << pDataEx->getNumNodes () << " node(s) and "
						<< pDataEx->getNumDevs () << " device(s)\n";

	std::cout << "update location: ";
	std::cin >> ch;

	loc.theID.strID = id;
	loc.x = pDataEx->getID () + 1.5;
	loc.y = pDataEx->getID () + 1.5;

	pDataEx->updateDevLocation (id, loc);
	pDataEx->pushUpdates (dataExOnTheCouch::DEVICES);

	std::cout << "get all locations: ";
	std::cin >> ch;

	pDataEx->pullUpdates (dataExOnTheCouch::DEVICES);

	devLocations = pDataEx->getDeviceLocations ();

	std::cout << "I have devices located at: \n";

	for (auto & thisLoc : devLocations)
	{
		std::cout << thisLoc.theID.strID << " : (" << thisLoc.x << ", "
							<< thisLoc.y << ")\n";
	}

	std::cout << "clear devices: ";
	std::cin >> ch;

	pDataEx->clearDevices ();

	std::cout << "I have " << pDataEx->getNumNodes () << " node(s) and "
						<< pDataEx->getNumDevs () << " device(s)\n";

	delete pDataEx;

	return 0;
}

