/*******************************************************************************
File:				DistMeasureAdmin.cpp
Author: 		Josh Siva
Date:				3/18/14
Project:		NachoNet
Purpose:		Implements the admin tool that manages the Distance Measurement
						object
*******************************************************************************/

#include "../../include/tools/DistMeasureAdmin.h"


DistMeasureAdmin::DistMeasureAdmin (NachoNet *pNacho) : Admin (pNacho),
distConfig (NachoNet::DIST_CONFIG_FILE)
{
}

/*******************************************************************************
 * Method:			options
 *
 * Description:	Sets up the equation that is to be used by NachoNet
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void DistMeasureAdmin::options ()
{
	int choice;

	if (NULL != pNacho->pDataEx && pNacho->pDataEx->alive ())
	{
		std::cout << "You can't do that while NachoNet is running!\n";
	}
	else
	{

		std::cout << "\nPlease select an equation to use:\n";
		std::cout << "1) Path Loss (default)\n";
		std::cout << "2) Free Space Path Loss\n";
		std::cout << "3) Log Normal Shadow Model\n";
		do
		{
			std::cout << "\nEnter your selection: ";
			std::cin >> choice;
		} while (PATH_LOSS != choice && FREE_SPACE_PATH_LOSS != choice
						 && LOG_NORMAL_SHADOW != choice);

		switch ( (int)choice)
		{
			case FREE_SPACE_PATH_LOSS:
				delete pNacho->pDistMeasure;
				pNacho->pDistMeasure = new fsPathLoss (pNacho->isDebug (), &distConfig);
				break;

			case LOG_NORMAL_SHADOW:
				delete pNacho->pDistMeasure;
				pNacho->pDistMeasure = new logNormalShadow (pNacho->isDebug (),
																										&distConfig);
				break;

			default:
				delete pNacho->pDistMeasure;
				pNacho->pDistMeasure = new pathLoss (pNacho->isDebug (), &distConfig);
				break;
		}

		std::cout << "Check the configuration for the current equation\n";
	}
}

/*******************************************************************************
 * Method:			configure
 *
 * Description:	Configures the Distance Measurement object's equation with the
 * 							appropriate constants.
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void DistMeasureAdmin::configure ()
{
	std::vector<std::pair<std::string, std::string>> updates;
	std::map<std::string, std::string> keyVal;
	std::map<std::string, std::string>::iterator iter;
	std::string name, input;
	std::pair<std::string, std::string> update;

	jsonData data;
	JSON json;
	std::vector<std::string> varNames;

	if (NULL != pNacho->pDataEx && pNacho->pDataEx->alive ())
	{
		std::cout << "You can't do that while NachoNet is running!\n";
	}
	else
	{

		if(NULL != pNacho->pDistMeasure)
		{

			name = pNacho->pDistMeasure->getName();

			json = distConfig.getSection (name);
			varNames = pNacho->pDistMeasure->getVariables ();

			std::cout << "Enter a number to update the value or 0 to leave it alone\n";
			std::cout << name << std::endl;

			for (auto & var : varNames)
			{
				std::cout << var << " (";

				if (jsonParser::INT_TYPE == json.getData (var).type)
				{
					std::cout << json.getData (var).value.intVal;
				}
				else if (jsonParser::FLT_TYPE == json.getData (var).type)
				{
					std::cout << json.getData (var).value.floatVal;
				}

				std::cout << "): ";
				std::cin >> input;

				if(0 != input.compare("0"))
				{
					data = json.getData (var);

					if (jsonParser::INT_TYPE == data.type)
					{
						data.value.intVal = std::atoi (input.c_str ());
					}
					else if (jsonParser::FLT_TYPE == data.type)
					{
						data.value.floatVal = std::atof (input.c_str ());
					}

					distConfig.write (name, var, data);
				}
			}

			distConfig.save ();
		}
	}
}

/*******************************************************************************
 * Method:			test
 *
 * Description: Calls the parent method, gets an input file from the user and
 * 							then produces measurements for all of the signal strengths in
 * 							the input file.
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void DistMeasureAdmin::test ()
{
	std::ifstream testFile;
	std::string fileName;
	ssMeasurement devSS;
	distMeasurement devDist;

	if (NULL != pNacho->pDataEx && pNacho->pDataEx->alive ())
	{
		std::cout << "You can't do that while NachoNet is running!\n";
	}
	else
	{
		Admin::test ();

		do
		{
			std::cout << "Please enter a valid input file name: ";
			std::cin >> fileName;

			testFile.open (fileName.c_str());
		} while (!testFile);

		if(NULL != pNacho->pDistMeasure)
		{
			//input file is devID SS
			while(!testFile.eof ())
			{
				testFile >> devSS.devID >> devSS.ss;
				devDist = pNacho->pDistMeasure->measure (devSS);

				*pOut << devDist.devID << "  " << devDist.dist << "\n";
			}
		}

		testFile.close ();
	}

}


