/*******************************************************************************
File:				Admin.cpp
Author: 		Josh Siva
Date:				3/18/14
Project:		NachoNet
Purpose:		Implements the basic functionality required by our admin tool
*******************************************************************************/

#include "../../include/tools/Admin.h"

Admin::Admin ()
{
	os = 0;
}

Admin::~Admin ()
{
}

void Admin::options ()
{
	std::ofstream outFile;
	std::string outFileName;
	char choice;

	do
	{
		std::cout << "Would you like to capture the output in a file? (y/n): ";
		std::cin >> choice;
	} while(YES != choice && NO != choice);

	if(YES == choice)
	{
		std::cout << "Please enter an output file name: ";
		std::cin >> outFileName;

		outFile.open(outFileName.c_str());

		if (!outFile.fail())
		{
			os = outFile;
		}
		else
		{
			std::cout << "\nFailed to open file!\n";
			os = std::cout;
		}
	}
	else
	{
		os = std::cout;
	}
}

void Admin::test ()
{
	std::cout << "\nNo testing behavior has been defined.\n";
}

void Admin::configure ()
{
	std::cout << "\nNo configuration behavior has been defined.\n";
}



