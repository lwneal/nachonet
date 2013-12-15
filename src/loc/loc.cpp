/*******************************************************************************
File:				loc.cpp
Author: 		Josh Siva
Date:				12/8/13
Project:		NachoNet
Purpose:		Implements the functionality of the localization class with the
						equation for trilateration
*******************************************************************************/

#include "../../include/loc/loc.h"

localization::localization(bool debug)
{
	this->debug = debug;
}

/*****************************************************************************
 * Method: 			localize
 *
 * Description: Implements the following trilateration formula:
 * 							x = (A * Y_32 + B * Y_13 + C * Y_21) /
 * 									2 * (x_1 * Y_32 + x_2 * Y_13 + x_3 * Y_21)
 *
 * 							y = (A * X_32 + B * X_13 + C * X_21) /
 * 									2 * (y_1 * X_32 + y_2 * X_13 + y_3 * X_21)
 *
 * 							A  = (x_1)^2 + (y_1)^2 - (d_1)^2
 * 							B  = (x_2)^2 + (y_2)^2 - (d_2)^2
 * 							C  = (x_3)^2 + (y_3)^2 - (d_3)^2
 *
 * 							[X|Y]_ab = ([x|y]_a - [x|y]_b)
 *
 * 							For the sake of clarity, we are going to ignore the coding
 * 							standards. We're also going to assume that _ is read as a
 * 							subscript.
 *
 * Parameters:	ref1 - the reference measurement from node 1
 * 							ref2 - the reference measurement from node 2
 * 							ref3 - the reference measurement from node 3
 *
 * Returned:		location - the location of the device
 */

location localization::localize(refMeasurement ref1, refMeasurement ref2,
																refMeasurement ref3)
{
	location returnLocation;
	float X_32, X_13, X_21, Y_32, Y_13, Y_21;
	float A = pow(ref1.nodeLocation.x, 2) + pow(ref1.nodeLocation.y, 2) -
						pow(ref1.devDist.dist, 2);
	float B = pow(ref2.nodeLocation.x, 2) + pow(ref2.nodeLocation.y, 2) -
						pow(ref2.devDist.dist, 2);
	float C = pow(ref3.nodeLocation.x, 2) + pow(ref3.nodeLocation.y, 2) -
						pow(ref3.devDist.dist, 2);

	X_32 = ref3.nodeLocation.x - ref2.nodeLocation.x;
	X_13 = ref1.nodeLocation.x - ref3.nodeLocation.x;
	X_21 = ref2.nodeLocation.x - ref1.nodeLocation.x;
	Y_32 = ref3.nodeLocation.y - ref2.nodeLocation.y;
	Y_13 = ref1.nodeLocation.y - ref3.nodeLocation.y;
	Y_21 = ref2.nodeLocation.y - ref1.nodeLocation.y;

	returnLocation.x = (A * Y_32 + B * Y_13 + C * Y_21) /
  		(2 * (ref1.nodeLocation.x * Y_32 + ref2.nodeLocation.x * Y_13 +
  				ref3.nodeLocation.x * Y_21));

  returnLocation.y = (A * X_32 + B * X_13 + C * X_21) /
 			(2 * (ref1.nodeLocation.y * X_32 + ref2.nodeLocation.y * X_13 +
 					ref3.nodeLocation.y * X_21));

  returnLocation.theID.strID = ref1.devDist.devID;

  if(debug)
  {
  	std::cout << "(X_1, Y_2):d_1  (X_2, Y_2):d_2  (X_3, Y_3):d_3  (X, Y)\n";
  	std::cout << "--------------  --------------  --------------  ------\n";
  	std::cout << std::setprecision(3) << "("
  						<< ref1.nodeLocation.x << ", " << ref1.nodeLocation.y
  						<< "):" << ref1.devDist.dist;
  	std::cout << "       " << "(" << ref2.nodeLocation.x << ", "
  						<< ref2.nodeLocation.y << "):" << ref2.devDist.dist;
  	std::cout << "       " << "(" << ref3.nodeLocation.x << ", "
  						<< ref3.nodeLocation.y << "):" << ref3.devDist.dist;
  	std::cout << "       (" << returnLocation.x << ", "
  						<< returnLocation.y << ")\n";
  }


	return returnLocation;
}




