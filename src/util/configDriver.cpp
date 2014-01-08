/*******************************************************************************
File:				configDriver.cpp
Author: 		Josh Siva
Date:				11/29/13
Project:		NachoNet
Purpose:		Tests the stability and functionality of the Config object though
						a series of unit tests
*******************************************************************************/

#include "../../include/util/config.h"
#include <iostream>
#include <fstream>
#include <string>

void failure(const char * message)
{
	std::cout << "FAIL: " << message << std::endl;
}


int main()
{
	Config *pConfig = new Config;
	std::string data;
	std::vector<std::pair<std::string, std::string>> values;
	std::map<std::string, std::string> keyVals;

	if(0 != (pConfig->getFileName()).compare("default.conf"))
	{
		failure("default file name incorrect");
	}

	//since the file doesn't actually exist object should be bad
	if(!(pConfig->isCorrupt()))
	{
		failure("config object is not corrupt with non-existant file");
	}

	if(Config::BAD_FILE != pConfig->save())
	{
		failure("tried to save with bad object/non-existant file");
	}

	data = "section";

	if(Config::BAD_OBJECT != pConfig->fillMap(data))
	{
		failure("tried to fill map with bad object/non-existant file");
	}

	if(Config::BAD_OBJECT != pConfig->write(data, values))
	{
		failure("tried to write with bad object/non-existant file");
	}

	delete pConfig;

	pConfig = new Config("testfiles/new.conf");

	if(0 != (pConfig->getFileName()).compare("testfiles/new.conf"))
	{
		failure("non-default file name incorrect");
	}

	if(Config::SECTION_NOT_FOUND != pConfig->fillMap(data))
	{
		failure("fillMap did not return SECTION_NOT_FOUND with non-existant section");
	}

	if(Config::SECTION_NOT_FOUND != pConfig->write(data, values))
	{
		failure("write did not return SECTION_NOT_FOUND with non-existant section");
	}

	pConfig->addSection(data);

	if(Config::NO_ERROR != pConfig->fillMap(data))
	{
		failure("fillMap did not find newly added section");
	}

	if(Config::NO_ERROR != pConfig->write(data, values))
	{
		failure("write did not find newly added section");
	}

	delete pConfig;

	pConfig = new Config("test.txt");

	if(!(pConfig->isCorrupt()))
	{
		failure("did not catch wrong file extension");
	}

	delete pConfig;

	pConfig = new Config("testfiles/comments.conf");

	if(Config::NO_ERROR != pConfig->fillMap(data))
	{
		failure("fillMap did not fill correctly with comments.conf");
	}

	keyVals = pConfig->read(data);

	float num = 1.1; //because comparing against 1.1 just isn't good enough
	if(0 != keyVals["a"].compare("a") || 1 != std::stoi(keyVals["b"])
		|| num != std::stof(keyVals["c"]) || 0 != (keyVals["d"]).compare("blah")
		|| 2 != std::stoi(keyVals["f"]))
	{
		failure("read failed to return the correct values from comments.conf");

		if(0 != keyVals["a"].compare("a"))
		{
			std::cout << "a" << "=" << keyVals["a"] << std::endl;
		}

		if(1 != std::stoi(keyVals["b"]))
		{
			std::cout << "b" << "=" << keyVals["b"] << std::endl;
		}

		if(num != std::stof(keyVals["c"]))
		{
			std::cout << "c" << "=" << keyVals["c"] << std::endl;
		}

		if(0 != (keyVals["d"]).compare("blah"))
		{
			std::cout << "d" << "=" << keyVals["d"] << std::endl;
		}

		if(2 != std::stoi(keyVals["f"]))
		{
			std::cout << "f" << "=" << keyVals["f"] << std::endl;
		}
	}

	delete pConfig;

	//Test writing to and reading from the file.

	pConfig = new Config("testfiles/writeTest.conf");

	pConfig->addSection("SectionOne");

	if(Config::NO_ERROR != pConfig->fillMap("SectionOne"))
	{
		failure ("Did not find newly added section");
	}

	values.clear();

	values.push_back(std::make_pair("a", "1"));
	values.push_back(std::make_pair("b", "2"));
	values.push_back(std::make_pair("c", "3"));

	if(Config::NO_ERROR != pConfig->write("SectionOne", values))
	{
		failure ("Failed to write data to section");
	}

	delete pConfig;

	pConfig = new Config("testfiles/writeTest.conf");

	if(Config::NO_ERROR != pConfig->fillMap("SectionOne"))
	{
		failure ("Did not find section");
	}

	keyVals = pConfig->read("SectionOne");

	if(1 != std::stoi(keyVals["a"]) || 2 != std::stoi(keyVals["b"])
		|| 3 != std::stof(keyVals["c"]))
	{
		failure("read failed to return the correct values from writeTest.conf");

		if(1 != std::stoi(keyVals["a"]))
		{
			std::cout << "a" << "=" << keyVals["a"] << std::endl;
		}

		if(2 != std::stoi(keyVals["b"]))
		{
			std::cout << "b" << "=" << keyVals["b"] << std::endl;
		}

		if(3 != std::stoi(keyVals["c"]))
		{
			std::cout << "c" << "=" << keyVals["c"] << std::endl;
		}
	}

	pConfig->addSection("SectionTwo");

	if(Config::NO_ERROR != pConfig->fillMap("SectionTwo"))
	{
		failure ("Did not find newly added section");
	}

	values.clear();

	values.push_back(std::make_pair("d", "4"));
	values.push_back(std::make_pair("e", "5"));
	values.push_back(std::make_pair("f", "6"));

	if(Config::NO_ERROR != pConfig->write("SectionTwo", values))
	{
		failure ("Failed to write data to section");
	}

	delete pConfig;

	pConfig = new Config("testfiles/writeTest.conf");

	if(Config::NO_ERROR != pConfig->fillMap("SectionTwo"))
	{
		failure ("Did not find section");
	}

	keyVals = pConfig->read("SectionTwo");

	if(4 != std::stoi(keyVals["d"]) || 5 != std::stoi(keyVals["e"])
		|| 6 != std::stof(keyVals["f"]))
	{
		failure("read failed to return the correct values from writeTest.conf");

		if(4 != std::stoi(keyVals["d"]))
		{
			std::cout << "d" << "=" << keyVals["d"] << std::endl;
		}

		if(5 != std::stoi(keyVals["e"]))
		{
			std::cout << "e" << "=" << keyVals["e"] << std::endl;
		}

		if(6 != std::stoi(keyVals["f"]))
		{
			std::cout << "f" << "=" << keyVals["f"] << std::endl;
		}
	}

	if(Config::NO_ERROR != pConfig->fillMap("SectionOne"))
	{
		failure ("Did not find section");
	}

	keyVals = pConfig->read("SectionOne");

	if(1 != std::stoi(keyVals["a"]) || 2 != std::stoi(keyVals["b"])
		|| 3 != std::stof(keyVals["c"]))
	{
		failure("read failed to return the correct values from writeTest.conf");

		if(1 != std::stoi(keyVals["a"]))
		{
			std::cout << "a" << "=" << keyVals["a"] << std::endl;
		}

		if(2 != std::stoi(keyVals["b"]))
		{
			std::cout << "b" << "=" << keyVals["b"] << std::endl;
		}

		if(3 != std::stoi(keyVals["c"]))
		{
			std::cout << "c" << "=" << keyVals["c"] << std::endl;
		}
	}

	delete pConfig;

	return 0;
}


