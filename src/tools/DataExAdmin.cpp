/*******************************************************************************
File:				DataExAdmin.cpp
Author: 		Josh Siva
Date:				3/19/14
Project:		NachoNet
Purpose:		Implements the behavior of the admin tool that manages the Data
						Exchange object
*******************************************************************************/

#include "../../include/tools/DataExAdmin.h"


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

	if (pNacho->pDataEx->alive ())
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
		thisNode = pNacho->pDataEx->nodes[pNacho->pDataEx->getID ()];

		std::cout << "Please enter the location of this node ('x y'):";
		std::cin >> loc.x >> loc.y;

		thisNode.setLocation (loc);

		pNacho->pDataEx->nodes[pNacho->pDataEx->getID ()] = thisNode;
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

	if (pNacho->pDataEx->alive ())
	{
		std::cout << "You can't do that while NachoNet is running!\n";
	}
	else if (NULL == pNacho->pDataEx)
	{
		std::cout << "This node has not yet been added to NachoNet\n";
	}
	else
	{
		for (auto & thisNode : pNacho->pDataEx->nodes)
		{
			message.dest.push_back (thisNode.first);
		}

		pNacho->pDataEx->ping (message);
		std::cout << "Pinging all nodes. Please wait a moment.\n";

		startTimer = std::clock ();

		do
		{
			duration = ( std::clock() - startTimer ) / (double) CLOCKS_PER_SEC;
		} while (duration < TEST_DURATION);

		for (auto & thisNode : pNacho->pDataEx->nodes)
		{
			std::cout << "Node " << thisNode.first << ": ";

			if (pNacho->pDataEx->lastPingResult (thisNode.first))
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


