/*******************************************************************************
File:				logNormalShadow
Author: 		Josh Siva
Date:				11/25/13
Class:			CS460
Project:		NachoNet
Purpose:		This file defines the interface to the logNormalShadow module which
						will implement the following equation for path loss:
						d = d_0 * e^((P - P_d0 - X_sigma) / 10 * n)
*******************************************************************************/

#pragma once
#include "dist.h"
#include <random>

class logNormalShadow : public distMeasure
{
	public:
		logNormalShadow(bool debug = false);
		virtual void init();
		//virtual bool configFileSetup();
		virtual distMeasurement measure(ssMeasurement devSS);
	private:
		float refDist; //d_0
		float powerAtRefDist; //P_d0
		float envVal; //n
		std::normal_distribution<> randomVar; //X
		float stdDev; //sigma
};
