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

typedef struct id
{
	int intID;
	std::string strID;
};

typedef struct location
{
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
    location localize(refMeasurement, refMeasurement, refMeasurement);
  private:
};
