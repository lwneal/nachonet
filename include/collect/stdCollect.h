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
#include <functional>
#include <pcap.h>

#define ETHERNET_ADDR_LEN 6

class stdCollect : public dataCollect
{
	public:
		stdCollect (std::string interface, bool debug = false);
		virtual void readFromNetwork ();


		const int CAPTURE_LENGTH = 2048;
		const int PACKETS_TO_GRAB = 512;
		const int DEBUG_PACKETS_TO_GRAB = 5;
		const int TIMEOUT = 512;

	private:
		void packetLoop (pcap_t *pHandler, int numPackets);

		std::string interface;
};
