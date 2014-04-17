/*******************************************************************************
File:				helloTest.cpp
Author: 		Josh Siva
Date:				4/16/14
Project:		NachoNet
Purpose:		Tests the hello system of data exchange
*******************************************************************************/

#include "../../include/exch/dataExOnTheCouch.h"

int main ()
{
	dataEx *					pDataEx;
	char							ch;
	Message						message;
	std::vector<int> 	nodeIDs;

	pDataEx = new dataExOnTheCouch;


	std::cout << "Enter s to send, else hit any key: ";
	std::cin >> ch;

	nodeIDs = pDataEx->getNodeIDs ();

	if ('s' == ch)
	{
		message.msg = dataEx::HELLO;

		for (int id : nodeIDs)
		{
			if (pDataEx->getID () != id)
			{
				message.dest.push_back (id);
			}
		}

		pDataEx->ping (message);
	}
	else
	{
		pDataEx->checkMessages ();
	}

	std::cout << "Enter s to check responses, else hit any key:: ";
	std::cin >> ch;

	if ('s' == ch)
	{
		pDataEx->checkMessages ();

		for (int id : nodeIDs)
		{
			if (pDataEx->getID () != id && pDataEx->lastPingResult (id))
			{
				std::cout << "Heard back from " << id << "\n";
			}
		}
	}

	pDataEx->checkMessages ();
	delete pDataEx;

	return 0;
}


