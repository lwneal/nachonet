/*******************************************************************************
File:				adminLocalization.h
Author: 		Josh Siva
Date:				12/14/13
Project:		NachoNet
Purpose:		Defines the interface to the adminLocalization module
*******************************************************************************/

#pragma once
#include "../loc/loc.h"

class adminLocalization
{
	public:
		void setDebug(bool debug);
		void test(std::string inputFile, std::ostream & os);

	private:
		bool debug;
		localization * pLoc;
};
