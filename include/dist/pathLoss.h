/******************************************************************************
File:				pathLoss.h
Author: 		Josh Siva
Date:				11/15/13
Project:		NachoNet
Purpose:		The file defines the interface to the pathLoss module which will
						implement the following path loss equation:
						d = 10^((P - P_d0) / (10 * n))
******************************************************************************/

#pragma once
#include "dist.h"

class pathLoss : public distMeasure
{
	public:
		pathLoss(bool debug = false, EZConfig *pConfig = NULL);
		virtual ~pathLoss();
		virtual void init(EZConfig *pConfig);
		virtual distMeasurement measure(ssMeasurement devSS);
		virtual std::vector<std::string> getVariables ();

		const float DEFAULT_ENV_VAL = -.2;
		static const int DEFAULT_POW_AT_REF = -32; //the power at reference dist
		const float DEFAULT_REF_DIST = .3048;

	private:
		//n
		float envVal; //environmental characterization value
		//P_d0
		int powerAtRefDist;
		//d0
		float refDist;

		bool debug;
};

