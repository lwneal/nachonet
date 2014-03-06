/*******************************************************************************
File:				dataExOnTheCouch.cpp
Author: 		Josh Siva
Date:				3/5/14
Project:		NachoNet
Purpose:		Implements the behavior of the CouchDB data exchange module. This
						module will need to communicate with two CouchDB databases per node,
						node_db and dev_db.

						A document in the node_db looks like this:
						{
							"_id" : nodeID,
							"_rev" : [auto],
							"location" : {"x" : x, "y" : y},
							"measurements" : [{"devID" : devID, "dist" : dist}, ...]
						}

						A document in the dev_db looks like this:
						{
							"_id" : devID,
							"_rev" : [auto],
							"location" : {"x" : x, "y" : y}
						}
*******************************************************************************/

#include "../../include/exch/dataExOnTheCouch.h"

/*******************************************************************************
 * Constructor:	dataExOnTheCouch
 *
 * Description:	The constructor is responsible for the initial communication
 * 							between this node and the rest of NachoNet. Initial
 * 							synchronization and discovery happens at this point.
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
dataExOnTheCouch::dataExOnTheCouch ()
{

}

/*******************************************************************************
 * Destroyer!:	~dataExOnTheCouch
 *
 * Description:	Does nothing...
 *
 * Parameters:
 *
 * Returned:
 ******************************************************************************/
virtual dataExOnTheCouch::~dataExOnTheCouch ()
{

}

/*******************************************************************************
 * Method:			addNode
 *
 * Description: Adds a node to NachoNet
 *
 * Parameters:
 *
 * Returned:
 ******************************************************************************/
virtual void dataExOnTheCouch::addNode (node newNode)
{

}

/*******************************************************************************
 * Method:
 *
 * Description:
 *
 * Parameters:
 *
 * Returned:
 ******************************************************************************/
virtual void dataExOnTheCouch::dropNode (int id)
{

}

/*******************************************************************************
 * Method:
 *
 * Description:
 *
 * Parameters:
 *
 * Returned:
 ******************************************************************************/
virtual void dataExOnTheCouch::addDevice (device newDev)
{

}

/*******************************************************************************
 * Method:
 *
 * Description:
 *
 * Parameters:
 *
 * Returned:
 ******************************************************************************/
virtual void dataExOnTheCouch::dropDevice (std::string id)
{

}

/*******************************************************************************
 * Method:
 *
 * Description:
 *
 * Parameters:
 *
 * Returned:
 ******************************************************************************/
virtual void dataExOnTheCouch::pingAll (Message message)
{

}

/*******************************************************************************
 * Method:
 *
 * Description:
 *
 * Parameters:
 *
 * Returned:
 ******************************************************************************/
virtual void dataExOnTheCouch::ping (Message message)
{

}

/*******************************************************************************
 * Method:
 *
 * Description:
 *
 * Parameters:
 *
 * Returned:
 ******************************************************************************/
virtual void dataExOnTheCouch::pushUpdates ()
{

}

/*******************************************************************************
 * Method:
 *
 * Description:
 *
 * Parameters:
 *
 * Returned:
 ******************************************************************************/
virtual void dataExOnTheCouch::pullUpdates ()
{

}

/*******************************************************************************
 * Method:
 *
 * Description:
 *
 * Parameters:
 *
 * Returned:
 ******************************************************************************/
void dataExOnTheCouch::updateNodeFromCouch ()
{

}

/*******************************************************************************
 * Method:
 *
 * Description:
 *
 * Parameters:
 *
 * Returned:
 ******************************************************************************/
void dataExOnTheCouch::updateDevFromCouch ()
{

}

/*******************************************************************************
 * Method:
 *
 * Description:
 *
 * Parameters:
 *
 * Returned:
 ******************************************************************************/
void dataExOnTheCouch::updateCouchFromNode ()
{

}

/*******************************************************************************
 * Method:
 *
 * Description:
 *
 * Parameters:
 *
 * Returned:
 ******************************************************************************/
void dataExOnTheCouch::updateCouchFromDev ()
{

}
