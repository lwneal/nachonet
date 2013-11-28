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

/******************************************************************************
 *Constructor: logNormalShadow
 *
 *Description: Initializes the logNormalShadow model by setting the various
 *						 variables to their default values and then sets up the config
 *						 object to work with this object
 *
 *Parameters:	 debug - set to false by default. If true, this module will
 *										 print debugging output for the computations
 *						 pConfig - the configuration object that will save the values
 *						 					 needed by this module
 *
 *Returned:		 None
 *****************************************************************************/
logNormalShadow::logNormalShadow(bool debug, Config *pConfig) :
randomVar(0, 3.0)
{
	std::vector<std::pair<std::string, std::string>> keyVal;

	name = "logNormal";
	envVal = logNormalShadow::DEFAULT_ENV_VAL;
	powerAtRefDist = logNormalShadow::DEFAULT_POW_AT_REF;
	refDist = logNormalShadow::DEFAULT_POW_AT_REF;

	if(NULL != pConfig)
	{
		pConfig->addSection(name);

		keyVal.push_back(std::make_pair("n", std::to_string(envVal)));
		keyVal.push_back(std::make_pair("P_d0", std::to_string(powerAtRefDist)));
		keyVal.push_back(std::make_pair("d0", std::to_string(refDist)));

		pConfig->write(name, keyVal);

		noConfig = false;
	}
	else
	{
		noConfig = true;
	}


	this->debug = debug;
}

/******************************************************************************
 *Method:				init
 *
 *Description:  Uses the config object (which has hopefully been set up) to
 *							get the saved values for our calculation variables. If the
 *							config file has not been set up then we'll just stick with the
 *							default values we set up in the constructor
 *
 *Parameters:		pConfig - the configuration object that will provide a map
 *												of the values.
 *
 *Returned:			None
 *****************************************************************************/
void logNormalShadow::init(Config *pConfig)
{
	std::map<std::string, std::string> keyVal;

	if(NULL != pConfig && !noConfig)
	{
		keyVal = pConfig->read(name);

		//if there is nothing in the file then don't overwrite the default values
		if(0 == keyVal.size())
		{
			envVal = std::stof(keyVal["n"]);
			powerAtRefDist = std::stoi(keyVal["P_d0"]);
			refDist = std::stof(keyVal["d0"]);
		}
	}
}

/******************************************************************************
 *Method:				measure
 *
 *Description:	Measures the distance to the device using the log normal
 *							shadow model which is:
 *							d = d_0 * e^((P - P_d0 - X_sigma) / 10 * n)
 *
 *Parameters:		devSS - the signal strength measurement struct which has the
 *											signal strength and the device ID
 *
 *Returned:			distMeasurement - the distance and device ID
 *****************************************************************************/
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
