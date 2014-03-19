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
 * Constructor:	DataExAdmin
 *
 * Description:	Initializes our copy of the Data Exchange object
 *
 * Parameters:	pDataEx - the object we want to manipulate
 *
 * Returned:		None
 ******************************************************************************/
DataExAdmin::DataExAdmin (dataEx * pDataEx)
{
	this->pDataEx = pDataEx;
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

	if (NULL != pDataEx)
	{
		loc.theID.intID = pDataEx->getID ();
		thisNode = pDataEx->nodes[pDataEx->getID ()];

		std::cout << "Please enter the location of this node ('x y'):";
		std::cin >> loc.x >> loc.y;

		thisNode.setLocation (loc);

		pDataEx->nodes[pDataEx->getID ()] = thisNode;
	}
	else
	{
		std::cout << "This node has not been added to NachoNet\n";
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

	if (NULL != pDataEx)
	{
		for (auto & thisNode : pDataEx->nodes)
		{
			message.dest.push_back (thisNode.first);
		}

		pDataEx->ping(message);
		std::cout << "Pinging all nodes. Please wait a moment.\n";

		startTimer = std::clock ();

		do
		{
			duration = ( std::clock() - startTimer ) / (double) CLOCKS_PER_SEC;
		} while (duration < TEST_DURATION);

		for (auto & thisNode : pDataEx->nodes)
		{
			std::cout << "Node " << thisNode.first << ": ";

			if (pDataEx->lastPingResult(thisNode.first))
			{
				std::cout << "says hello\n";
			}
			else
			{
				std::cout << "NO RESPONSE\n";
			}
		}
	}
	else
	{
		std::cout << "This node has not been added to NachoNet\n";
	}



}


