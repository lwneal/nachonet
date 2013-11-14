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
#include <iostream>
#include <iomanip>

fsPathLoss::fsPathLoss(float frequency, bool debug)
{
	// lamda = speed of light / frequency
	wavelength = 299792458 / frequency * pow(10,9);

	this->debug = debug;
}

fsPathLoss::~fsPathLoss()
{

}

void fsPathLoss::init()
{

}

distMeasurement fsPathLoss::measure(ssMeasurement devSS)
{
	distMeasurement devDist;

	devDist.devID = devSS.devID;

	devDist.dist = sqrt(pow(wavelength, 2) / (pow(4 * M_PI, 2)
			* pow(10.0, devSS.ss / -10.0)));

	if(debug)
	{
		std::cout << "***" << devDist.devID << "***\n";
		std::cout << std::setw(5) << "SS" << std::setw(6)<< "lamda";
		std::cout	<< std::setw(5) << "d\n";
		std::cout << "----------------\n";
		std::cout << std::setw(5) << devSS.ss << std::setw(6) << std::setprecision(4)
							<< wavelength;
		std::cout << std::setw(5) << std::setprecision(4) << devDist.dist << "\n\n";
	}

	return devDist;
}

