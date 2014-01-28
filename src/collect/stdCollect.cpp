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
	int status;

	pHandle = pcap_create (interface.c_str(), errorBuffer);
	if (NULL != pHandle)
	{
		if (pcap_can_set_rfmon (pHandle))
		{
			std::cout << "Can use monitor mode\n";

			if (0 == pcap_set_rfmon (pHandle, 1))
			{
				std::cout << "Monitor mode enabled\n";
			}
		}

		pcap_set_snaplen (pHandle, 2048);
		pcap_set_promisc (pHandle, 0);
		pcap_set_timeout (pHandle, 512);
		status = pcap_activate (pHandle);

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

		pcap_close (pHandle);
	}
	else
	{
		std::cout << "Failed to open device: " << errorBuffer << std::endl;
	}
}


