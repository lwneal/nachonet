/*******************************************************************************
File:
Author: 		Josh Siva
Date:
Class:			CS460
Assignment:
Purpose:
*******************************************************************************/

#include "../../include/tools/LocalizationAdmin.h"

/*******************************************************************************
 * Constructor:	LocalizationAdmin
 *
 * Description: Sets up the local copy of the Localization object.
 *
 * Parameters:	pLocalization - the object we want to manipulate
 *
 * Returned:		None
 ******************************************************************************/
LocalizationAdmin::LocalizationAdmin (localization *pLocalization)
{
	this->pLocalization = pLocalization;
}

/*******************************************************************************
 * Method:			test
 *
 * Description: Calls the parent method, gets an input file from the user and
 * 							then calculates locations based on the data from the input file.
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void LocalizationAdmin::test ()
{
	std::ifstream testFile;
	std::string fileName;
	location devLoc;
	refMeasurement ref1, ref2, ref3;

	Admin::test ();

	do
	{
		std::cout << "Please enter a valid input file name: ";
		std::cin >> fileName;

		testFile.open (fileName.c_str());
	} while (!testFile);

	/*input file is formatted like this:
		nodeID x y
		nodeID x y
		nodeID x y
		devID d_1 d_2 d_3
		devID d_1 d_2 d_3

	where d_1 corresponds to the first node's measurement*/
	testFile >> ref1.nodeLocation.theID.intID >> ref1.nodeLocation.x
				 >> ref1.nodeLocation.y;
	testFile >> ref2.nodeLocation.theID.intID >> ref2.nodeLocation.x
				 >> ref2.nodeLocation.y;
	testFile >> ref3.nodeLocation.theID.intID >> ref3.nodeLocation.x
				 >> ref3.nodeLocation.y;

	while(!testFile.eof())
	{
		testFile >> ref1.devDist.devID >> ref1.devDist.dist
					 >> ref2.devDist.dist >> ref3.devDist.dist;

		devLoc = pLocalization->localize(ref1, ref2, ref3);

		os << devLoc.theID.strID << "  (" << devLoc.x << ", " << devLoc.y
			 << ")\n";
	}

	testFile.close();
}


