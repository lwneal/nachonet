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

static const char * CONFIG = "dist.config";

class fsPathLoss : public distMeasure
{
	public:
		fsPathLoss(bool debug = false);
		virtual ~fsPathLoss();
		virtual void init();
		virtual bool configFileSetup();
		virtual distMeasurement measure(ssMeasurement devSS);

		void setWavelength(int channel);
		void setWavelength(float frequency);

		//frequency in GHz of WiFi channel 1
		static const float DEFAULT_FREQ = 2.412;
		static const float CHANNEL_DIFF = .005;

	private:
		float wavelength; //lambda in the above eqn (from file) based on channel

		bool debug;
};
