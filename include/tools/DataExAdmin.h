/*******************************************************************************
File:				DataExAdmin.h
Author: 		Josh Siva
Date:				3/19/14
Project:		NachoNet
Purpose:		Defines the interface to the admin tool responsible for managing
						the Data Exchange object.
*******************************************************************************/

#pragma once
#include "Admin.h"
#include "../exch/dataEx.h"
#include <ctime>

class dataEx;

class DataExAdmin : public Admin
{
	public:
		DataExAdmin (NachoNet *pNacho);
		virtual void configure ();
		virtual void test ();

		static const int TEST_DURATION = 20;
};
