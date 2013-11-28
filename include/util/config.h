/*******************************************************************************
File:				config.h
Author: 		Josh Siva
Date:				11/24/13
Project:		NachoNet
Purpose:		Implements the interface to the config object which will handle all
						reading from and writing to a file that is filled with key-value
						pairs differentiated by section headers. Comments in this file are
					  on their own line and preceded by #
*******************************************************************************/

#pragma once
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

class Config
{
	public:
		Config(std::string fileName = "default.conf");
		~Config();
		int write(std::string section,
				std::vector<std::pair<std::string, std::string>> keyVals);
		std::map<std::string, std::string> read(std::string section);

		int save();

		void addSection(std::string section);

		std::string getFileName() const;
		bool isCorrupt() const;

		typedef enum Errors
		{
			BAD_FILE = -3,
			SECTION_NOT_FOUND,
			BAD_OBJECT,
			NO_ERROR
		}Errors ;

	private:
		bool corruptObject;
		std::string fileName;
		std::vector<std::string> fileText;
		std::map<std::string, std::pair<int, std::string>> sectionMap;
		int sectionStart;

		int fillMap(std::string section);
};
