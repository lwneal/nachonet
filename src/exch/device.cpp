/*******************************************************************************
File:				device.cpp
Author: 		Josh Siva
Date:				2/25/14
Project:		NachoNet
Purpose:		Implements the behavior of the device object
*******************************************************************************/

#include "../../include/exch/device.h"

/*******************************************************************************
 * Constructor:	device
 *
 * Description:	Initializes the id and location of the device
 *
 * Parameters:	id - the device id (MAC address)
 *
 * Returned:		None
 ******************************************************************************/
device::device (std::string id)
{
	setID (id);
	loc.x = 0.0;
	loc.y = 0.0;
}

/*******************************************************************************
 * Destroyer:		~device
 *
 * Description:	Sets the id to null
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
device::~device ()
{
	id = "";
}

/*******************************************************************************
 * Method:			getID
 *
 * Description:	Returns the id of the device
 *
 * Parameters:	None
 *
 * Returned:		string - the device id
 ******************************************************************************/
std::string device::getID () const
{
	return id;
}

/*******************************************************************************
 * Method:			setID
 *
 * Description: Set the device id
 *
 * Parameters:	id - the device id (MAC address)
 *
 * Returned:		None
 ******************************************************************************/
void device::setID (std::string id)
{
	this->id = id;
}

/*******************************************************************************
 * Method:			getLocation
 *
 * Description: Returns the location of the device
 *
 * Parameters:	None
 *
 * Returned:		location - the device location
 ******************************************************************************/
location device::getLocation () const
{
	return this->loc;
}

/*******************************************************************************
 * Method:			setLocation
 *
 * Description:	Sets the device location
 *
 * Parameters:	loc - the new location
 *
 * Returned:		None
 ******************************************************************************/
void device::setLocation (location loc)
{
	this->loc = loc;
}


