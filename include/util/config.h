/*******************************************************************************
File:				config.h
Author: 		Josh Siva
Date:				11/24/13
Project:		NachoNet
Purpose:		Implements the interface to the config object which will handle all
						reading from and writing to a file that is filled with key-value
						pairs differentiated by section headers.
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
		std::vector<std::pair<std::string, std::string>> read(std::string section);
	private:
		bool corruptObject;
		std::fstream configFile;
		std::map<std::string, std::string> configMap;
};
