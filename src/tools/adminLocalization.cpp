/*******************************************************************************
File:				adminLocalization.cpp
Author: 		Josh Siva
Date:				12/14/13
Project:		NachoNet
Purpose:		Implements the behavior of the adminLocalization module
*******************************************************************************/

#include "../../include/tools/adminLocalization.h"

void adminLocalization::setDebug(bool debug)
{
	this->debug = debug;
}

void adminLocalization::test(std::string inputFile, std::ostream & os)
{
	std::ifstream inFile;
	location devLoc;
	refMeasurement ref1, ref2, ref3;
	pLoc = new localization(debug);

	inFile.open(inputFile.c_str());

	if(!inFile)
	{
		std::cout << "Could not open " << inputFile << std::endl;
	}
	else
	{

		/*input file is formatted like this:
			nodeID x y
			nodeID x y
			nodeID x y
			devID d_1 d_2 d_3
			devID d_1 d_2 d_3

		where d_1 corresponds to the first node's measurement*/
		inFile >> ref1.nodeLocation.theID.intID >> ref1.nodeLocation.x
					 >> ref1.nodeLocation.y;
		inFile >> ref2.nodeLocation.theID.intID >> ref2.nodeLocation.x
					 >> ref2.nodeLocation.y;
		inFile >> ref3.nodeLocation.theID.intID >> ref3.nodeLocation.x
					 >> ref3.nodeLocation.y;

		while(!inFile.eof())
		{
			inFile >> ref1.devDist.devID >> ref1.devDist.dist
						 >> ref2.devDist.dist >> ref3.devDist.dist;

			devLoc = pLoc->localize(ref1, ref2, ref3);

			os << devLoc.theID.strID << "  (" << devLoc.x << ", " << devLoc.y
				 << ")\n";
		}

	}

	delete pLoc;

	inFile.close();
}




