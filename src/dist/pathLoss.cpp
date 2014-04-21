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
pathLoss::pathLoss(bool debug, EZConfig *pConfig)
{
	std::vector<std::pair<std::string, std::string>> keyVal;
	jsonData data;

	name = "pathLoss";
	envVal = pathLoss::DEFAULT_ENV_VAL;
	powerAtRefDist = pathLoss::DEFAULT_POW_AT_REF;
	refDist = pathLoss::DEFAULT_REF_DIST;


	if(NULL != pConfig)
	{
		//so that we do not overwrite
		if(!pConfig->sectionExists(name))
		{
			data.type = jsonParser::FLT_TYPE;
			data.value.floatVal = envVal;
			pConfig->write (name, "n", data);

			data.value.floatVal = refDist;
			pConfig->write (name, "d0", data);

			data.type = jsonParser::INT_TYPE;
			data.value.intVal = powerAtRefDist;
			pConfig->write (name, "P_d0", data);

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
void pathLoss::init(EZConfig *pConfig)
{
	JSON * pJson;

	if(NULL != pConfig && !noConfig)
	{
		pJson = pConfig->getSection (name);

		//if there is nothing in the file then don't overwrite the default values
		if(0.0f != pJson->getData ("n").value.floatVal
			 && 0 != pJson->getData ("P_d0").value.intVal
			 && 0.0f != pJson->getData ("d0").value.floatVal)
		{
			envVal = pJson->getData ("n").value.floatVal;
			powerAtRefDist = pJson->getData ("P_d0").value.intVal;
			refDist = pJson->getData ("d0").value.floatVal;
		}
	}

}


/******************************************************************************
 *Method:				measure
 *
 *Description:  Measures the distance of a device based on the signal strength
 *							from the device. The equation, once more, is:
 *							d = ((P_d0 * d_0^n) /  P) ^ (1/n)
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

	devDist.dist = pow (powerAtRefDist * pow (refDist, envVal) / devSS.ss,
									1 / envVal);

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

/******************************************************************************
 *Method:				getVariables
 *
 *Description:  Get the symbols/names of the variables needed by this equation
 *
 *Parameters:   None
 *
 *Returned:			vector of strings - the variables names
 *****************************************************************************/
std::vector<std::string> pathLoss::getVariables ()
{
	std::vector<std::string> varNames;

	varNames.push_back ("n");
	varNames.push_back ("P_d0");
	varNames.push_back ("d0");

	return varNames;
}


