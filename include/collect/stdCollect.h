/*******************************************************************************
File:				stdCollect.h
Author: 		Josh Siva
Date:				1/26/14
Project:		NachoNet
Purpose:		Defines the interface to the stdCollect class which uses libpcap
						to take RSSI measurements from the network.
*******************************************************************************/

#pragma once
#include "dataCollect.h"
#include <string>

#define ETHERNET_ADDR_LEN 6

class stdCollect : public dataCollect
{
	public:
		stdCollect (std::string interface, bool debug = false);
		virtual void readFromNetwork ();
		void packetHandler (u_char *args,
												const struct pcap_pkthdr * pPacketHeader,
												const u_char* pPacket);

		const int CAPTURE_LENGTH = 2048;
		const int PACKETS_TO_GRAB = 512;
		const int DEBUG_PACKETS_TO_GRAB = 128;
		const int TIMEOUT = 512;

	private:
		static void packetHandlerWrapper (u_char *args,
																		  const struct pcap_pkthdr * pPacketHeader,
																		  const u_char* pPacket);

		std::string interface;
};
