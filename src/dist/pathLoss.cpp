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
pathLoss::pathLoss(bool debug, Config *pConfig) : name("pathLoss")
{
	std::vector<std::pair<std::string, float>> keyVal;

	pConfig->addSection(name);

	envVal = pathLoss::DEFAULT_ENV_VAL;
	powerAtRefDist = pathLoss::DEFAULT_POW_AT_REF;

	keyVal.push_back(std::make_pair("n", envVal));
	keyVal.push_back(std::make_pair("P_d0", powerAtRefDist));

	pConfig->write(name, keyVal);
	pConfig->save();

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
	std::map<std::string, float> keyVal;

	keyVal = pConfig->read(name);

	//if there is nothing in the file then don't overwrite the default values
	if(0 == keyVal.size())
	{
		envVal = keyVal["n"];
		powerAtRefDist = keyVal["P_d0"];
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

	devDist.dist = pow(10, (devSS.ss - powerAtRefDist) / 10 * envVal);

	if(debug)
	{
		std::cout << "***" << devDist.devID << "***\n";
		std::cout << std::setw(5) << "SS";
		std::cout << std::setw(5) << "P_d0" << std::setw(5) << "n\n";
		std::cout << "--------------------\n";
		std::cout << std::setw(5) << devSS.ss << std::setw(5)
							<< powerAtRefDist << std::setw(5) << std::setprecision(4)
							<< envVal << "\n\n";
	}

	return devDist;
}


