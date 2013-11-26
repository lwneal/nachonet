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
	std::ifstream configFile;
	std::string line;

	this->fileName = fileName;

	configFile.open(fileName.c_str());

	if(!configFile)
	{
		corruptObject = true;
	}
	else
	{
		corruptObject = false;

		while(!configFile.eof())
		{
			std::getline(configFile, line);
			fileText.push_back(line);
		}

		sectionStart = -1;

		configFile.close();
	}
}

Config::~Config()
{
	save();
}

int Config::save()
{
	std::ofstream configFile;
	int returnVal = NO_ERROR;
	std::vector<std::string>::iterator iter = fileText.begin();

	configFile.open(fileName.c_str());

	if(!configFile)
	{
		returnVal = BAD_FILE;
	}
	else
	{
		while(fileText.end() != iter)
		{
			configFile << *iter;
			++iter;
		}

		configFile.close();
	}

	return returnVal;
}

int Config::fillMap(std::string section)
{
	char tmp[101];
	int returnVal = NO_ERROR, lineNum = 0;
	std::string line, currentSection, key;
	std::vector<std::string>::iterator iter = fileText.begin();
	float value;
	bool sectionFound = false;

	if(!corruptObject)
	{
		//search for the correct section and fill map
		while(fileText.end() != iter)
		{
			line = *iter;

			switch(line[0])
			{
				case '[':

					for(int i = 1; i < line.length(); i++)
					{
						currentSection += line[i];
					}

					if(0 == currentSection.compare(section))
					{
						sectionFound = true;
						sectionStart = lineNum;
					}

					break;

				case '#': //Do nothing because this is an in line comment
					break;

				default:
					if(sectionFound)
					{
						std::sscanf(line.c_str(), "%100s=%f", &tmp, &value);

						key = tmp;

						sectionMap[key] = std::make_pair(lineNum, value);
					}
					break;
			}

			++iter;
			lineNum++;
		}

		if(!sectionFound)
		{
			returnVal = SECTION_NOT_FOUND;
		}
	}
	else
	{
		returnVal = BAD_OBJECT;
	}

	return returnVal;
}

void Config::addSection(std::string section)
{
	if(SECTION_NOT_FOUND != fillMap(section))
	{
		fileText.push_back('[' + section + ']');
	}
}

std::string Config::getFileName() const
{
	return fileName;
}

bool Config::isCorrupt() const
{
	return corruptObject;
}

int Config::write(std::string section,
		std::vector<std::pair<std::string, float>> keyVals)
{
	int returnVal = NO_ERROR, lineNumber;
	std::vector<std::pair<std::string, float>>::iterator iter = keyVals.begin(),
																											 newEntryIter;
	std::vector<std::pair<std::string, float>> newEntryBuffer;
	std::map<std::string, std::pair<int, float>>::iterator mapIter;
	std::string line;
	std::pair<int, float> entry;

	if(!corruptObject)
	{
		fillMap(section);

		//update sectionMap of values
		while(keyVals.end() != iter)
		{
			mapIter = sectionMap.find((*iter).first);

			if(sectionMap.end() != mapIter)
			{
				sectionMap[(*iter).first] = std::make_pair((*mapIter).second.first,
																											(*iter).second);
			}
			else
			{
				entry = std::make_pair((*iter).first, (*iter).second);

				newEntryBuffer.push_back(entry);
			}

			++iter;
		}

		//write to file buffer
		mapIter = sectionMap.begin();

		while(sectionMap.end() != mapIter)
		{
			line = fileText[(*mapIter).second.first];
			line.clear();

			line = (*mapIter).first + '=' + (*mapIter).second.second;
			fileText[(*mapIter).second.first] = line;

			++mapIter;
		}

		//now write the new stuff if there is any
		if(0 != newEntryBuffer.size() && -1 != sectionStart)
		{
			newEntryIter = newEntryBuffer.begin();

			while(newEntryBuffer.end() != newEntryIter)
			{
				line = (*newEntryIter).first + '=' + (*newEntryIter).second;

				fileText.insert(fileText.begin() + sectionStart + 1, line);

				++newEntryIter;
			}
		}
		else if(0 != newEntryBuffer.size() && -1 == sectionStart)
		{
			returnVal = SECTION_NOT_FOUND;
		}

	}
	else
	{
		returnVal = BAD_OBJECT;
	}


	return returnVal;
}


std::map<std::string, float> Config::read(std::string section)
{
	std::map<std::string, float> keyVals;
	std::map<std::string, std::pair<int, float>>::iterator iter =
																												sectionMap.begin();

	if(SECTION_NOT_FOUND != fillMap(section))
	{
		while(sectionMap.end() != iter)
		{
			keyVals[(*iter).first] = (*iter).second.second;

			++iter;
		}
	}

	return keyVals;
}




