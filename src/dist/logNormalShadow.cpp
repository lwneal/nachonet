/*******************************************************************************
File:				logNormalShadow.cpp
Author: 		Josh Siva
Date:				11/25/13
Project:		NachoNet
Purpose:		Defines the behavior of the logNormalShadow object which means
						implementing:
						d = d_0 * e^((P - P_d0 - X_sigma) / 10 * n)
*******************************************************************************/

#include "../../include/dist/logNormalShadow.h"

logNormalShadow::logNormalShadow(bool debug, Config *pConfig) :
name("logNormal"), randomVar(0, 3.0)
{
	std::vector<std::pair<std::string, float>> keyVal;

	pConfig->addSection(name);

	envVal = logNormalShadow::DEFAULT_ENV_VAL;
	powerAtRefDist = logNormalShadow::DEFAULT_POW_AT_REF;
	refDist = logNormalShadow::DEFAULT_POW_AT_REF;

	keyVal.push_back(std::make_pair("n", envVal));
	keyVal.push_back(std::make_pair("P_d0", powerAtRefDist));
	keyVal.push_back(std::make_pair("d0", refDist));

	pConfig->write(name, keyVal);

	this->debug = debug;
}

void logNormalShadow::init(Config *pConfig)
{
	std::map<std::string, float> keyVal;

	keyVal = pConfig->read(name);

	//if there is nothing in the file then don't overwrite the default values
	if(0 == keyVal.size())
	{
		envVal = keyVal["n"];
		powerAtRefDist = keyVal["P_d0"];
		refDist = keyVal["d0"];
	}

}

distMeasurement logNormalShadow::measure(ssMeasurement devSS)
{
	distMeasurement devDist;
	float random = randomVar(generator);
	devDist.devID = devSS.devID;

	devDist.dist = refDist * exp(((float)(devSS.ss) - (float)(powerAtRefDist) -
																random) / (10.0 * envVal));

	if(debug)
	{
		std::cout << "***" << devDist.devID << "***\n";
		std::cout << std::setw(8) << "SS" << std::setw(8)<< "d_0"
							<< std::setw(8) << "P_d0" << std::setw(8)<< "X"
							<< std::setw(8) << "n" << std::setw(8) << "d\n";

		std::cout << "------------------------------------------------\n";

		std::cout << std::setw(8) << devSS.ss << std::setw(8) << std::setprecision(2)
							<< refDist << std::setw(8) << std::setprecision(2) << powerAtRefDist
							<< std::setw(8) << std::setprecision(4) << random
							<< std::setw(8) << std::setprecision(4) << envVal
							<< std::setw(8) << std::setprecision(8) << devDist.dist << "\n\n";
	}

	return devDist;
}
