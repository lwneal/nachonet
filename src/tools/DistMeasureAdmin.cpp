/*******************************************************************************
File:				DistMeasureAdmin.cpp
Author: 		Josh Siva
Date:				3/18/14
Project:		NachoNet
Purpose:		Implements the admin tool that manages the Distance Measurement
						object
*******************************************************************************/

#include "../../include/tools/DistMeasureAdmin.h"


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
	char choice;

	if (pNacho->pDataEx->alive ())
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
			std::cin >> choice;
		} while (PATH_LOSS != (int)choice && FREE_SPACE_PATH_LOSS != (int)choice
						 && LOG_NORMAL_SHADOW != (int)choice);

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

	if (pNacho->pDataEx->alive ())
	{
		std::cout << "You can't do that while NachoNet is running!\n";
	}
	else
	{

		if(NULL != pNacho->pDistMeasure)
		{

			name = pNacho->pDistMeasure->getName();

			keyVal = distConfig.read(name);

			iter = keyVal.begin();

			std::cout << "Enter a number to update the value or 0 to leave it alone\n";
			std::cout << name << std::endl;

			for (auto & entry : keyVal)
			{
				std::cout << (*iter).first << " (" << (*iter).second << "): ";
				std::cin >> input;

				if(0 != input.compare("0"))
				{
					update = std::make_pair((*iter).first, input);

					updates.push_back(update);
				}
			}

			distConfig.write(name, updates);
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

	if (pNacho->pDataEx->alive ())
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

				os << devDist.devID << "  " << devDist.dist << "\n";
			}
		}

		testFile.close ();
	}

}


