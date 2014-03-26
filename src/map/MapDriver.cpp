/*******************************************************************************
File:				MapDriver.cpp
Author: 		Josh Siva
Date:				3/25/14
Project:		NachoNet
Purpose:		Tests the Map/MapOnTheCouch object(s)
*******************************************************************************/

#include "../../include/map/MapOnTheCouch.h"

int main ()
{
	Map *pMap = new MapOnTheCouch;
	std::ifstream inFile ("web/cs_lab.jpg", std::ios::binary);

	if (!inFile)
	{
		std::cout << "shit went down\n";
	}
	else
	{
		inFile.close ();
	}

	pMap->load ();
	pMap->getUserInput ();
	pMap->save ();
	delete pMap;

	pMap = new MapOnTheCouch;

	pMap->load ();
	pMap->getUserInput ();
}


