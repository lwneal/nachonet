/*******************************************************************************
File:				Map.h
Author: 		Josh Siva
Date:				3/25/14
Project:		NachoNet
Purpose:		Defines the abstract interface to the Map object which is
						responsible for getting a map of the location in which NachoNet is
						deployed from the user. This map will be used to show where the
						devices are located in the room. How the devices and nodes are
						displayed on the map is completely up to the developer.
*******************************************************************************/

#pragma once
#include <string>
#include <iostream>
#include <fstream>

class Map
{
	public:
		Map ();
		virtual ~Map ();
		bool getUserInput ();

		void setMapFile (std::string fileName);
		std::string getMapFileName ();

		void setMaxDimensions (float x, float y);
		float getMaxX ();
		float getMaxY ();

		virtual void save () = 0;
		virtual void load () = 0;

		static const std::string FILE_EXT_1;
		static const std::string FILE_EXT_2;
	private:
		std::string fileName;
		float maxX;
		float maxY;
};
