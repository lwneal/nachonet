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
logNormalShadow::logNormalShadow(bool debug, EZConfig *pConfig) :
randomVar(0, 3.0)
{
	jsonData data;

	name = "logNormal";
	envVal = logNormalShadow::DEFAULT_ENV_VAL;
	powerAtRefDist = logNormalShadow::DEFAULT_REF_DIST;
	refDist = logNormalShadow::DEFAULT_POW_AT_REF;

	if(NULL != pConfig)
	{

		if (!pConfig->sectionExists (name))
		{
			data.type = jsonParser::FLT_TYPE;
			data.value.floatVal = envVal;
			pConfig->write (name, "n", data);

			data.value.floatVal = powerAtRefDist;
			pConfig->write (name, "P_d0", data);

			data.value.floatVal = refDist;
			pConfig->write (name, "d0", data);

			pConfig->save ();
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
void logNormalShadow::init(EZConfig *pConfig)
{
	JSON json;

	if(NULL != pConfig && !noConfig)
	{
		json = pConfig->getSection (name);

		//if there is nothing in the file then don't overwrite the default values
		if(0.0f != json.getData ("n").value.floatVal
			 && 0.0f != json.getData ("P_d0").value.floatVal
			 && 0.0f != json.getData ("d0").value.floatVal)
		{
			envVal = json.getData ("n").value.floatVal;
			powerAtRefDist = json.getData ("P_d0").value.floatVal;
			refDist = json.getData ("d0").value.floatVal;
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
		std::cout << std::setw(10) << "SS" << std::setw(10)<< "d_0"
							<< std::setw(10) << "P_d0" << std::setw(10)<< "X"
							<< std::setw(10) << "n" << std::setw(10) << "d\n";

		std::cout << "------------------------------------------------------------\n";

		std::cout << std::setw(10) << devSS.ss << std::setw(10) << std::setprecision(2)
							<< refDist << std::setw(10) << std::setprecision(2) << powerAtRefDist
							<< std::setw(10) << std::setprecision(4) << random
							<< std::setw(10) << std::setprecision(4) << envVal
							<< std::setw(10) << std::setprecision(4) << devDist.dist << "\n\n";
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
std::vector<std::string> logNormalShadow::getVariables ()
{
	std::vector<std::string> varNames;

	varNames.push_back ("n");
	varNames.push_back ("P_d0");
	varNames.push_back ("d0");

	return varNames;
}
