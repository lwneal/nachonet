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
		fsPathLoss(bool debug = false, EZConfig *pConfig = NULL);
		virtual ~fsPathLoss();
		virtual void init(EZConfig *pConfig);
		virtual distMeasurement measure(ssMeasurement devSS);
		virtual std::vector<std::string> getVariables ();

		void setWavelength(int channel);
		void setWavelength(double frequency);

		//frequency in GHz of WiFi channel 1
		const float DEFAULT_FREQ = 2.412;

		const float CHANNEL_DIFF = .005;
		static const int MIN_CHANNEL = 1;
		static const int MAX_CHANNEL = 12;

		static const char CHANNEL = 'c';
		static const char FREQUENCY = 'f';

	private:
		double wavelength; //lambda in the above eqn (from file) based on channel

		bool debug;
};
