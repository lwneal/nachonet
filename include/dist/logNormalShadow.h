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
		logNormalShadow(bool debug = false, Config *config = NULL);
		~logNormalShadow() {};
		virtual void init(Config *pConfig);
		virtual distMeasurement measure(ssMeasurement devSS);

		const float DEFAULT_REF_DIST = 1;
		const int DEFAULT_POW_AT_REF = 38;
		const float DEFAULT_ENV_VAL = 3.5;
	private:
		bool debug;
		float refDist; //d_0
		int powerAtRefDist; //P_d0
		float envVal; //n
		std::default_random_engine generator;
		std::normal_distribution<float> randomVar; //X
};
