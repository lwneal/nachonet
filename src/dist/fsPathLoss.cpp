/*******************************************************************************
File:				fsPathLoss.cpp
Author: 		Josh Siva
Date:				11/15/13
Project:		NachoNet
Purpose:		Implements the behavior of the fsPathLoss module (free space path
						loss) which means implementing:
						d = sqrt(lamba^2 / ((4*pi)^2 * 10^(P/-10)))
*******************************************************************************/

#include "../../include/dist/fsPathLoss.h"
#include <fstream>

fsPathLoss::fsPathLoss(float frequency)
{
	wavelength = 299792458 / frequency; // lamda = speed of light / frequency
}

distMeasurement measure(ssMeasurement devSS)
{
	distMeasurement devDist;

	devDist.devID = devSS.devID;

	devDist.dist = sqrt(pow(wavelength, 2) / (pow(4 * M_PI, 2) * pow(10, devSS.ss / -10)));

	return devDist;
}

