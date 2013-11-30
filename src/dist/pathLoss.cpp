/******************************************************************************
File:				pathLoss.cpp
Author: 		Josh Siva
Date:				11/15/13
Project:		NachoNet
Purpose:		The file implements the behavior of the pathLoss module which means
						implementing the following equation:
						d = 10^((P - P_d0) / (10 * n))
******************************************************************************/

#include "../../include/dist/pathLoss.h"
#include <fstream>
#include <iostream>
#include <iomanip>

/******************************************************************************
 *Constructor: pathLoss
 *
 *Description: Initializes the object by setting default values for the
 *						 variables and setting up the config module to hold the variables
 *						 used by this module.
 *
 *Parameters:	 debug - set to false by default. If true, this module will
 *										 print debugging output for the computations
 *						 pConfig - the configuration object that will save the values
 *						 					 needed by this module
 *
 *Returned:		 None
 *****************************************************************************/
pathLoss::pathLoss(bool debug, Config *pConfig)
{
	std::vector<std::pair<std::string, std::string>> keyVal;

	name = "pathLoss";
	envVal = pathLoss::DEFAULT_ENV_VAL;
	powerAtRefDist = pathLoss::DEFAULT_POW_AT_REF;


	if(NULL != pConfig)
	{
		//so that we do not overwrite
		if(Config::SECTION_EXISTS != pConfig->addSection(name))
		{
			keyVal.push_back(std::make_pair("n", std::to_string(envVal)));
			keyVal.push_back(std::make_pair("P_d0", std::to_string(powerAtRefDist)));

			pConfig->write(name, keyVal);
			pConfig->save();
		}



		noConfig = false;
	}
	else
	{
		noConfig = true;
	}

	this->debug = debug;
}

/******************************************************************************
 *Destructor:		pathLoss
 *
 *Description:	Does nothing
 *
 *Parameters:   None
 *
 *Returned:			None
 *****************************************************************************/
pathLoss::~pathLoss()
{

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
void pathLoss::init(Config *pConfig)
{
	std::map<std::string, std::string> keyVal;

	if(NULL != pConfig && !noConfig)
	{
		keyVal = pConfig->read(name);

		//if there is nothing in the file then don't overwrite the default values
		if(0 != keyVal.size())
		{
			envVal = std::stof(keyVal["n"]);
			powerAtRefDist = std::stoi(keyVal["P_d0"]);
		}
	}

}


/******************************************************************************
 *Method:				measure
 *
 *Description:  Measures the distance of a device based on the signal strength
 *							from the device. The equation, once more, is:
 *							d = 10^((P - P_d0) / (10 * n))
 *
 *Parameters:   devSS - the signal strength measurement struct which has the
 *											signal strength and the device ID
 *
 *Returned:			distMeasurement - the distance and device ID
 *****************************************************************************/
distMeasurement pathLoss::measure(ssMeasurement devSS)
{
	distMeasurement devDist;

	devDist.devID = devSS.devID;

	devDist.dist = pow(10, (devSS.ss - powerAtRefDist) / (10 * envVal));

	if(debug)
	{
		std::cout << "***" << devDist.devID << "***\n";
		std::cout << std::setw(10) << "SS" << std::setw(10) << "P_d0";
		std::cout << std::setw(10) << "n" << std::setw(10) << "d\n";
		std::cout << "----------------------------------------\n";
		std::cout << std::setw(10) << devSS.ss << std::setw(10)
							<< powerAtRefDist << std::setw(10) << std::setprecision(4)
							<< envVal << std::setw(10) << devDist.dist << "\n\n";
	}

	return devDist;
}


