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

class fsPathLoss : public distMeasure
{
	public:
		fsPathLoss(float frequency = 2.412); //frequency in GHz of WiFi channel 1
		virtual ~fsPathLoss();
		virtual distMeasurement measure(ssMeasurement devSS);

	private:
		float wavelength; //lambda in the above eqn (from file) based on channel
};
