/*******************************************************************************
File:				distDriver.cpp
Author: 		Josh Siva
Date:
Project:		NachoNet
Purpose:
*******************************************************************************/

#include "../../include/dist/fsPathLoss.h"
#include "../../include/dist/pathLoss.h"

int main(int argc, char**argv)
{
	pathLoss pl;
	fsPathLoss fspl;
	ssMeasurement devSS;
	distMeasurement devDist;

	devSS.devID = "abc123";
	devSS.ss = -60.0; //dBm is a log scale

	devDist = pl.measure(devSS);

	return 0;
}
