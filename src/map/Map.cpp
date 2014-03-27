/*******************************************************************************
File:				Map.cpp
Author: 		Josh Siva
Date:				3/25/14
Project:		NachoNet
Purpose:		Implements data collection from the user and simple sets and gets
						for the Map object.
*******************************************************************************/

#include "../../include/map/Map.h"

const std::string Map::FILE_EXT_1 = ".jpeg";
const std::string Map::FILE_EXT_2 = ".jpg";

/*******************************************************************************
 * Constructor:	Map
 *
 * Description: Initializes the member variables
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
Map::Map ()
{
	fileName = "none";
	maxX = 0.0;
	maxY = 0.0;
}

/*******************************************************************************
 * Destroyer!:	~Map
 *
 * Description: Does nothing
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
Map::~Map ()
{
}

/*******************************************************************************
 * Method: 			getUserInput
 *
 * Description: Prompts the user for an image file and the max coordinates of
 * 							the map
 *
 * Parameters:	None
 *
 * Returned:		false if the file could not be opened, true otherwise
 ******************************************************************************/
bool Map::getUserInput ()
{
	bool returnVal = true;
	std::ifstream inFile;
	std::string fileName;
	float x, y;

	std::cout << "Proper map format: \n";
	std::cout << "(0,0)------>x\n";
	std::cout << "  |\n";
	std::cout << "  |\n";
	std::cout << "  |\n";
	std::cout << "  v\n";
	std::cout << "  y\n";
	std::cout << "Current map image file: " << getMapFileName () << "\n";
	std::cout << "Max x: " << getMaxX () << "   Max y: " << getMaxY () << "\n";

	do
	{
		std::cout << "New map image file (.jpeg or .jpg only!): ";
		std::cin >> fileName;
	} while (std::string::npos == fileName.find (FILE_EXT_1) &&
					 std::string::npos == fileName.find (FILE_EXT_2));

	inFile.open (fileName.c_str (), std::ios::binary);

	if (!inFile)
	{
		std::cout << "Unable to open file\n";
		returnVal = false;
	}
	else
	{
		inFile.close ();

		setMapFile (fileName);

		std::cout << "Please enter the room's max dimensions in meters\n";
		do
		{
			std::cout << "x: ";
			std::cin >> x;
		} while (0 >= x);

		do
		{
			std::cout << "y: ";
			std::cin >> y;
		} while (0 >= y);

		setMaxDimensions (x, y);
	}

	return returnVal;
}

/*******************************************************************************
 * Method:			setMapFile
 *
 * Description:	Sets the file name of the map image
 *
 * Parameters:	fileName - the name of the image file
 *
 * Returned:		None
 ******************************************************************************/
void Map::setMapFile (std::string fileName)
{
	this->fileName = fileName;
}

/*******************************************************************************
 * Method:			getMapFileName
 *
 * Description:	Get the name of the file containing the map image
 *
 * Parameters:	None
 *
 * Returned:		string - the file name
 ******************************************************************************/
std::string Map::getMapFileName ()
{
	return fileName;
}

/*******************************************************************************
 * Method:			setMaxDimensions
 *
 * Description: Set the max dimensions of the map (used for scaling)
 *
 * Parameters:	x - the maximum x dimension
 * 							y - the maximum y dimension
 *
 * Returned:		None
 ******************************************************************************/
void Map::setMaxDimensions (float x, float y)
{
	maxX = x;
	maxY = y;
}

/*******************************************************************************
 * Method:			getMaxX
 *
 * Description:	Get the maximum x dimension
 *
 * Parameters:	None
 *
 * Returned:		float
 ******************************************************************************/
float Map::getMaxX ()
{
	return maxX;
}

/*******************************************************************************
 * Method:			getMaxY
 *
 * Description:	Get the maximum y dimension
 *
 * Parameters:	None
 *
 * Returned:		float
 ******************************************************************************/
float Map::getMaxY ()
{
	return maxY;
}
