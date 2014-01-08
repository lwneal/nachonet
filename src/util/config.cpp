/*******************************************************************************
File:				config.cpp
Author: 		Josh Siva
Date:				11/24/13
Project:		NachoNet
Purpose:		Implements the functionality of the config object
*******************************************************************************/

#include "../../include/util/config.h"
#include <cstdio>

/******************************************************************************
 *Constructor: Config
 *
 *Description: Initializes the object by opening the file specified. If no file
 *						 is specified then the file 'default.conf' will be used. Since it
 *						 is easier to read to and write from a vector, the file is read
 *						 into vector which acts as a buffer for reads and writes.
 *
 *Parameters:  fileName - the name of the config file, default.conf by default
 *
 *Returned:		 None
 *****************************************************************************/
Config::Config(std::string fileName)
{
	const std::string FILE_EXT = ".conf";
	std::ifstream configFile;
	std::string line;

	if(std::string::npos == fileName.find(FILE_EXT))
	{
		corruptObject = true;
	}
	else
	{
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

			//Check to see if first line is empty
			if("" == fileText[0])
			{
				fileText.erase(fileText.begin());
			}

			sectionStart = -1;

			configFile.close();
		}
	}


}

/******************************************************************************
 *Destructor:		~Config
 *
 *Description:	Saves the configuration out to the file before being destroyed
 *
 *Parameters:		None
 *
 *Returned:			None
 *****************************************************************************/
Config::~Config()
{
	save();
}

/******************************************************************************
 *Method:				save
 *
 *Description:  Writes the contents of the file buffer out to the file so that
 *							the data is safer
 *
 *Parameters:		None
 *
 *Returned:			int - NO_ERROR, BAD_FILE
 *****************************************************************************/
int Config::save()
{
	std::ifstream fileTest;
	std::ofstream configFile;
	int returnVal = NO_ERROR;
	std::vector<std::string>::iterator iter = fileText.begin();

	fileTest.open(fileName.c_str());

	if(!fileTest)
	{
		returnVal = BAD_FILE;
	}
	else
	{
		fileTest.close();
		configFile.open(fileName.c_str(), std::ofstream::trunc);

		while(fileText.end() != iter)
		{
			configFile << *iter;
			++iter;

			if(fileText.end() != iter)
			{
				configFile << std::endl;
			}

		}

		configFile.close();
	}

	return returnVal;
}

/******************************************************************************
 *Method:				fillMap
 *
 *Description:	Locates the specified section in the config file (buffer) and
 *							fills in the map for that section
 *
 *Parameters:		section - the header of the section whose values need to fill
 *												the map
 *
 *Returned:			int - NO_ERROR, SECTION_NOT_FOUND, BAD_OBJECT
 *****************************************************************************/
int Config::fillMap(std::string section)
{
	//char tmp[101], tmp2[101];
	unsigned position;
	int returnVal = NO_ERROR, lineNum = 0;
	std::string line, currentSection, key, value;
	std::vector<std::string>::iterator iter = fileText.begin();
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

					for(unsigned int i = 1; i < line.length() - 1; i++)
					{
						currentSection += line[i];
					}

					if(0 == currentSection.compare(section))
					{
						sectionFound = true;
						sectionStart = lineNum;
					}

					currentSection.clear();

					break;

				case '#': //Do nothing because this is an in line comment
					break;

				default:
					if(sectionFound && '\n' != line[0])
					{
						position = line.find("=");
						key = line.substr(0, position);
						value = line.substr(position + 1, line.length() - (position + 1));

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

/******************************************************************************
 *Method:				addSection
 *
 *Description:	Add a section to the configuration file. fillMap is used to
 *							check if the section exists in the file already. If this is
 *							the case, we return an appropriate error code
 *
 *Parameters:		section - the section header to be added to the file
 *
 *Returned:			int - NO_ERROR, SECTION_EXISTS
 *****************************************************************************/
int Config::addSection(std::string section)
{
	int returnVal = NO_ERROR;

	returnVal = fillMap(section);

	if(SECTION_NOT_FOUND == returnVal)
	{
		fileText.push_back('[' + section + ']');
	}
	else
	{
		returnVal = SECTION_EXISTS;
	}

	return returnVal;
}

/******************************************************************************
 *Method:				getFileName
 *
 *Description:  Get the name of the file associated with this config object
 *
 *Parameters:		None
 *
 *Returned:			std::string - the file name
 *****************************************************************************/
std::string Config::getFileName() const
{
	return fileName;
}

/******************************************************************************
 *Method:				isCorrupt
 *
 *Description:	Check to see whether the object is corrupt and should be
 *							reinitialized/fixed
 *
 *Parameters:		None
 *
 *Returned:			bool
 *****************************************************************************/
bool Config::isCorrupt() const
{
	return corruptObject;
}

/******************************************************************************
 *Method:				write
 *
 *Description:	Use a vector of key-value pairs to update/fill in the specified
 *							section of the file (buffer).
 *
 *Parameters:		section - the section to which we are writing
 *							keyVals - the vector of key-value pairs
 *
 *Returned:			int - NO_ERROR, SECTION_NOT_FOUND, BAD_OBJECT
 *****************************************************************************/
int Config::write(std::string section,
		std::vector<std::pair<std::string, std::string>> keyVals)
{
	int returnVal = NO_ERROR;
	std::vector<std::pair<std::string, std::string>>::iterator iter =
																							keyVals.begin(), newEntryIter;
	std::vector<std::pair<std::string, std::string>> newEntryBuffer;
	std::map<std::string, std::pair<int, std::string>>::iterator mapIter;
	std::string line;
	std::pair<std::string, std::string> entry;

	if(!corruptObject)
	{
		returnVal = fillMap(section);

		if(SECTION_NOT_FOUND != returnVal)
		{
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
			if(0 != newEntryBuffer.size())
			{
				newEntryIter = newEntryBuffer.begin();

				while(newEntryBuffer.end() != newEntryIter)
				{
					line = (*newEntryIter).first + '=' + (*newEntryIter).second;

					fileText.insert(fileText.begin() + sectionStart + 1, line);

					++newEntryIter;
				}
			}
		}
	}
	else
	{
		returnVal = BAD_OBJECT;
	}


	return returnVal;
}

/******************************************************************************
 *Method:				read
 *
 *Description:	Update the section map and then return the key value pairs in
 *							a map.
 *
 *Parameters:   section - the section from which to read
 *
 *Returned:			std::map<std::string, float>
 *****************************************************************************/
std::map<std::string, std::string> Config::read(std::string section)
{
	std::map<std::string, std::string> keyVals;
	std::map<std::string, std::pair<int, std::string>>::iterator iter =
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




