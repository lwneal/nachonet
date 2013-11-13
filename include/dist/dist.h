/*******************************************************************************
File:				dist.h
Author: 		Josh Siva
Date:				11/15/13
Project:		NachoNet
Purpose:		This file defines the interface to the abstract class
						distMeasure which provides the basic structure for calculating
						a distance from a IEEE 802.11 signal strength
*******************************************************************************/

#pragma once
#include <dataCollect.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>

typedef struct distMeasurement
{
  string devID;
  float dist;
} distMeasurement;

class distMeasure
{
	public:
		virtual ~distMeasure();
		virtual void init() = 0;
		virtual distMeasurement measure(ssMeasurement devSS) = 0;

		const char * CONFIG = "dist.config";

	private:
};


