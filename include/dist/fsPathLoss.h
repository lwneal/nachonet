/******************************************************************************
File:				fsPathLoss.h
Author: 		Josh Siva
Date:				11/15/13
Project:		NachoNet
Purpose:		Defines the interface to the fsPathLoss module which implements
						the free space path loss equation:
						d = sqrt(lamba^2 / ((4*pi)^2 * 10^(P/-10)))
******************************************************************************/

#pragma once
#include "dist.h"


class fsPathLoss : public distMeasure
{
	public:
		fsPathLoss(bool debug = false, Config *pConfig);
		virtual ~fsPathLoss();
		virtual void init(Config *pConfig);
		virtual bool configFileSetup();
		virtual distMeasurement measure(ssMeasurement devSS);

		void setWavelength(int channel);
		void setWavelength(float frequency);

		//frequency in GHz of WiFi channel 1
		static const float DEFAULT_FREQ = 2.412;

		static const float CHANNEL_DIFF = .005;
		static const int MIN_CHANNEL = 1;
		static const int MAX_CHANNEL = 12;

		static const char CHANNEL = 'c';
		static const char FREQUENCY = 'f';

	private:
		float wavelength; //lambda in the above eqn (from file) based on channel

		bool debug;
};
