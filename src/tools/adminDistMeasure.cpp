/*******************************************************************************
File:				adminDistMeasure.cpp
Author: 		Josh Siva
Date:				11/29/13
Project:		NachoNet
Purpose:		Implements the testing and configuration behaviors of the
						adminDistMeasure module
*******************************************************************************/

#include "../../include/tools/adminDistMeasure.h"

adminDistMeasure::adminDistMeasure() : distConfig("dist.conf")
{
	debug = false;
	pDistEqn = NULL;
}

adminDistMeasure::~adminDistMeasure()
{
	if(NULL != pDistEqn)
	{
		delete pDistEqn;
	}
}

void adminDistMeasure::setDebug(bool debug)
{
	this->debug = debug;
}

void adminDistMeasure::setEqn(int eqnType)
{
	switch(eqnType)
	{
		case FREE_SPACE_PATH_LOSS:
			pDistEqn = new fsPathLoss(debug, &distConfig);
			break;

		case LOG_NORMAL_SHADOW:
			pDistEqn = new logNormalShadow(debug, &distConfig);
			break;

		default:
			pDistEqn = new pathLoss(debug, &distConfig);
			break;
	}

	pDistEqn->init(&distConfig);
}

void adminDistMeasure::configure()
{
	std::vector<std::pair<std::string, std::string>> updates;
	std::map<std::string, std::string> keyVal;
	std::map<std::string, std::string>::iterator iter;
	std::string name, input;
	std::pair<std::string, std::string> update;

	if(NULL != pDistEqn)
	{

		name = pDistEqn->getName();

		keyVal = distConfig.read(name);

		iter = keyVal.begin();

		std::cout << "Enter a number to update the value or 0 to leave it alone\n";
		std::cout << name << std::endl;

		while(keyVal.end() != iter)
		{
			std::cout << (*iter).first << " (" << (*iter).second << "): ";
			std::cin >> input;

			if(0 != input.compare("0"))
			{
				update = std::make_pair((*iter).first, input);

				updates.push_back(update);
			}

			++iter;
		}

		distConfig.write(name, updates);

	}
}

void adminDistMeasure::test(std::string inputFile, std::ostream & os)
{
	std::ifstream inFile;
	ssMeasurement devSS;
	distMeasurement devDist;

	inFile.open(inputFile.c_str());

	if(!inFile)
	{
		std::cout << "Could not open " << inputFile << std::endl;
	}
	else
	{
		if(NULL != pDistEqn)
		{
			//input file is devID SS
			while(!inFile.eof())
			{
				inFile >> devSS.devID >> devSS.ss;
				devDist = pDistEqn->measure(devSS);

				os << devDist.devID << "  " << devDist.dist << "\n";
			}
		}
	}

	inFile.close();
}



