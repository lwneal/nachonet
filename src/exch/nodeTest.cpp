/*******************************************************************************
File:				nodeTest.cpp
Author: 		Josh Siva
Date:				4/16/14
Project:		NachoNet
Purpose:		Tests the node interactions with CouchDB
*******************************************************************************/

#include "../../include/exch/dataExOnTheCouch.h"

int main ()
{
	dataEx * pDataEx;
	location loc;
	node myNode;
	device dev;
	std::string id;
	std::vector<location> locations;
	std::vector<refMeasurement> refMeasures;
	std::vector<int> nodeIDs;
	distMeasurement dist;
	char ch;

	pDataEx = new dataExOnTheCouch;

	std::cout << "update node loc:";
	std::cin >> ch;

	loc.theID.intID = pDataEx->getID ();

	if (0 == pDataEx->getID ())
	{
		loc.x = 0.0;
		loc.y = 0.0;
	}
	else if (1 == pDataEx->getID ())
	{
		loc.x = 7.5;
		loc.y = 0.0;
	}
	else if (2 == pDataEx->getID ())
	{
		loc.x = 3.25;
		loc.y = 13.2;
	}

	pDataEx->pullUpdates (dataExOnTheCouch::NODES);

	myNode = pDataEx->dropNode (pDataEx->getID ());
	myNode.setLocation (loc);
	pDataEx->addNode (myNode);

	pDataEx->pushUpdates (dataExOnTheCouch::NODES);

	std::cout << "get other node loc:";
	std::cin >> ch;

	pDataEx->pullUpdates (dataExOnTheCouch::NODES);

	std::cout << "update dev measurement:";
	std::cin >> ch;

	dev.setID ("AAAAAAAAAAAA");
	pDataEx->addDevice (dev);
	dist.devID = dev.getID ();
	dist.dist = 2.0 * pDataEx->getID ();

	pDataEx->updateDevMeasurement (dist);
	pDataEx->pushUpdates (dataExOnTheCouch::NODES);
	pDataEx->pushUpdates (dataExOnTheCouch::DEVICES);

	std::cout << "get other dev measurements:";
	std::cin >> ch;

	pDataEx->pullUpdates (dataExOnTheCouch::DEVICES);
	pDataEx->pullUpdates (dataExOnTheCouch::NODES);

	std::cout << "get reference measurement:";
	std::cin >> ch;

	refMeasures = pDataEx->getMeasurements ("AAAAAAAAAAAA");

	std::cout << "I have: \n";

	for (auto & measure : refMeasures)
	{
		std::cout << measure.devDist.devID << " is " << measure.devDist.dist
							<< " from " << measure.nodeLocation.theID.intID
							<< " at (" << measure.nodeLocation.x << ", "
							<< measure.nodeLocation.y << ")\n";
	}

	std::cout << "get random data:";
	std::cin >> ch;

	locations = pDataEx->getNodeLocations ();
	nodeIDs = pDataEx->getNodeIDs ();

	std::cout << "I know: ";
	for (int id : nodeIDs)
	{
		std::cout << id << " ";
	}
	std::cout << "\n\n";

	std::cout << "Nodes located at:\n";
	for (auto & thisLoc : locations)
	{
		std::cout << thisLoc.theID.intID << ": (" << thisLoc.x << ", "
							<< thisLoc.y << ")\n";
	}

	delete pDataEx;

	return 0;
}


