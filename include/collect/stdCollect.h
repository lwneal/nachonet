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

class stdCollect : public dataCollect
{
	public:
		stdCollect (std::string interface, bool debug = false);
		virtual void readFromNetwork ();

	private:
		std::string interface;
};
