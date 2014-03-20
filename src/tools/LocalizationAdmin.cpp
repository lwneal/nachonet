/*******************************************************************************
File:
Author: 		Josh Siva
Date:
Class:			CS460
Assignment:
Purpose:
*******************************************************************************/

#include "../../include/tools/LocalizationAdmin.h"

LocalizationAdmin::LocalizationAdmin (NachoNet *pNacho) : Admin (pNacho)
{
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

			devLoc = pNacho->pLocalization->localize (ref1, ref2, ref3);

			*pOut << devLoc.theID.strID << "  (" << devLoc.x << ", " << devLoc.y
				 << ")\n";
		}

		testFile.close();
	}
}


