/*******************************************************************************
File:				EZConfig.h
Author: 		Josh Siva
Date:				3/20/14
Project:		NachoNet
Purpose:		Defines the interface to a Config file that uses JSON format to
						store data.
						{
							"sectionName" : {"val1" : val1, "val1" : val2 },
							"section2Name" : {"val3" : val3}
						}
*******************************************************************************/

#pragma once
#include "json.h"
#include "jsonParser.h"
#include <fstream>
#include <streambuf>

class EZConfig
{
	public:
		EZConfig (std::string fileName);
		~EZConfig ();
		void write (std::string section, std::string key, jsonData data);
		void save (bool readable = false);
		JSON getSection (std::string section);
		JSON getAll ();
		std::string getFileName ();
		void clearSection (std::string section);
		bool sectionExists (std::string section);

	private:
		JSON myJSON;
		std::string fileName;

};
