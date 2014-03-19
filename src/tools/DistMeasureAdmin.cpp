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
 * Constructor:	DistMeasureAdmin
 *
 * Description:	Sets up the config file and stores the object pointer
 *
 * Parameters:	hDistMeasure - a handle the the Distance Measurement object
 * 														 because we may want to change what the object
 * 														 points to
 *
 * Returned:		None
 ******************************************************************************/
DistMeasureAdmin::DistMeasureAdmin (distMeasure ** hDistMeasure)
: distConfig("dist.conf")
{
	pDistMeasure = *hDistMeasure;
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
	char choice;

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
			delete pDistMeasure;
			pDistMeasure = new fsPathLoss (false, &distConfig);
			break;

		case LOG_NORMAL_SHADOW:
			delete pDistMeasure;
			pDistMeasure = new logNormalShadow (false, &distConfig);
			break;

		default:
			delete pDistMeasure;
			pDistMeasure = new pathLoss (false, &distConfig);
			break;
	}

	std::cout << "Be sure to check the configuration for the current equation\n";
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

	if(NULL != pDistMeasure)
	{

		name = pDistMeasure->getName();

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

	Admin::test ();

	do
	{
		std::cout << "Please enter a valid input file name: ";
		std::cin >> fileName;

		testFile.open (fileName.c_str());
	} while (!testFile);

	if(NULL != pDistMeasure)
	{
		//input file is devID SS
		while(!testFile.eof ())
		{
			testFile >> devSS.devID >> devSS.ss;
			devDist = pDistMeasure->measure (devSS);

			os << devDist.devID << "  " << devDist.dist << "\n";
		}
	}

	testFile.close ();

}


