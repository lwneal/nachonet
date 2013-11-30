/******************************************************************************
File:				distDriver.cpp
Author: 		Josh Siva
Date:
Project:		NachoNet
Purpose:
******************************************************************************/

#include "../../include/dist/fsPathLoss.h"
#include "../../include/dist/pathLoss.h"
#include "../../include/dist/logNormalShadow.h"

#include "../../include/util/config.h"

int main(int argc, char**argv)
{
	Config config("testfiles/distTest.conf");
	pathLoss pl(true, &config);
	fsPathLoss fspl(true, &config);
	logNormalShadow lns(true, &config);
	ssMeasurement devSS;
	distMeasurement devDist;

	pl.init(&config);
	fspl.init(&config);
	lns.init(&config);


	devSS.devID = "abc123";
	devSS.ss = -60.0; //dBm is a log scale

	devDist = pl.measure(devSS);
	devDist = fspl.measure(devSS);
	devDist = lns.measure(devSS);


	return 0;
}
