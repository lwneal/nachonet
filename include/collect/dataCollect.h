/*******************************************************************************
File:				dataCollect
Author: 		Josh Siva
Date:				-----
Project:		NachoNet
Purpose:		The file defines the interface to the data collection module
*******************************************************************************/

#pragma once
#include <string>

typedef struct ssMeasurement
{
  std::string devID;
  int ss;
} ssMeasurement;
