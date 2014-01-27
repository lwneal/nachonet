/*******************************************************************************
File:				stdCollect.cpp
Author: 		Josh Siva
Date:				1/26/13
Project:		NachoNet
Purpose:		Defines the behavior of the stdCollect module which utilizes
						libpcap to sniff the network
*******************************************************************************/

#include "../../include/collect/stdCollect.h"

stdCollect::stdCollect (std::string interface, bool debug) : interface ("wlan0")
{
	setDebug (debug);
}


