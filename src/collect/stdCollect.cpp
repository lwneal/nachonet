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

stdCollect::stdCollect (std::string interface, bool debug) : dataCollect (debug)
, interface ("wlan0")
{
}

void stdCollect::readFromNetwork ()
{
	pcap_t * pHandle;
	pcap_pkthdr * pHeader;
	const u_char * pPacket;
	struct ieee80211_radiotap_iterator radiotapIter;
	struct ieee80211_radiotap_header * pRadiotapHeader;
	const u_char * pAddr;
	char converted [(ETHERNET_ADDR_LEN * 2) + 1];
	u_char addr [ETHERNET_ADDR_LEN];
	char errorBuffer[PCAP_ERRBUF_SIZE];
	int status, returnVal, channel, ss;

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

		status = pcap_next_ex (pHandle, &pHeader, &pPacket);
		std::cout << "status of pcap_next_ex: " << status << std::endl;
		pRadiotapHeader = (struct ieee80211_radiotap_header * ) pPacket;

		returnVal = ieee80211_radiotap_iterator_init (&radiotapIter, pRadiotapHeader,
								pRadiotapHeader->it_len);
		do
		{
			returnVal = ieee80211_radiotap_iterator_next (&radiotapIter);
			std::cout << "radiotap_iterator_next return val: " << returnVal << "\n";

			if (returnVal >= 0)
			{
				switch (radiotapIter.this_arg_index)
				{
					case IEEE80211_RADIOTAP_CHANNEL:
						channel = le16_to_cpu (* (uint16_t *)radiotapIter.this_arg);
						std::cout << "my channel: " << channel << "\n";
						break;

					case IEEE80211_RADIOTAP_DBM_ANTSIGNAL:
						ss = (int) ((signed char)*radiotapIter.this_arg);
						std::cout << "my ss: " << ss << "dBm\n";
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

		std::cout << "from: " << converted << "\n";


		pcap_close (pHandle);
	}
	else
	{
		std::cout << "Failed to open device: " << errorBuffer << std::endl;
	}
}


