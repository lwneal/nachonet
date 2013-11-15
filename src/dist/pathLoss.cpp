/*******************************************************************************
File:				pathLoss.cpp
Author: 		Josh Siva
Date:				11/15/13
Project:		NachoNet
Purpose:		The file implements the behavior of the pathLoss module which means
						implementing the following equation:
						d = d_0 * lg_n(P_d0 / P_r)
*******************************************************************************/

#include "../../include/dist/pathLoss.h"
#include <fstream>
#include <iostream>
#include <iomanip>

pathLoss::pathLoss(bool debug)
{
	refDist = pathLoss::DEFAULT_REF_DIST;
	envVal = pathLoss::DEFAULT_ENV_VAL;
	powerAtRefDist = pathLoss::DEFAULT_POW_AT_REF;

	this->debug = debug;
}

pathLoss::~pathLoss()
{

}

//expects d_0 then n then P_d0
void pathLoss::init()
{
	std::ifstream inFile;

	inFile.open(CONFIG);

	if(!inFile)
	{
		std::cout << "ERROR: could not open file\n";
		std::cout << "Using default values: d_0 = "<< pathLoss::DEFAULT_REF_DIST;
		std::cout << ", n = " << pathLoss::DEFAULT_ENV_VAL << std::endl;
	}
	else
	{
		inFile >> refDist >> envVal >> powerAtRefDist;
	}

	inFile.close();
}

bool pathLoss::configFileSetup()
{
	float d_0, n;
	int P_d0;
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
		std::cout << "Enter the values needed for this equation: d = d_0 * lg_n(P_d0 / P_r)\n";

		do
		{
			std::cout << "d_0: ";
			std::cin >> d_0;
			std::cout << "\n";
		}while(0 >= d_0);

		do
		{
			std::cout << "n: ";
			std::cin >> n;
			std::cout << "\n";
		}while(0 >= n);

		do
		{
			std::cout << "P_d0: ";
			std::cin >> P_d0;
			std::cout << "\n";
		}while(0 <= P_d0);

		outFile << d_0 << "\n" << n << "\n" << P_d0;
	}

	outFile.close();

	return returnVal;
}

distMeasurement pathLoss::measure(ssMeasurement devSS)
{
	distMeasurement devDist;

	devDist.devID = devSS.devID;

	devDist.dist = refDist * (log( powerAtRefDist / (float)(devSS.ss) ) /
			log(envVal));

	if(debug)
	{
		std::cout << "***" << devDist.devID << "***\n";
		std::cout << std::setw(5) << "SS" << std::setw(5) << "d_0";
		std::cout << std::setw(5) << "P_d0" << std::setw(5) << "n\n";
		std::cout << "--------------------\n";
		std::cout << std::setw(5) << devSS.ss << std::setw(5)
							<< std::setprecision(3) << refDist << std::setw(5)
							<< powerAtRefDist << std::setw(5) << std::setprecision(4)
							<< envVal << "\n\n";
	}

	return devDist;
}


