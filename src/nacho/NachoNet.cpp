/*******************************************************************************
File:				NachoNet.cpp
Author: 		Josh Siva
Date:				3/19/14
Project:		NachoNet
Purpose:		Implements the behavior of the NachoNet object which is the object
						that integrates all of the pieces of NachoNet (including admin
						functionality)
*******************************************************************************/

#include "../../include/nacho/NachoNet.h"

const std::string NachoNet::DIST_CONFIG_FILE = "dist.json";

/*******************************************************************************
 * Constructor:	NachoNet
 *
 * Description:	Initializes the debug and verbose variables. Sets up the modules
 * 							and kicks of the listener who will listen for things to do as
 * 							soon as the data exchange module is initialized
 *
 * Parameters:	debug - flag indicating whether to use debug mode or not
 * 							verbose - flag indicating whether to use verbose mode or not
 * 							feeder - flag indicating whether we are using a DataFeeder for
 * 											 data collection
 * 							feederFile - the file name used by the DataFeeder
 *
 * Returned:		None
 ******************************************************************************/
NachoNet::NachoNet (bool debug, bool verbose, bool feeder,
										std::string feederFile)
{
	EZConfig distConfig (DIST_CONFIG_FILE);
	this->debug = debug;
	this->verbose = verbose;
	this->feeder = feeder;

	if (isVerbose ())
	{
		std::cout << "Initializing modules\n";
	}

	pMap = new MapOnTheCouch;
	pMap->load ();

	if (feeder)
	{
		pDataCollect = new DataFeeder (feederFile, debug);
	}
	else
	{
		pDataCollect = new stdCollect (stdCollect::DEFAULT_IFACE, debug);
	}
	pDistMeasure = new pathLoss (debug, &distConfig);
	pDataEx = NULL;
	pLocalization = new localization (debug);
	pWorker = NULL;

	active = true;
	pListener = new std::thread (&NachoNet::listener, this);

	if (isVerbose ())
	{
		std::cout << "Message listener started\n";
	}
}

/*******************************************************************************
 * Destroyer!:	~NachoNet
 *
 * Description:	Delete all modules and kill all threads. This includes shutting
 * 							down NachoNet if it's running.
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
NachoNet::~NachoNet ()
{
	if (NULL != pWorker)
	{
		if (isVerbose ())
		{
			std::cout << "Stopping NachoNet\n";
		}

		pDataEx->setIsAlive (false);

		pWorker->join ();
		delete pWorker;

		pWorker = NULL;
	}

	if (isVerbose ())
	{
		std::cout << "Stopping message listener\n";
	}

	active = false;
	pListener->join ();
	delete pListener;


	if (isVerbose ())
	{
		std::cout << "Deleting modules\n";
	}

	delete pMap;
	delete pDataCollect;
	delete pDistMeasure;

	if (NULL != pDataEx)
	{
		delete pDataEx;
	}

	delete pLocalization;

	if (isVerbose ())
	{
		std::cout << "Adios!\n";
	}
}

/*******************************************************************************
 * Method:			stop
 *
 * Description: Stop the worker thread (which carries out all data collection
 * 							and calculations) and tell other nodes to the same.
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void NachoNet::stop ()
{
	Message message;

	if (!pDataEx->alive ())
	{
		std::cout << "NachoNet is already stopped... Now doing less than nothing.\n";
	}
	else if (NULL == pDataEx)
	{
		std::cout << "This node has not yet been added to NachoNet.\n";
	}
	else
	{
		pDataEx->setIsAlive (false);

		pWorker->join ();
		delete pWorker;

		pWorker = NULL;

		message.msg = dataEx::STOP;

		for (int id : pDataEx->getNodeIDs ())
		{
			message.dest.push_back (id);
		}

		pDataEx->ping (message);

		if (isVerbose ())
		{
			std::cout << "No more nachos :'(\n";
		}
	}
}

/*******************************************************************************
 * Method:			start
 *
 * Description: Start the worker thread (which carries out all data collection
 * 							and calculations) and send a message to other nodes in
 * 							NachoNet to do the same.
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void NachoNet::start ()
{
	Message message;
	if (pDataEx->alive ())
	{
		std::cout << "NachoNet is already running\n";
	}
	else if (NULL == pDataEx)
	{
		std::cout << "This node has not yet been added to NachoNet.\n";
	}
	else
	{
		pDataEx->setIsAlive (true);

		pWorker = new std::thread (&NachoNet::worker, this);

		message.msg = dataEx::START;

		for (int id : pDataEx->getNodeIDs ())
		{
			message.dest.push_back (id);
		}

		pDataEx->ping (message);

		if (isVerbose ())
		{
			std::cout << "Nachos for everyone! :D\n";
		}
	}

}

/*******************************************************************************
 * Method:			worker
 *
 * Description: Gathers signal strengths for devices in the area, calculates
 * 							distances from these measurements then shares these measurements
 * 							with the data exchange module. Also pulls data out of the data
 * 							exchange module to localize a device. This data is then pushed
 * 							back into the data exchange module. (runs as a thread)
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void NachoNet::worker ()
{
	std::vector<ssMeasurement> signalStrengths;
	std::vector<distMeasurement> distances;
	std::vector<refMeasurement> referenceDistances;
	location devLocation;

	while (pDataEx->alive ())
	{
		//get updated node data
		pDataEx->pullUpdates (dataExOnTheCouch::NODES);

		//collect data from network
		pDataCollect->readFromNetwork ();
		signalStrengths = pDataCollect->getSS ();

		//calculate distances from signal strengths
		for (auto & ss : signalStrengths)
		{
			distances.push_back (pDistMeasure->measure (ss));
		}

		//update measurement information in data exchange
		for (auto & dist : distances)
		{
			pDataEx->updateDevMeasurement (dist);
		}

		//share new data with the rest of NachoNet
		pDataEx->pushUpdates (dataExOnTheCouch::NODES);

		//get updated device data
		pDataEx->pullUpdates (dataExOnTheCouch::DEVICES);

		//get reference measurements for a valid device
		referenceDistances = pDataEx->getMeasurements (pDataEx->getDevForUpdate ());

		//we must have exactly 3 measurements to trilaterate
		if (REQUIRED_MEASUREMENTS == referenceDistances.size ())
		{
			//trilateration
			devLocation = pLocalization->localize (referenceDistances[0],
																						 referenceDistances[1],
																						 referenceDistances[2]);

			//update device location in data exchange
			pDataEx->updateDevLocation (devLocation.theID.strID, devLocation);
		}

		//share updated location with the rest of NachoNet
		pDataEx->pushUpdates (dataExOnTheCouch::DEVICES);

		signalStrengths.clear ();
		distances.clear ();
		referenceDistances.clear ();
	}
}

/*******************************************************************************
 * Method:			listener
 *
 * Description:	Handles all checking of messages for the data exchange module
 * 							and listens for a command to start or stop running. Kicks off
 * 							or kills the worker thread accordingly. (runs as a thread)
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void NachoNet::listener ()
{
	bool stateChange = false;

	while (active)
	{
		if (NULL != pDataEx)
		{
			pDataEx->checkMessages ();

			stateChange = stateChange ^ pDataEx->alive ();

			if (stateChange && pDataEx->alive ())
			{
				pDataEx->setIsAlive (true);

				pWorker = new std::thread (&NachoNet::worker, this);
			}
			else if (stateChange && !pDataEx->alive ())
			{
				stateChange = false;

				pWorker->join ();
				delete pWorker;

				pWorker = NULL;

			}
		}
	}
}

/*******************************************************************************
 * Method:			listDevices
 *
 * Description:	Prints a nice table of node IDs, device IDs and their locations
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void NachoNet::listDevices ()
{
	std::vector<location> nodeLocations;
	std::vector<location> devLocations;

	if (NULL == pDataEx)
	{
		std::cout << "This node has not been added to NachoNet.\n";
	}
	else
	{
		nodeLocations = pDataEx->getNodeLocations ();
		devLocations = pDataEx->getDeviceLocations ();

		std::cout << "                     LOCATIONS\n"
							<< "                     ---------\n"
							<< "Nodes:\n";

		for (auto & nodeLoc : nodeLocations)
		{
			std::cout << nodeLoc.theID.intID << "   (" << nodeLoc.x << ", "
																								 << nodeLoc.y << ")\n";
		}
		std::cout << "\n--------------------------------------------------------\n";
		std::cout << "Devices:\n";

		for (auto & devLoc : devLocations)
		{
			std::cout << devLoc.theID.strID << "   ";

			if (0.0f == devLoc.x && 0.0f == devLoc.y)
			{
				std::cout << "(?,?)\n";
			}
			else
			{
				std::cout << "(" << devLoc.x << ", " << devLoc.y << ")\n";
			}
		}

	}
}

/*******************************************************************************
 * Method:			addNode
 *
 * Description:	Add this node to NachoNet provided it has not already been added
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void NachoNet::addNode ()
{
	if (NULL != pDataEx)
	{
		std::cout << "This node has already been added.\n";
	}
	else
	{
		if (isVerbose ())
		{
			std::cout << "Adding node to NachoNet...\n";
		}

		pDataEx = new dataExOnTheCouch ();


		if (isVerbose ())
		{
			std::cout << "Node added! Be sure to update the node's location under the"
								<< " management menu.\n";
		}
	}
}

/*******************************************************************************
 * Method:			dropNode
 *
 * Description:	Remove a node from NachoNet if it exists in NachoNet.
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void NachoNet::dropNode ()
{
	if (NULL == pDataEx)
	{
		std::cout << "This node has not been added.\n";
	}
	else
	{
		if (isVerbose ())
		{
			std::cout << "Removing node from NachoNet...\n";
		}

		delete pDataEx;
		pDataEx = NULL;


		if (isVerbose ())
		{
			std::cout << "Node removed.\n";
		}
	}
}

/*******************************************************************************
 * Method:			updateMap
 *
 * Description:	Get user input to update the map of the room. If the data is
 * 							valid then it is saved.
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void NachoNet::updateMap ()
{
	pMap->load ();

	if (!pMap->getUserInput ())
	{
		std::cout << "Failed to set map.\n";
	}
	else
	{
		pMap->save ();
	}
}

/*******************************************************************************
 * Method:			isDebug
 *
 * Description: Returns whether debug mode is on
 *
 * Parameters:	None
 *
 * Returned:		bool
 ******************************************************************************/
bool NachoNet::isDebug ()
{
	return this->debug;
}

/*******************************************************************************
 * Method:			isVerbose
 *
 * Description: Returns whether verbose mode is on
 *
 * Parameters:	None
 *
 * Returned:		bool
 ******************************************************************************/
bool NachoNet::isVerbose ()
{
	return this->verbose;
}


