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