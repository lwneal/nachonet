/*******************************************************************************
File:				dataExDriver.cpp
Author: 		Josh Siva
Date:				3/12/14
Project:		NachoNet
Purpose:		This driver is used for testing the data exchange module and its
						pieces.
*******************************************************************************/

#include "../../include/exch/multicast.h"
#include "../../include/exch/dataEx.h"
//#include "../../include/exch/dataExOnTheCouch.h"

void fail (std::string errorMsg)
{
	std::cout << "FAILED: " << errorMsg << "\n";
}

int main ()
{
	const int NODE_ID = 10;
	const std::string DEV_ID = "AABBCCDDEEFF";
	const std::string DEV_ID_2 = "aabbccddeeff";
	bool noIssues = true;
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
	dataEx * pDataEx;

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

	//do stuff I suppose


	//***************************************************************************
	//														DATA EXCHANGE TESTING
	//***************************************************************************



	if (noIssues)
	{
		std::cout << "\nTesting completed successfully. Nachos for everyone!\n";
	}

	return 0;
}
