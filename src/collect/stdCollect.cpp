/*******************************************************************************
File:				stdCollect.cpp
Author: 		Josh Siva
Date:				1/26/14
Project:		NachoNet
Purpose:		Defines the behavior of the stdCollect module which utilizes
						libpcap to sniff the network
*******************************************************************************/

#include "../../include/collect/stdCollect.h"
#include "../../extern/radiotapParser/radiotap-parser.h"
#include <pcap.h>
#include <iostream>
#include <cstring>

void * pgObject;

/******************************************************************************
 * Constructor: stdCollect
 *
 * Description: Initialize the interface and pass of the debug variable to the
 * 							parent class
 *
 * Parameters:	interface - the name of the interface to capture on
 * 							debug			- true if in debug mode, false otherwise
 *
 * Returned:		None
 *****************************************************************************/
stdCollect::stdCollect (std::string interface, bool debug) : dataCollect (debug)
, interface ("wlan0")
{
}

/******************************************************************************
 * Method: 		packetHandler
 *
 * Description: Callback function that is used on each packet grabbed from the
 * 							network. The packet headers are parsed to pick out the signal
 * 							strength of the received packet and the MAC address of the
 * 							sender.
 *
 * Parameters:	args 					- any arguments passed to the callback function
 * 							pPacketHeader - a pointer to a struct that contains the packet
 * 															header
 * 							pPacket       - the packet
 *
 * Returned:		None
 *****************************************************************************/
void stdCollect::packetHandler (u_char *args,
																const struct pcap_pkthdr * pPacketHeader,
																const u_char* pPacket)
{
	struct ieee80211_radiotap_iterator radiotapIter;
	struct ieee80211_radiotap_header * pRadiotapHeader;
	const u_char * pAddr;
	char converted [(ETHERNET_ADDR_LEN * 2) + 1];
	u_char addr [ETHERNET_ADDR_LEN];
	static std::string lastDevID ("000000000000");
	std::string currentDevID;
	int returnVal, channel, ss;

	pRadiotapHeader = (struct ieee80211_radiotap_header * ) pPacket;

	returnVal = ieee80211_radiotap_iterator_init (&radiotapIter, pRadiotapHeader,
							pRadiotapHeader->it_len);
	do
	{
		returnVal = ieee80211_radiotap_iterator_next (&radiotapIter);

		if (returnVal >= 0)
		{
			switch (radiotapIter.this_arg_index)
			{
				case IEEE80211_RADIOTAP_CHANNEL:
					channel = le16_to_cpu (* (uint16_t *)radiotapIter.this_arg);
					break;

				case IEEE80211_RADIOTAP_DBM_ANTSIGNAL:
					ss = (int) ((signed char)*radiotapIter.this_arg);
					break;

				default:
					break;
			}
		}

	} while (returnVal >= 0);

	pAddr = (pPacket + pRadiotapHeader->it_len + 10);
	memcpy (addr, pAddr, sizeof (addr));

	for (int i = 0; i < ETHERNET_ADDR_LEN; i++)
	{
		sprintf (&converted[i * 2], "%02X", addr[i]);
	}

	currentDevID.assign(converted);

	if (0 != currentDevID.compare(lastDevID))
	{
		if (isDebug ())
		{
			std::cout << channel << "   " << ss << "   " << currentDevID << "\n";
		}

		update (currentDevID, ss);
		lastDevID.assign (currentDevID);
	}

}

/******************************************************************************
 * Method: 		packetHandlerWrapper
 *
 * Description: This is a wrapper for the callback function needed by pcap_loop
 * 							because the function needs to fit the parameters of the pcap
 * 							function but also needs to be able to call update.
 *
 * Parameters:	args 					- any arguments passed to the callback function
 * 							pPacketHeader - a pointer to a struct that contains the packet
 * 															header
 * 							pPacket       - the packet
 *
 * Returned:		None
 *****************************************************************************/
void stdCollect::packetHandlerWrapper(u_char *args,
																const struct pcap_pkthdr * pPacketHeader,
																const u_char* pPacket)
{
	stdCollect * pSelf = (stdCollect *)pgObject;

	pSelf->packetHandler (args, pPacketHeader, pPacket);
}

/******************************************************************************
 * Method: 			readFromNetwork
 *
 * Description: Prepares the device to be read from and then grabs
 * 							PACKETS_TO_GRAB packets to analyze.
 *
 * Parameters:	None
 *
 * Returned:		None
 *****************************************************************************/
void stdCollect::readFromNetwork ()
{
	pcap_t * pHandle;
	u_char * pArgs = NULL;
	char errorBuffer[PCAP_ERRBUF_SIZE];
	int numPackets;

	pgObject = (void *)this;

	pHandle = pcap_create (interface.c_str(), errorBuffer);
	if (NULL != pHandle)
	{
		if (pcap_can_set_rfmon (pHandle))
		{
			if (isDebug ())
			{
				std::cout << "Can use monitor mode\n";
			}

			if (0 == pcap_set_rfmon (pHandle, 1))
			{
				if (isDebug ())
				{
					std::cout << "Monitor mode enabled\n";
				}
			}
		}

		pcap_set_snaplen (pHandle, CAPTURE_LENGTH); //bytes to capture from packet
		pcap_set_promisc (pHandle, 0);							//disable promiscuous mode
		pcap_set_timeout (pHandle, TIMEOUT);				//set timeout
		pcap_activate (pHandle);

		if (isDebug ())
		{
			if (pcap_datalink (pHandle) == DLT_IEEE802_11)
			{
				std::cout << "DLT_IEEE802_11\n";
			}
			else if (pcap_datalink (pHandle) == DLT_IEEE802_11_RADIO)
			{
				std::cout << "DLT_IEEE802_11_RADIO\n";
			}
			else if (pcap_datalink (pHandle) == DLT_EN10MB)
			{
				std::cout << "DLT_EN10MB\n";
			}
			else
			{
				std::cout << "something else\n";
			}
		}

			if (isDebug ())
			{
				numPackets = DEBUG_PACKETS_TO_GRAB;
			}
			else
			{
				numPackets = PACKETS_TO_GRAB;
			}


		pcap_loop (pHandle, numPackets, stdCollect::packetHandlerWrapper,
																					pArgs);


		pcap_close (pHandle);
	}
	else
	{
		std::cout << "Failed to open device: " << errorBuffer << std::endl;
	}
}


