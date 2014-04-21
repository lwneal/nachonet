/******************************************************************************
File:				dist.h
Author: 		Josh Siva
Date:				11/15/13
Project:		NachoNet
Purpose:		This file defines the interface to the abstract class
						distMeasure which provides the basic structure for calculating
						a distance from a IEEE 802.11 signal strength
******************************************************************************/

#pragma once
#include "../collect/dataCollect.h"
#include "../util/EZConfig.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iomanip>
#include <fstream>
#include <iostream>

typedef struct distMeasurement
{
	distMeasurement ()
	{
		devID = "";
		dist = 0.0;
	}

  std::string devID;
  double dist;
} distMeasurement;

class distMeasure
{
	public:
		virtual ~distMeasure(){}
		virtual void init(EZConfig *pConfig) = 0;
		virtual distMeasurement measure(ssMeasurement devSS) = 0;
		virtual std::vector<std::string> getVariables () = 0;
		std::string getName() const;

	protected:
		std::string name;
		bool noConfig;

	private:

};


