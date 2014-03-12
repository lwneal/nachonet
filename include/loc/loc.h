/*******************************************************************************
File:				loc.h
Author: 		Josh Siva
Date:				12/8/13
Project:		NachoNet
Purpose:		Defines the interface to the localization module. This module
						is more of a set of structs and an operation than a full
						fledged module, but it is a distinct enough operation to not
						be grouped in with another module
*******************************************************************************/

#pragma once
#include "../dist/dist.h"

struct id
{
	id ()
	{
		intID = -1;
		strID = "";
	}

	int intID;
	std::string strID;
};

typedef struct location
{
	location ()
	{
		x = 0.0;
		y = 0.0;
	}

  id theID;
  float x;
  float y;
} location;

typedef struct refMeasurement
{
	location nodeLocation;
  distMeasurement devDist;
} refMeasurement;

class localization
{
  public:
		localization(bool debug = false);
    location localize(refMeasurement, refMeasurement, refMeasurement);
  private:
    bool debug;
};
