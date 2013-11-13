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

pathLoss::pathLoss()
{
	refDist = pathLoss::DEFAULT_REF_DIST;
	envVal = pathLoss::DEFAULT_ENV_VAL;
	powerAtRefDist = pathLoss::DEFAULT_POW_AT_REF;
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

distMeasurement pathLoss::measure(ssMeasurement devSS)
{
	distMeasurement devDist;

	devDist.devID = devSS.devID;

	devDist.dist = refDist * log( powerAtRefDist / (float)(devSS.ss) );

	return devDist;
}


