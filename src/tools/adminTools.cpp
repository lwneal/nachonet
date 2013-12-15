/*******************************************************************************
File:				adminTools.cpp
Author: 		Josh Siva
Date:				10/21/13
Project:		NachoNet
Purpose:		Implements the methods of the adminTools object
*******************************************************************************/

#include "../../include/tools/adminTools.h"

void AdminTools::setDistEqn(int eqnType)
{
	distAdmin.setEqn(eqnType);
}

void AdminTools::testDist()
{
	std::ofstream outFile;
	std::string inFileName, outFileName;
	char choice;

	std::cout << "Please enter an input file: ";
	std::cin >> inFileName;

	do
	{
		std::cout << "Would you like to capture the output in a file? (y/n): ";
		std::cin >> choice;
	} while('y' != choice && 'n' != choice);

	if('y' == choice)
	{
		std::cout << "Please enter an output file name: ";
		std::cin >> outFileName;

		outFile.open(outFileName.c_str());

		distAdmin.test(inFileName, outFile);
	}
	else
	{
		distAdmin.test(inFileName, std::cout);
	}

}

void AdminTools::configDist()
{
	distAdmin.configure();
}

void AdminTools::testLocalize()
{
	std::ofstream outFile;
	std::string inFileName, outFileName;
	char choice;

	locAdmin.setDebug(false);

	std::cout << "Please enter an input file: ";
	std::cin >> inFileName;

	do
	{
		std::cout << "Would you like to capture the output in a file? (y/n): ";
		std::cin >> choice;
	} while('y' != choice && 'n' != choice);

	if('y' == choice)
	{
		std::cout << "Please enter an output file name: ";
		std::cin >> outFileName;

		outFile.open(outFileName.c_str());

		locAdmin.test(inFileName, outFile);
	}
	else
	{
		locAdmin.test(inFileName, std::cout);
	}
}
