/*******************************************************************************
File:				DataExAdmin.cpp
Author: 		Josh Siva
Date:				3/19/14
Project:		NachoNet
Purpose:		Implements the behavior of the admin tool that manages the Data
						Exchange object
*******************************************************************************/

#include "../../include/tools/DataExAdmin.h"


DataExAdmin::DataExAdmin (NachoNet *pNacho) : Admin (pNacho)
{
}

/*******************************************************************************
 * Method:			configure
 *
 * Description: Set the location for this node,
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void DataExAdmin::configure ()
{
	location loc;
	node thisNode;

	if (NULL != pNacho->pDataEx && pNacho->pDataEx->alive ())
	{
		std::cout << "You can't do that while NachoNet is running!\n";
	}
	else if (NULL == pNacho->pDataEx)
	{
		std::cout << "This node has not yet been added to NachoNet\n";
	}
	else
	{
		loc.theID.intID = pNacho->pDataEx->getID ();
		thisNode = pNacho->pDataEx->dropNode (pNacho->pDataEx->getID ());

		do
		{
			std::cout << "Please enter the location of this node x y:";
		 std::cin >> loc.x >> loc.y;
		} while (0 > loc.x || 0 > loc.y);

		thisNode.setLocation (loc);

		pNacho->pDataEx->addNode (thisNode);
	}

}

/*******************************************************************************
 * Method:			test
 *
 * Description: Ping all nodes in NachoNet with a HELLO, wait some amount of
 * 							time, and then check for responses.
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void DataExAdmin::test ()
{
	std::clock_t startTimer;
	double duration;
	Message message;
	message.msg = dataEx::HELLO;

	if (NULL != pNacho->pDataEx && pNacho->pDataEx->alive ())
	{
		std::cout << "You can't do that while NachoNet is running!\n";
	}
	else if (NULL == pNacho->pDataEx)
	{
		std::cout << "This node has not yet been added to NachoNet\n";
	}
	else
	{
		for (int id : pNacho->pDataEx->getNodeIDs ())
		{
			message.dest.push_back (id);
		}

		pNacho->pDataEx->ping (message);

		std::cout << "Pinging all nodes. Please wait a moment.\n";

		startTimer = std::clock ();

		do
		{
			duration = ( std::clock() - startTimer ) / (double) CLOCKS_PER_SEC;
		} while (duration < TEST_DURATION);

		for (int id : pNacho->pDataEx->getNodeIDs ())
		{
			std::cout << "Node " << id << ": ";

			if (pNacho->pDataEx->lastPingResult (id))
			{
				std::cout << "says hello\n";
			}
			else
			{
				std::cout << "NO RESPONSE\n";
			}
		}
	}



}


