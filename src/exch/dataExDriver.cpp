/*******************************************************************************
File:				dataExDriver.cpp
Author: 		Josh Siva
Date:				3/12/14
Project:		NachoNet
Purpose:		This driver is used for testing the data exchange module and its
						pieces.
*******************************************************************************/

#include "../../include/exch/dataExOnTheCouch.h"

bool gNoIssues = true;

void fail (std::string errorMsg)
{
	std::cout << "FAILED: " << errorMsg << "\n";
	gNoIssues = false;
}

int main ()
{
	const int NODE_ID = 10;
	const std::string DEV_ID = "AABBCCDDEEFF";
	const std::string DEV_ID_2 = "aabbccddeeff";

	location loc, locChk;
	distMeasurement dist;
	refMeasurement distChk;
	std::vector <refMeasurement> measurements;
	loc.x = 0.0;
	loc.y = 0.0;
	loc.theID.intID = node::NO_ID;
	node myNode (node::NO_ID, loc);
	device myDev (DEV_ID);
	multicast nachoCast;
	dataEx * pDataEx = NULL;


	//***************************************************************************
	//														NODE TESTING
	//***************************************************************************

	if (node::NO_ID != myNode.getID())
	{
		fail ("node constructor failed to initialize id or getID () failed");
	}

	locChk = myNode.getLocation ();
	if (0.0 != locChk.x || 0.0 != locChk.y)
	{
		fail ("node constructor failed to initialize location or getLocation () failed");
	}

	myNode.setID (NODE_ID);
	if (NODE_ID != myNode.getID ())
	{
		fail ("node setID failed to set the id of the node");
	}

	loc.x = 1.0;
	loc.y = 1.0;
	myNode.setLocation (loc);
	locChk = myNode.getLocation ();
	if (1.0 != locChk.x || 1.0 != locChk.y)
	{
		fail ("node setLocation () or getLocation () failed");
	}

	dist.devID = DEV_ID;
	dist.dist = 1.0;
	myNode.setMeasurement (dist);
	distChk = myNode.getMeasurement (DEV_ID);
	if (1.0 != distChk.devDist.dist || 1.0 != distChk.nodeLocation.x
			|| 1.0 != distChk.nodeLocation.y)
	{
		fail ("node setMeasurement () or getMeasurement () failed");
	}

	dist.devID = DEV_ID_2;
	dist.dist = 1.0;
	myNode.setMeasurement (dist);
	measurements = myNode.getAllMeasurements ();

	if (2 != measurements.size ())
	{
		fail ("node getMeasurements () failed to return the correct number of items");
	}

	myNode.clearMeasurements ();
	measurements = myNode.getAllMeasurements ();

	if (0 != measurements.size ())
	{
		fail ("node failed to clear measurements");
	}

	//***************************************************************************
	//														DEVICE TESTING
	//***************************************************************************

	if (DEV_ID != myDev.getID())
	{
		fail ("device constructor failed to set id or getID failed");
	}

	myDev.setID (DEV_ID_2);
	if (DEV_ID_2 != myDev.getID())
	{
		fail ("device setID () or getID () failed");
	}

	myDev.setLocation(loc);
	locChk = myDev.getLocation();
	if (1.0 != locChk.x || 1.0 != locChk.y)
	{
		fail ("device setLocation () or getLocation () failed");
	}

	//***************************************************************************
	//														MULTICAST TESTING
	//***************************************************************************

	//tested elsewhere


	//***************************************************************************
	//										DATA EXCHANGE TESTING SINGLE NODE
	//***************************************************************************
	//be sure to check CouchDB for effects as well
	pDataEx = new dataExOnTheCouch ();

	//we shouldn't find anyone, so we should be node 0
	if (0 != pDataEx->getID ())
	{
		fail ("data ex id not set correctly on instantiation");
	}

	//there are three nodes already hanging out in the db that we won't grab
	if (1 != pDataEx->getNumNodes ())
	{
		fail ("data ex did not get the correct number of nodes on instantiation");
	}

	//though there are devs in the database, we should not get them because we
	//haven't seen them yet
	if (0 != pDataEx->getNumDevs ())
	{
		fail ("data ex did not get the correct number of devs on instantiation");
	}

	if (pDataEx->alive ())
	{
		fail ("data ex did not set alive state correctly on instantiation");
	}

	loc.theID.intID = 5;
	loc.x = 5;
	loc.y = 5;
	node newNode (5, loc);

	pDataEx->addNode (newNode);

	if (2 != pDataEx->getNumNodes ())
	{
		fail ("data ex did not add node");
	}

	pDataEx->dropNode (5);

	if (1 != pDataEx->getNumNodes ())
	{
		fail ("data ex did not drop node");
	}


	delete pDataEx;

	if (gNoIssues)
	{
		std::cout << "\nTesting completed successfully. Nachos for everyone!\n";
	}

	return 0;
}
