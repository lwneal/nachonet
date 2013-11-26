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
 *Constructor:
 *
 *Description:
 *
 *Parameters:
 *
 *Returned:
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

	this->debug = debug;
}

/******************************************************************************
 *Destructor:
 *
 *Description:
 *
 *Parameters:
 *
 *Returned:
 *****************************************************************************/
pathLoss::~pathLoss()
{

}

/******************************************************************************
 *Method:
 *
 *Description:
 *
 *Parameters:
 *
 *Returned:
 *****************************************************************************/
void pathLoss::init(Config *pConfig)
{
	std::ifstream inFile;

	inFile.open(CONFIG);

	if(!inFile)
	{
		std::cout << "ERROR: could not open file\n";
		std::cout << "Using default values: n = " << pathLoss::DEFAULT_ENV_VAL;
		std::cout << ", P_d0 = " << pathLoss::DEFAULT_POW_AT_REF << std::endl;
	}
	else
	{
		inFile >> envVal >> powerAtRefDist;
	}

	inFile.close();
}

/******************************************************************************
 *Method:
 *
 *Description:
 *
 *Parameters:
 *
 *Returned:
 *****************************************************************************/
bool pathLoss::configFileSetup()
{
	float n;
	int P_d0;
	bool returnVal = true;
	std::ofstream outFile;

	outFile.open(CONFIG);

	if(!outFile)
	{
		std::cout << "ERROR: could not open file for writing\n";
		returnVal = false;
	}
	else
	{
		std::cout << "Enter the values needed for this equation: ";
		std::cout << "d = 10^((P - P_d0) / (10 * n))";

		do
		{
			std::cout << "n: ";
			std::cin >> n;
			std::cout << "\n";
		}while(0 >= n);

		do
		{
			std::cout << "P_d0: ";
			std::cin >> P_d0;
			std::cout << "\n";
		}while(0 <= P_d0);

		outFile << n << "\n" << P_d0;
	}

	outFile.close();

	return returnVal;
}

/******************************************************************************
 *Method:
 *
 *Description:
 *
 *Parameters:
 *
 *Returned:
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


