/*******************************************************************************
File:				pathLoss.h
Author: 		Josh Siva
Date:				11/15/13
Project:		NachoNet
Purpose:		The file defines the interface to the pathLoss module which will
						implement the following path loss equation:
						d = d_0 * lg_n(P_d0 / P_r)
*******************************************************************************/

#pragma once
#include "dist.h"

class pathLoss : public distMeasure
{
	public:
		pathLoss(bool debug = false);
		virtual ~pathLoss();
		virtual void init();
		virtual distMeasurement measure(ssMeasurement devSS);


		//These aren't entirely arbitrary. They were chosen by looking at the plot
		//on page 13 of [Pu et. al. 2011]
		static const float DEFAULT_REF_DIST = 1;
		static const float DEFAULT_ENV_VAL = 3.5;
		static const float DEFAULT_POW_AT_REF = 1;

	private:
		//d_0
		float refDist; //dist at which reference measurement is taken
									 //(from config file)
		//n
		float envVal; //environmental characterization value (from config file)
		//P_d0
		float powerAtRefDist;

		bool debug;
};

