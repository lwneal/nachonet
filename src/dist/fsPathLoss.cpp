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

fsPathLoss::fsPathLoss(bool debug)
{

	setWavelength(DEFAULT_FREQ);

	this->debug = debug;
}

fsPathLoss::~fsPathLoss()
{

}

void fsPathLoss::setWavelength(int channel)
{
	float frequency = DEFAULT_FREQ + ((channel - 1) * CHANNEL_DIFF);

	setWavelength(frequency);
}

void fsPathLoss::setWavelength(float frequency)
{
	// lamda = speed of light / frequency
	wavelength = 299792458 / (frequency * pow(10,9));
}

void fsPathLoss::init()
{
	std::ifstream inFile;

	inFile.open(CONFIG);

	if(!inFile)
	{
		std::cout << "ERROR: could not open file\n";
		std::cout << "Using default values: lambda = " << wavelength << "\n";
	}
	else
	{
		inFile >> wavelength;
	}

	inFile.close();
}

bool fsPathLoss::configFileSetup()
{
	char format;
	float lambda;
	bool returnVal = true;
	std::ofstream outFile;

	outFile.open(CONFIG);

	if(!outFile)
	{
		std::cout << "ERROR: could not open file for writing\n";
		returnVal = false;
	}
	else
	{
		std::cout << "Enter the values needed for this equation: ";
		std::cout << "d = sqrt(lamba^2 / ((4*pi)^2 * 10^(P/-10)))\n";

		do
		{
			std::cout << "(c)hannel # or (r)aw frequency?: ";
			std::cin >> format;
			std::cout << "\n";
		}while ('c' != format && 'r' != format);

		if('c' == format)
		{

		}
	}

	outFile.close();

	return returnVal;
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

