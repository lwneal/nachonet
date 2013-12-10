/*******************************************************************************
File:				adminDistMeasure
Author: 		Josh Siva
Date:				11/29/13
Project:		NachoNet
Purpose:		Defines the interface to the module that is responsible for
						testing the distance measurement module through the admin tools
*******************************************************************************/

#pragma once
#include "../dist/pathLoss.h"
#include "../dist/fsPathLoss.h"
#include "../dist/logNormalShadow.h"
#include "../util/config.h"

class adminDistMeasure
{
	public:
		adminDistMeasure();
		~adminDistMeasure();
		void setDebug(bool debug);
		void setEqn(int eqnType);
		void configure();
		void test(std::string inputFile, std::ostream & os);

		typedef enum DIST_EQN
		{
			PATH_LOSS = 1,
			FREE_SPACE_PATH_LOSS,
			LOG_NORMAL_SHADOW
		}DIST_EQN;


	private:
		bool debug;
		Config distConfig;
		distMeasure *pDistEqn;
};
