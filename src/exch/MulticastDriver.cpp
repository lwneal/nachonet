/*******************************************************************************
File:				MulticastDriver.cpp
Author: 		Josh Siva
Date:				3/24/14
Project:		NachoNet
Purpose:		Tests the multicast object by setting up either a sender or
						receiver. Receivers listen for messages then send back a response.
						This is a bit like the ping pong test.
*******************************************************************************/

#include "../../include/exch/multicast.h"
#include <unistd.h>
#include <getopt.h>
#include <iostream>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <thread>

/*******************************************************************************
 * Function:
 *
 * Description:
 *
 * Parameters:
 *
 * Returned:
 ******************************************************************************/
void initBuffer (char * buffer, int size)
{
	for (int i = 0; i < size; i++)
	{
		buffer[i] = '\0';
	}
}

class threadTest
{
	public:
		threadTest (std::string address)
		{
			pMulticast = new multicast (multicast::DEFAULT_PORT, address,
																	multicast::MULTICAST_GROUP);
			running = true;
			pListener = new std::thread (&threadTest::listener, this);
		}

		~threadTest ()
		{
			running = false;

			pMulticast->kill();
			pListener->join ();
			delete pMulticast;
			delete pListener;
		}

		void send (std::string message)
		{
			char sendData[multicast::BUF_LENGTH];

			initBuffer (sendData, multicast::BUF_LENGTH);

			memcpy (sendData, message.c_str (), message.size ());

			pMulticast->transmit (sendData);
		}

		void listener ()
		{
			std::string message;

			while (running)
			{
				message = "";
				message = pMulticast->receive ();

				if (0 != message.compare (""))
				{
					std::cout << message << "\n";
					std::cout << "~>";
				}
			}
		}

		bool running;
		multicast *pMulticast;
		std::thread * pListener;
};


/*******************************************************************************
 * Function:
 *
 * Description:
 *
 * Parameters:
 *
 * Returned:
 ******************************************************************************/
int main (int argc, char** argv)
{
	const char * EXIT = "exit";

	//multicast * pMulticast;
	std::string data, receiveData, myAddress;
	int optionChar;
	bool sender = false/*, stillRunning = true*/;
	struct ifaddrs * pIfaceAddr = NULL;
	struct ifaddrs * pIface = NULL;
	void * pTempAddr = NULL;
	char addrBuffer[INET_ADDRSTRLEN];
	//char sendData[multicast::BUF_LENGTH];

	threadTest *pThread;

	while ((optionChar = getopt (argc, argv, "s")) != -1)
	{
		switch (optionChar)
		{
			case 's':
				sender = true;
				break;
		}
	}

	getifaddrs (&pIfaceAddr);

	for (pIface = pIfaceAddr; pIface != NULL; pIface = pIface->ifa_next)
	{
		//check for IPv4 address
		if (pIface->ifa_addr->sa_family == AF_INET)
		{
			pTempAddr = &((struct sockaddr_in *)pIface->ifa_addr)->sin_addr;
			inet_ntop (AF_INET, pTempAddr, addrBuffer, INET_ADDRSTRLEN);

			myAddress = addrBuffer;
		}

	}

	if (NULL != pIfaceAddr)
	{
		freeifaddrs (pIfaceAddr);
	}

	pThread = new threadTest (myAddress);

	do
	{
		std::cout << "~> ";
		std::cin >> data;
		pThread->send (data);
	} while (0 != data.compare (EXIT));

	delete pThread;

	/*pMulticast = new multicast (multicast::DEFAULT_PORT, myAddress,
															multicast::MULTICAST_GROUP);

	if (!sender)
	{
		while (stillRunning)
		{
			receiveData = pMulticast->receive ();

			if (0 < receiveData.length ())
			{

				initBuffer (sendData, multicast::BUF_LENGTH);

				data = myAddress + " : " + receiveData;

				memcpy (sendData, data.c_str (), data.size ());

				pMulticast->transmit (sendData);

				data.clear ();

				if (0 == receiveData.compare (EXIT))
				{
					stillRunning = false;
				}
			}
		}
	}
	else
	{
		do
		{
			initBuffer (sendData, multicast::BUF_LENGTH);

			std::cout << "~> ";
			std::cin >> data;

			memcpy (sendData, data.c_str (), data.size ());

			pMulticast->transmit (sendData);

			do
			{
				receiveData = pMulticast->receive ();
			} while (0 == receiveData.length ());

			std::cout << receiveData << "\n";

		} while (0 != data.compare (EXIT));
	}

	delete pMulticast;*/


	return 0;
}
