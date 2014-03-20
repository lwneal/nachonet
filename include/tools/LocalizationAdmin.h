/*******************************************************************************
File:				LocalizationAdmin.h
Author: 		Josh Siva
Date:				3/19/14
Project:		NachoNet
Purpose:		Defines the interface to the admin tool that manages the
						Localization object
*******************************************************************************/

#pragma once
#include "Admin.h"
#include "../loc/loc.h"

class LocalizationAdmin : public Admin
{
	public:
		LocalizationAdmin (NachoNet *pNacho);
		virtual void test ();

};
