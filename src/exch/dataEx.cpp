/*******************************************************************************
File:				dataEx.cpp
Author: 		Josh Siva
Date:				2/27/14
Project:		NachoNet
Purpose:		Defines the behavior of the data exchange abstract class
*******************************************************************************/

#include "../../include/exch/dataEx.h"

//MESSAGES TO SEND
const std::string dataEx::HELLO = "HELLO";			//check if node is alive
const std::string dataEx::GOODBYE = "GOODBYE";  //tell other nodes I'm
																							 //leaving
const std::string dataEx::ACK = "ACK";					//acknowledge a HELLO
const std::string dataEx::STOP = "STOP";				//tell other nodes to stop
const std::string dataEx::START = "START";			//tell other nodes to start

//STATES
const std::string dataEx::RUNNING = "RUNNING";  //state of running node
const std::string dataEx::DEAD = "DEAD";				//state of dead node


/*******************************************************************************
 * Constructor:	dataEx
 *
 * Description:	Initializes the id and the last node used where the latter is
 * 							used to determine which nodes' measurements should be used for
 * 							localization of a device
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
dataEx::dataEx ()
{
	setID (node::NO_ID);
}

/*******************************************************************************
 * Destroyer!:	~dataEx
 *
 * Description:	virtual Destroyer... does nothing
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
dataEx::~dataEx ()
{

}

/*******************************************************************************
 * Method:			getNumNodes
 *
 * Description:	Gets the number of nodes in the network.
 *
 * Parameters:	None
 *
 * Returned:		int - number of nodes
 ******************************************************************************/
int dataEx::getNumNodes ()
{
	return nodes.size ();
}

/*******************************************************************************
 * Method:			getNumDevs
 *
 * Description:	Gets the number of devices being tracked
 *
 * Parameters:	None
 *
 * Returned:		int - the number of devices
 ******************************************************************************/
int dataEx::getNumDevs ()
{
	return devices.size ();
}

/*******************************************************************************
 * Method:			setID
 *
 * Description:	Sets the id of the current node
 *
 * Parameters:	id - the current node's id
 *
 * Returned:		None
 ******************************************************************************/
void dataEx::setID (int id)
{
	myID = id;
}

/*******************************************************************************
 * Method:			getID
 *
 * Description:	Gets the current node's id
 *
 * Parameters:	None
 *
 * Returned:		int - the node's id
 ******************************************************************************/
int dataEx::getID () const
{
	return myID;
}

/*******************************************************************************
 * Method:			setIsAlive
 *
 * Description:	Set the sate of NachoNet. Alive means that measurements
 * 							are being taken and localization is occurring.
 *
 * Parameters:	state - the new state of NachoNet on this node
 *
 * Returned:		None
 ******************************************************************************/
void dataEx::setIsAlive (bool state)
{
	this->isAlive = state;
}

/*******************************************************************************
 * Method:			alive
 *
 * Description:	Returns whether NachoNet is running on this node
 *
 * Parameters:	None
 *
 * Returned:		true if NachoNet is running, false otherwise
 ******************************************************************************/
bool dataEx::alive () const
{
	return this->isAlive;
}

/*******************************************************************************
 * Method:			getDevForUpdate
 *
 * Description:	Gets the id of a device that this node is eligible to update.
 * 							Uses hashpjw (p.436, Aho, Sethi, Ullman) to grab device ids.
 *
 * Parameters:	None
 *
 * Returned:		std::string - the device id
 ******************************************************************************/
std::string dataEx::getDevForUpdate ()
{
	std::vector<std::string> myDevsToUpdate;
	std::vector<std::string>::iterator dev;
	std::string device;

	char * p;
	unsigned h = 0, g;

	for (auto dev : devices)
	{
		for (p = (char *)dev.first.c_str(); *p != '\0'; p++)
		{
			h = (h << 4) * (*p);

			if ((g) = h&0xf0000000)
			{
				h = h ^ (g >> 24);
				h = h ^ g;
			}
		}

		if ((unsigned)getID () == h % getNumNodes ())
		{
			myDevsToUpdate.push_back (dev.first);
		}
	}

	//now choose a random element to update since we have no idea how much this
	//vector has changed since the last time we called this function

	if (0 != myDevsToUpdate.size ())
	{
		dev = myDevsToUpdate.begin ();
		srand (time (0));
		std::advance (dev, (rand() % myDevsToUpdate.size ()));

		device = *dev;
	}
	else
	{
		device = "";
	}

	return device;
}

/*******************************************************************************
 * Method:			getNodeIDs
 *
 * Description:	Get a vector of the IDs of nodes in NachoNet
 *
 * Parameters:	None
 *
 * Returned:		vector of IDs (ints)
 ******************************************************************************/
std::vector<int> dataEx::getNodeIDs ()
{
	std::vector<int> ids;

	for (auto thisNode : nodes)
	{
		ids.push_back (thisNode.first);
	}

	return ids;
}

/*******************************************************************************
 * Method:			getMeasurements
 *
 * Description:	Gets a vector of three reference measurements to be sent off to
 * 							the localization module.
 *
 * Parameters:	id - the id of the device whose measurements we need
 *
 * Returned:		vector of reference measurements (can be empty)
 ******************************************************************************/
std::vector<refMeasurement> dataEx::getMeasurements (std::string id)
{
	const int REQ_NODES = 3;
	std::vector<refMeasurement> measurements;
	refMeasurement entry;
	int miss = 0;
	std::map<int, node>::iterator thisNode;

	srand (time (0));

	for (int i = 0; i < REQ_NODES; i++)
	{
		do
		{
			thisNode = nodes.begin ();
			std::advance (thisNode, (getID () + i) % nodes.size ());
			entry = thisNode->second.getMeasurement (id);

			if ("" == entry.devDist.devID)
			{
				miss++;
			}

		} while ("" == entry.devDist.devID && (REQ_NODES <= getNumNodes() - miss));

		if (REQ_NODES <= getNumNodes() - miss)
		{
			measurements.push_back (entry);
		}

	}

	return measurements;
}

/*******************************************************************************
 * Method:			updateDevLocation
 *
 * Description:	Update the location of a tracked device in the network. Also
 * 							track the devices that have been updated.
 *
 * Parameters:	id - the id of the device to update
 * 							loc - the location of the device
 *
 * Returned:		None
 ******************************************************************************/
void dataEx::updateDevLocation (std::string id, location loc)
{
	devices[id].setLocation (loc);

		//we need to track the updated devs so we throw them into a vector
	devsUpdatedSinceLastPush.push_back (devices[id]);
}

/*******************************************************************************
 * Method:			updateDevMeasurement
 *
 * Description:	Adds or updates a measurement for the current node
 *
 * Parameters:	devDist - the distance measurement by this node for a device
 *
 * Returned:		None
 ******************************************************************************/
void dataEx::updateDevMeasurement (distMeasurement devDist)
{
	device thisDev (devDist.devID);

	if (!devices.count (devDist.devID))
	{
		addDevice (thisDev);
	}

	nodes[myID].setMeasurement(devDist);
}

/*******************************************************************************
 * Method:			getDeviceLocations
 *
 * Description:	Returns the location of all of the devices currently being
 * 							tracked by NachoNet
 *
 * Parameters:	None
 *
 * Returned:		vector of device locations
 ******************************************************************************/
std::vector<location> dataEx::getDeviceLocations ()
{
	std::vector<location> locations;

	for (auto & dev : devices)
	{
		locations.push_back (dev.second.getLocation());
	}

	return locations;
}

/*******************************************************************************
 * Method:			getNodeLocations
 *
 * Description:	Get the locations of all of the nodes in NachoNet
 *
 * Parameters:	None
 *
 * Returned:		vector of node locations
 ******************************************************************************/
std::vector<location> dataEx::getNodeLocations ()
{
	std::vector<location> locations;

	for (auto & thisNode : nodes)
	{
		locations.push_back (thisNode.second.getLocation());
	}

	return locations;
}

/*******************************************************************************
 * Method:			setPingStatus
 *
 * Description:	This is used to track nodes that have acknowledged a HELLO from
 * 							this node.
 *
 * Parameters:	nodeId - the node whose status we want to update
 * 							status - the new status
 *
 * Returned:		None
 ******************************************************************************/
void dataEx::setPingStatus (int nodeID, bool status)
{
	aliveOnLastPing[nodeID] = status;
}

/*******************************************************************************
 * Method:			lastPingResult
 *
 * Description:	Get the result of the last ping - did the given node ACK the
 * 							last HELLO
 *
 * Parameters:	nodeID - the ID of the node to check
 *
 * Returned:		bool - true if the specified node ACK'd, false otherwise
 ******************************************************************************/
bool dataEx::lastPingResult (int nodeID)
{
	return aliveOnLastPing[nodeID];
}

/*******************************************************************************
 * Method:			clearDevices
 *
 * Description:	Remove all devices: stop tracking them
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void dataEx::clearDevices ()
{
	devices.clear ();
}

/*******************************************************************************
 * Method:			clearNodes
 *
 * Description:	Remove all nodes: clear nodes from the network
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void dataEx::clearNodes ()
{
	nodes.clear ();
}

/*******************************************************************************
 * Method:			addNode
 *
 * Description:	Add a complete node to the network (complete as in it better
 * 							have the necessary data)
 *
 * Parameters:	newNode - the node to add
 *
 * Returned:		None
 ******************************************************************************/
void dataEx::addNode (node newNode)
{
	nodes [newNode.getID()] = newNode;
}

/*******************************************************************************
 * Method:			dropNode
 *
 * Description:	Remove a node from NachoNet and returns it
 *
 * Parameters:	id - the id of the node to remove
 *
 * Returned:		the node that was just removed
 ******************************************************************************/
node dataEx::dropNode (int id)
{
	node returnNode;
	std::map<int, node>::iterator it;
	it = nodes.find (id);

	if (nodes.end () != it)
	{
		returnNode = it->second;
		nodes.erase (it);
	}

	return returnNode;
}

/*******************************************************************************
 * Method:			addDevice
 *
 * Description:	Add a complete device to the network (the device better have all
 * 							the necessary fields filled out)
 *
 * Parameters:	newDev - the new device
 *
 * Returned:		None
 ******************************************************************************/
void dataEx::addDevice (device newDev)
{
	devices [newDev.getID()] = newDev;
	devsUpdatedSinceLastPush.push_back (newDev);
}

/*******************************************************************************
 * Method:			dropDevice
 *
 * Description:	Remove a device from NachoNet
 *
 * Parameters:	id - the id of the device to remove
 *
 * Returned:		None
 ******************************************************************************/
void dataEx::dropDevice (std::string id)
{
	std::map<std::string, device>::iterator it;
	it = devices.find (id);

	if (devices.end () != it)
	{
		devices.erase (it);
	}
}
