/*******************************************************************************
File:				stdCollect.cpp
Author: 		Josh Siva
Date:				1/26/14
Project:		NachoNet
Purpose:		Defines the behavior of the stdCollect module which utilizes
						libpcap to sniff the network
*******************************************************************************/

#include "../../include/collect/stdCollect.h"
#include <pcap.h>
#include <iostream>

stdCollect::stdCollect (std::string interface, bool debug) : dataCollect (debug)
, interface ("wlan0")
{
}

void stdCollect::readFromNetwork ()
{
	pcap_t * pHandle;
	char errorBuffer[PCAP_ERRBUF_SIZE];

	pHandle = pcap_open_live (interface.c_str(), BUFSIZ, 1, 1000, errorBuffer);

	if (pcap_datalink (pHandle) == DLT_IEEE802_11)
	{
		std::cout << "DLT_IEEE802_11\n";
	}

	if (pcap_datalink (pHandle) == DLT_IEEE802_11_RADIO)
	{
		std::cout << "DLT_IEEE802_11_RADIO\n";
	}

	pcap_close (pHandle);
}


