/*******************************************************************************
File:				config.cpp
Author: 		Josh Siva
Date:
Project:		NachoNet
Purpose:
*******************************************************************************/

#include "../../include/util/config.h"

Config::Config(std::string fileName)
{
	configFile.open(fileName);

	if(!configFile)
	{
		corruptObject = true;
	}
	else
	{
		corruptObject = false;
	}
}

Config::~Config()
{
	if(!corruptObject)
	{
		configFile.close();
	}

}

int Config::write(std::string section,
		std::vector<std::pair<std::string, std::string>> keyVals)
{
	int returnVal = 0;


	return returnVal;
}


std::vector<std::pair<std::string, std::string>>
			Config::read(std::string section)
{
	std::vector<std::pair<std::string, std::string>> keyVals;


	return keyVals;
}




