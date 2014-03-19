/*******************************************************************************
File:				DataCollectAdmin.h
Author: 		Josh Siva
Date:				3/18/14
Project:		NachoNet
Purpose:		Defines the interface to the admin object that will be used for
						managing a Data Collection object
*******************************************************************************/

#pragma once
#include "Admin.h"
#include "../collect/stdCollect.h"

class DataCollectAdmin : Admin
{
	public:
		DataCollectAdmin (dataCollect *);
		virtual void test ();

	private:
		dataCollect *pDataCollect;

};
