/*******************************************************************************
File:				fsPathLoss.h
Author: 		Josh Siva
Date:				11/15/13
Project:		NachoNet
Purpose:		Defines the interface to the fsPathLoss module which implements
						the free space path loss equation:
						d = sqrt(lamba^2 / ((4*pi)^2 * 10^(P/-10)))
*******************************************************************************/

#pragma once
#include "dist.h"
#include <cmath>

class fsPathLoss : public distMeasure
{
	public:
		virtual ~fsPathLoss();
		virtual void init();
		virtual distMeasurement measure(ssMeasurement devSS);

	private:
		float wavelength; //lambda in the above eqn (from file) based on channel
};