/*******************************************************************************
File:				EZConfig.cpp
Author: 		Josh Siva
Date:				3/20/14
Project:		NachoNet
Purpose:		Implements the JSON format config file reader thing
						{
							"sectionName" : {"val1" : val1, "val1" : val2 },
							"section2Name" : {"val3" : val3}
						}
*******************************************************************************/

#include "../../include/util/EZConfig.h"

/*******************************************************************************
 *Constructor:	EZConfig
 *
 *Description:	Checks for the proper file extension then reads in the contents
 *							of the file which is then parsed into our JSON object
 *
 *Parameters:  	fileName - the name of the config file
 *
 *Returned:		 	None
 ******************************************************************************/
EZConfig::EZConfig (std::string fileName)
{
	const std::string FILE_EXT = ".json";
	std::ifstream configFile;
	std::string fileText;
	jsonParser parser;

	if(std::string::npos == fileName.find(FILE_EXT))
	{
		std::cout << "ERROR: incorrect file type for configuration file!\n";
	}
	else
	{
		this->fileName = fileName;

		configFile.open(fileName.c_str());

		if(!configFile)
		{
			std::cout << "ERROR: could not open configuration file!\n";
		}
		else
		{

			fileText.assign ((std::istreambuf_iterator<char>(configFile)),
												std::istreambuf_iterator<char>());

			parser.init (fileText);

			myJSON = parser.getObject ();

			configFile.close ();
		}
	}
}

/*******************************************************************************
 *Destroyer!:		EZConfig
 *
 *Description:	Saves off the JSON data to the file
 *
 *Parameters:  	None
 *
 *Returned:			None
 ******************************************************************************/
EZConfig::~EZConfig ()
{
	save ();
	myJSON.clear ();
}

/*******************************************************************************
 *Method:				write
 *
 *Description:	Write a value to a specified key in a specified section
 *
 *Parameters:  	section - the section in which our key resides
 *							key			- the key we want to update
 *							data		- the new data
 *
 *Returned:			None
 ******************************************************************************/
void EZConfig::write (std::string section, std::string key, jsonData data)
{
	jsonData sectionData;

	if (sectionExists (section))
	{
		sectionData = myJSON.getData (section);
		sectionData.type = jsonParser::OBJ_TYPE;
		sectionData.value.pObject->setValue (key, data);

		myJSON.setValue (section, sectionData);
	}
}

/*******************************************************************************
 *Method:				save
 *
 *Description:	Saves the JSON data to the file. If the flag is true then we
 *							will write the data to the file in a way that is easy to read.
 *
 *Parameters:  	readable - true if we want the file to be readable, false
 *												 otherwise
 *
 *Returned:			None
 ******************************************************************************/
void EZConfig::save (bool readable)
{
	std::ofstream configFile;
	std::string rawJSON;

	configFile.open(fileName.c_str(), std::ofstream::trunc);

	if (!readable)
	{
		configFile << myJSON.writeJSON ("");
	}
	else
	{
		rawJSON = myJSON.writeJSON ("");

		configFile << rawJSON[0] << "\n";

		for (unsigned int i = 1; i < rawJSON.length() - 1; i++)
		{
			configFile << "\t"; // beginning of a section

			while ('}' != rawJSON[i])
			{
				configFile << rawJSON[i];

				if (':' == rawJSON[i + 1])
				{
					configFile << " ";
				}
				else if (':' == rawJSON[i] || ',' == rawJSON[i])
				{
					configFile << " ";
				}

				i++;
			}

			configFile << rawJSON[i++];
			configFile << rawJSON[i] << "\n"; //end of a section
		}

		configFile << "\n" << rawJSON[rawJSON.length () - 1];
	}

	configFile.close ();
}

/*******************************************************************************
 *Method:				getSection
 *
 *Description:	Get a section as a JSON object
 *
 *Parameters:  	section - the key that identifies the section
 *
 *Returned:			JSON - the section data
 ******************************************************************************/
JSON * EZConfig::getSection (std::string section)
{
	jsonData sectionData;

	sectionData = myJSON.getData (section);

	if (NULL == sectionData.value.pObject)
	{
		return NULL;
	}
	else
	{
		return sectionData.value.pObject;
	}
}

/*******************************************************************************
 *Method:				getAll
 *
 *Description:	Get all of the JSON data
 *
 *Parameters:  	None
 *
 *Returned:			JSON - all of the config data
 ******************************************************************************/
JSON EZConfig::getAll ()
{
	return myJSON;
}

/*******************************************************************************
 *Method:				getFileName
 *
 *Description:	Returns the name of the config file this object is using.
 *
 *Parameters:  	None
 *
 *Returned:			string - the file name
 ******************************************************************************/
std::string EZConfig::getFileName ()
{
	return fileName;
}

/*******************************************************************************
 *Method:				clearSection
 *
 *Description:	Clear all key-vals from a section (does not remove the section)
 *
 *Parameters:  	section - the key that identifies the section
 *
 *Returned:			None
 ******************************************************************************/
void EZConfig::clearSection (std::string section)
{
	jsonData blankData;

	blankData.type = jsonParser::OBJ_TYPE;

	myJSON.setValue (section, blankData);
}

/*******************************************************************************
 *Method:				sectionExists
 *
 *Description:	Checks to see if the given section already exists
 *
 *Parameters:  	section - the key that identifies the section
 *
 *Returned:			bool - true if the section exists, false otherwise
 ******************************************************************************/
bool EZConfig::sectionExists (std::string section)
{
	bool returnVal = true;

	if (NULL == getSection (section))
	{
		returnVal = false;
	}

	return returnVal;
}


