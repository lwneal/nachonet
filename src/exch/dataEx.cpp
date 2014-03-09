/*******************************************************************************
File:				dataEx.cpp
Author: 		Josh Siva
Date:				2/27/14
Project:		NachoNet
Purpose:		Defines the behavior of the data exchange abstract class
*******************************************************************************/

#include "../../include/exch/dataEx.h"

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
	setID (NO_ID);
	lastNodeUsed = 0;
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
virtual dataEx::~dataEx ()
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
 *
 * Parameters:	None
 *
 * Returned:		std::string - the device id
 ******************************************************************************/
std::string dataEx::getDevForUpdate ()
{
	//hash devID vs num nodes in network
	//use hashed devID as a key for a std::map
}

/*******************************************************************************
 * Method:			getMeasurements
 *
 * Description:	Gets a vector of three reference measurements to be sent off to
 * 							the localization module.
 *
 * Parameters:	id - the id of the device whose measurements we need
 *
 * Returned:		vector of reference measurements
 ******************************************************************************/
std::vector<refMeasurement> dataEx::getMeasurements (std::string id)
{
	const int REQ_NODES = 3;
	std::vector<refMeasurement> measurements;
	int currentNode;

	for (int i = 0; i < REQ_NODES; i++)
	{
		currentNode = lastNodeUsed++ % nodes.size ();
		measurements.push_back (nodes[currentNode].getMeasurement(id));
	}

	return measurements;
}

/*******************************************************************************
 * Method:			updateDevLocation
 *
 * Description:	Update the location of a tracked device in the network.
 *
 * Parameters:	id - the id of the device to update
 * 							loc - the location of the device
 *
 * Returned:		None
 ******************************************************************************/
void dataEx::updateDevLocation (std::string id, location loc)
{
	devices[id].setLocation (loc);
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
std::vector<location> dataEx::getDeviceLocations () const
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
std::vector<location> dataEx::getNodeLocations () const
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
bool dataEx::lastPingResult (int nodeID) const
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
virtual void dataEx::addNode (node newNode)
{
	nodes [newNode.getID()] = newNode;
}

/*******************************************************************************
 * Method:			dropNode
 *
 * Description:	Remove a node from NachoNet
 *
 * Parameters:	id - the id of the node to remove
 *
 * Returned:		None
 ******************************************************************************/
virtual void dataEx::dropNode (int id)
{
	std::map<int, node>::iterator it;
	it = nodes.find (id);

	if (nodes.end () != it)
	{
		nodes.erase (it);
	}
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
virtual void dataEx::addDevice (device newDev)
{
	devices [newDev.getID()] = newDev;
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
virtual void dataEx::dropDevice (std::string id)
{
	std::map<std::string, device>::iterator it;
	it = devices.find (id);

	if (devices.end () != it)
	{
		devices.erase (it);
	}
}
