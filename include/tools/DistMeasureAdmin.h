/*******************************************************************************
File:				DistMeasureAdmin.h
Author: 		Josh Siva
Date:				3/18/14
Project:		NachoNet
Purpose:		Defines the interface to the admin tool that will be used with the
						Distance Measurement object
*******************************************************************************/

#pragma once
#include "../dist/dist.h"
#include "../dist/fsPathLoss.h"
#include "../dist/logNormalShadow.h"
#include "../dist/pathLoss.h"
#include "../util/config.h"

class DistMeasureAdmin : Admin
{
	public:
		DistMeasureAdmin (distMeasure **);
		virtual void options ();
		virtual void configure ();
		virtual void test ();

		typedef enum DIST_EQN
		{
			PATH_LOSS = 1,
			FREE_SPACE_PATH_LOSS,
			LOG_NORMAL_SHADOW
		}DIST_EQN;

	private:
		distMeasure *pDistMeasure;
		Config distConfig;
};

