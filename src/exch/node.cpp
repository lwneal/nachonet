/*******************************************************************************
File:				node.cpp
Author: 		Josh Siva
Date:				2/25/14
Project:		NachoNet
Purpose:		Implements the behavior of the node object
*******************************************************************************/

#include "../../include/exch/node.h"

/*******************************************************************************
 * Constructor:	node
 *
 * Description: Initializes the id and location of the node
 *
 * Parameters:  id - the id of the node
 * 							loc - the location of the node
 *
 * Returned:		None
 ******************************************************************************/
node::node (int id, location loc)
{
	setID (id);
	setLocation (loc);
}

/*******************************************************************************
 * Destroyer:		~node
 *
 * Description:	Sets the id to unused value and clears all of the measurements
 *
 * Parameters:  None
 *
 * Returned:		None
 ******************************************************************************/
node::~node ()
{
	id = NO_ID;
	measurements.clear ();
}

/*******************************************************************************
 * Method:			getID
 *
 * Description:	Returns the id of the node
 *
 * Parameters:	None
 *
 * Returned:		int - the node's id
 ******************************************************************************/
int node::getID () const
{
	return id;
}

/*******************************************************************************
 * Method:			setID
 *
 * Description:	Sets the node id
 *
 * Parameters:	id - the new id
 *
 * Returned:		None
 ******************************************************************************/
void node::setID (int id)
{
	this->id = id;
}

/*******************************************************************************
 * Method:			getLocation
 *
 * Description:	Returns the location of the node
 *
 * Parameters:	None
 *
 * Returned:		location - the node's location
 ******************************************************************************/
location node::getLocation () const
{
	return loc;
}

/*******************************************************************************
 * Method:			setLocation
 *
 * Description:	Set the location of the node
 *
 * Parameters:	loc - the new location
 *
 * Returned:		None
 ******************************************************************************/
void node::setLocation (location loc)
{
	this->loc = loc;
}

/*******************************************************************************
 * Method:			setMeasurement
 *
 * Description:	Set a measurement for a given device
 *
 * Parameters:	devDist - the distance measurement of a particular device
 *
 * Returned:		None
 ******************************************************************************/
void node::setMeasurement (distMeasurement devDist)
{
	measurements[devDist.devID] = devDist;
}

/*******************************************************************************
 * Method:			getMeasurement
 *
 * Description:	Get a reference measurement for a particular device.
 *
 * Parameters:	devID - the device whose reference measurement we want to get
 *
 * Returned:		refMeasurement - a distance combined with the node location
 ******************************************************************************/
refMeasurement node::getMeasurement (std::string devID) const
{
	refMeasurement ref;
	ref.nodeLocation = loc;
	ref.devDist = measurements[devID];

	return ref;
}

/*******************************************************************************
 * Method:			getAllMeasurements
 *
 * Description:	Get a vector of all of the measurements that the node has taken
 *
 * Parameters:	None
 *
 * Returned:		vector of reference measurements
 ******************************************************************************/
std::vector<refMeasurement> node::getAllMeasurements ()
{
	std::vector<refMeasurement> allMeasurements;

	for (auto measurement : measurements)
	{
		allMeasurements.push_back (getMeasurement (measurement.first));
	}

	return allMeasurements;
}

/*******************************************************************************
 * Method:			clearMeasurements
 *
 * Description:	Clear all of the measurements made by the node
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void node::clearMeasurements ()
{
	measurements.clear();
}


