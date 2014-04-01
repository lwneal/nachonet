/******************************************************************************
File:				fsPathLoss.cpp
Author: 		Josh Siva
Date:				11/15/13
Project:		NachoNet
Purpose:		Implements the behavior of the fsPathLoss module (free space path
						loss) which means implementing:
						d = sqrt(lamba^2 / ((4*pi)^2 * 10^(P/-10)))
******************************************************************************/

#include "../../include/dist/fsPathLoss.h"
#include <fstream>
#include <iostream>
#include <iomanip>

/******************************************************************************
 *Constructor: fsPathLoss
 *
 *Description: Initializes the fsPathLoss object by initializing the wavelength
 *						 and setting up the config object to work with this module
 *
 *Parameters:	 debug - set to false by default. If true, this module will
 *										 print debugging output for the computations
 *						 pConfig - the configuration object that will save the values
 *						 					 needed by this module
 *
 *Returned:		 None
 *****************************************************************************/
fsPathLoss::fsPathLoss(bool debug, EZConfig *pConfig)
{
	jsonData data;

	name = "fsPathLoss";
	setWavelength(MIN_CHANNEL);

	if(NULL != pConfig)
	{
		if(!pConfig->sectionExists(name))
		{
			data.type = jsonParser::INT_TYPE;
			data.value.intVal = MIN_CHANNEL;

			pConfig->write (name, "channel", data);

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
 *Destructor:		fsPathLoss
 *
 *Description:	Does nothing
 *
 *Parameters:		None
 *
 *Returned:			None
 *****************************************************************************/
fsPathLoss::~fsPathLoss()
{
}

/******************************************************************************
 *Method:				setWavelength
 *
 *Description:	Sets the wavelength based on the WiFi channel.
 *
 *Parameters:		channel - the integer between 1 and 12 that indicates the WiFi
 *												channel
 *
 *Returned:			None
 *****************************************************************************/
void fsPathLoss::setWavelength(int channel)
{
	float frequency = DEFAULT_FREQ + ((channel - 1) * CHANNEL_DIFF);

	setWavelength(frequency);
}

/******************************************************************************
 *Method:				setWavelength
 *
 *Description:	Sets the wavelength based on the frequency of the wave and
 *							using the equation: lamda = speed of light / frequency
 *
 *Parameters:		frequency - the frequency in GHz of the wave
 *
 *Returned:			None
 *****************************************************************************/
void fsPathLoss::setWavelength(float frequency)
{
	wavelength = 299792458 / (frequency * pow(10,9));
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
void fsPathLoss::init(EZConfig *pConfig)
{
	JSON json;

	if(NULL != pConfig && !noConfig)
	{
		json = pConfig->getSection (name);

		//if there is nothing in the file then don't overwrite the default values
		if(0 != json.getData ("channel").value.intVal)
		{

			setWavelength(json.getData ("channel").value.intVal);
		}
	}

}

/******************************************************************************
 *Method:				measure
 *
 *Description:	Measures the distance of a device based on the free space path
 *							loss model which is, once more:
 *							d = sqrt(lamba^2 / ((4*pi)^2 * 10^(P/-10)))
 *
 *Parameters:		devSS - the signal strength measurement struct which has the
 *											signal strength and the device ID
 *
 *Returned:			distMeasurement - the distance and device ID
 *****************************************************************************/
distMeasurement fsPathLoss::measure(ssMeasurement devSS)
{
	distMeasurement devDist;

	devDist.devID = devSS.devID;

	devDist.dist = sqrt(pow(wavelength, 2) / (pow(4 * M_PI, 2)
			* pow(10.0, devSS.ss / 10.0)));

	if(debug)
	{
		std::cout << "***" << devDist.devID << "***\n";
		std::cout << std::setw(10) << "SS" << std::setw(10)<< "lamda";
		std::cout	<< std::setw(10) << "d\n";
		std::cout << "------------------------------\n";
		std::cout << std::setw(10) << devSS.ss << std::setw(10) << std::setprecision(4)
							<< wavelength;
		std::cout << std::setw(10) << std::setprecision(4) << devDist.dist << "\n\n";
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
std::vector<std::string> fsPathLoss::getVariables ()
{
	std::vector<std::string> varNames;

	varNames.push_back ("channel");

	return varNames;
}


