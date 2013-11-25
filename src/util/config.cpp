/*******************************************************************************
File:				config.cpp
Author: 		Josh Siva
Date:				11/24/13
Project:		NachoNet
Purpose:		Implements the functionality of the config object
*******************************************************************************/

#include "../../include/util/config.h"
#include <cstdio>

Config::Config(std::string fileName)
{
	configFile.open(fileName.c_str());

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
	char tmp[101];
	int returnVal = 0;
	std::string line, currentSection, key;
	float value;
	bool sectionFound = false, readyToWrite = false;

	if(!corruptObject)
	{
		configFile.clear();
		configFile.seekg(0, std::ios::beg);

		//search for the correct section and fill map
		while(!readyToWrite)
		{
			std::getline(configFile, line);

			if(!configFile.eof())
			{
				switch(line[0])
				{
					case '[':

						if(!sectionFound)
						{
							for(int i = 1; i < line.length(); i++)
							{
								currentSection.push_back(line[i]);
							}

							if(0 == currentSection.compare(section))
							{
								sectionFound = true;
							}
						}
						else
						{
							readyToWrite = true;
						}

						break;

					case '#': //Do nothing because this is an in line comment
						break;

					default:
						if(sectionFound)
						{
							std::sscanf(line.c_str(), "%100s=%f", &tmp, &value);

							key = tmp;

							configMap[key] = value;
						}
						break;
				}
			}
			else
			{
				readyToWrite = true;
			}
		}

		//the map is filled, so now we need to write out the data from the vector

	}

	return returnVal;
}


std::vector<std::pair<std::string, std::string>>
			Config::read(std::string section)
{
	std::vector<std::pair<std::string, std::string>> keyVals;


	return keyVals;
}




