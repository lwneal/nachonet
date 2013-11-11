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
#include <cmath>

class pathLoss : public distMeasure
{
	public:
		pathLoss();
		virtual ~pathLoss();
		virtual distMeasurement measure(ssMeasurement devSS);

	private:
		float d_0; //dist at which reference measurement is taken (from config file)
		float n; //environmental characterization value (from config file)
};

