/*******************************************************************************
File:				device.h
Author: 		Josh Siva
Date:				2/25/14
Project:		NachoNet
Purpose:		Defines the device object which is an abstracted view of a device
						in NachoNet
*******************************************************************************/

#pragma once
#include <string>
#include "../loc/loc.h"

class device
{
	public:
    device (std::string id = "");
    ~device ();
    std::string getID () const;
    void setID (std::string id);
    location getLocation () const;
    void setLocation (location loc);
  private:
    std::string id;
    location loc;
};
