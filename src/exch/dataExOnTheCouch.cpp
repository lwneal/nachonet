/*******************************************************************************
 File:		dataExOnTheCouch.cpp
 Author: 	Josh Siva
 Date:		3/5/14
 Project:	NachoNet
 Purpose:	Implements the behavior of the CouchDB data exchange module. This
				  module will need to communicate with three CouchDB databases per
				  node:	node_db, dev_db, and admin_db.

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

 A document in the admin_db looks like this:
 {
	 "_id" : nodeID,
	 "_rev" : [auto],
	 "ip" : ipAddr,
	 "state" : state,
	 "message" : [{"msg" : message, "src" : nodeID}, ...]
 }
 *******************************************************************************/

#include "../../include/exch/dataExOnTheCouch.h"
#include "../../include/util/config.h"

/*******************************************************************************
 * Constructor:	dataExOnTheCouch
 *
 * Description:	The constructor is responsible for the initial communication
 * 							between this node and the rest of NachoNet. Initial
 * 							synchronization and discovery happens in the admin_db happens at
 * 							this point. The id of this node is figured out and set, and all
 * 							of the node and device objects are set up. Finally, this node
 * 							creates a new entry for itself in admin_db and node_db and
 * 							synchronizes with the rest of NachoNet.
 *
 * 							###############################################################
 * 							#       CouchDB must be running BEFORE the program begins     #
 * 							###############################################################
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
dataExOnTheCouch::dataExOnTheCouch () : nachoCast ()
{
	struct ifaddrs * pIfaceAddr = NULL;
	struct ipaddrs * pIface = NULL;
	void * pTempAddr = NULL;
	char addrBuffer[INET_ADDRSTRLEN];
	char message[multicast::BUF_LENGTH];
	std::string myAddress, url;
	std::ostringstream oss;
	JSON json;
	jsonData data;
	jsonParser parser;
	std::clock_t startTimeout;
	double duration;
	int nextID = NO_ID;
	location loc;
	loc.x = 0;
	loc.y = 0;
	loc.theID.intID = NO_ID;
	node newNode (NO_ID, loc);

	//Let's go get our IP address
	getifaddrs (&pIfaceAddr);

	for (pIface = pIfaceAddr; pIface != NULL; pIface = pIface->ifa_next)
	{
		//check for IPv4 address
		if (pIface->ifa_addr->sa_family == AF_INET)
		{
			pTempAddr = &((struct sockaddr_in *)pIface->ifa_addr)->sin_addr;
			inet_ntop (AF_INET, pTempAddr, addrBuffer, INET_ADDRSTRLEN);

			myAddress = addrBuffer;
			setIP (myAddress);
		}

	}

	if (NULL != pIfaceAddr)
	{
		freeifaddrs (pIfaceAddr);
	}

	//prepare our broadcast message
	for (int i = 0; i < multicast::BUF_LENGTH; i++)
	{
		message[i] = '\0';
	}

	for (int i = 0; i < myAddress.length(); i++)
	{
		message[i] = myAddress[i];
	}

	//broadcast our IP address to anybody listening
	nachoCast.transmit (message);

	startTimeout = std::clock ();

	//now we need to check if anyone got back to us or for the timeout to occur
	url = "http://" + LOCALHOST + ':';
	url.append (std::to_string (DEFAULT_COUCH_PORT));
	url += '/' + TARGET_DB[ADMIN] + '/' + ALL_DOCS_Q;

	do
	{
		if (CURLE_OK == curlRead (url, oss))
		{
			// Web page successfully written to string
			parser (oss.str());
			json = parser.getObject ();
			oss.str ("");
		}

		duration = ( std::clock() - startTimeout ) / (double) CLOCKS_PER_SEC;

	} while (0 >= json.getData (TOTAL_ROWS).value.intVal
					 || TIMEOUT > duration);

	//we heard back so we need to go through admin_db and set up our local data
	//to prepare for the first full sync
	if (0 < json.getData (TOTAL_ROWS).value.intVal)
	{
		for (auto & entry : json.getData (ROWS).value.array)
		{
			nodeIPAddr[entry.value.pObject->getData(ID).value.intVal]
			           = entry.value.pObject->getData(IP).value.strVal;

			nodes[entry.value.pObject->getData(ID).value.intVal] = newNode;

			//figure out our ID
			if (entry.value.pObject->getData(ID).value.intVal > nextID)
			{
				nextID = entry.value.pObject->getData(ID).value.intVal + 1;
			}
		}

		pullUpdates (ADMIN);
		pullUpdates (NODES);
		pullUpdates (DEVICES);

		setID (nextID);
	}
	else if (TIMEOUT < duration) //we didn't hear back so we are the first node
	{
		setID (0);
	}

	//now add our docs to the database
	json.clear();
	data.type = jsonParser::INT_TYPE;
	data.value.intVal = std::to_string(getID ());
	json.setValue(ID, data);

	data.type = jsonParser::STR_TYPE;
	data.value.strVal = getIP ();
	json.setValue (IP, data);

	data.value.strVal = DEAD;
	json.setValue (STATE, data);

	data.type = jsonParser::VEC_TYPE;
	data.value.array.clear ();
	json.setValue (MESSAGE, data);

	url = "http://" + LOCALHOST + ':';
	url.append (std::to_string (DEFAULT_COUCH_PORT));
	url += '/' + TARGET_DB[ADMIN] + '/' + std::to_string(getID ());

	curlPost (url, json.writeJSON(""));

	json.clear ();
	//we can put a mostly empty doc in for the node because this will get updated
	//later
	data.type = jsonParser::INT_TYPE;
	data.value.intVal = std::to_string(getID ());
	json.setValue(ID, data);

	url = "http://" + LOCALHOST + ':';
	url.append (std::to_string (DEFAULT_COUCH_PORT));
	url += '/' + TARGET_DB[NODES] + '/' + std::to_string(getID ());

	curlPost (url, json.writeJSON(""));

	//Share our updates with everyone else
	pushUpdates (ADMIN);
	pushUpdates (NODES);

	//now set up our greeter for future new nodes
	stillGreetingNodes = true;
	pGreeter = new std::thread (&dataExOnTheCouch::greetNewNode,
															dataExOnTheCouch ());

}

/*******************************************************************************
 * Destroyer!:	~dataExOnTheCouch
 *
 * Description:	Clean up this node's presence in NachoNet. Send GOODBYE to other
 * 							nodes and remove the documents relevant to this node. Also stop
 * 							the greeter.
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
virtual dataExOnTheCouch::~dataExOnTheCouch ()
{
	Message message;
	std::string url;
	std::ostringstream oss;
	JSON json;
	jsonData data;
	jsonParser parser;

	stillGreetingNodes = false;

	message.msg = GOODBYE;

	for (auto & entry : nodeIPAddr)
	{
		message.dest.push_back (entry.first);
	}

	ping (message);

	//remove node from node_db and admin_db
	//add field {"_deleted" : true} to docs
	url = "http://" + LOCALHOST + ':';
	url.append (std::to_string (DEFAULT_COUCH_PORT));
	url += '/' + TARGET_DB[ADMIN] + '/' + std::to_string(getID ());

	if (CURLE_OK == curlRead (url, oss))
	{
		// Web page successfully written to string
		parser (oss.str());
		json = parser.getObject ();
		oss.str ("");

		data.type = jsonParser::BOOL_TYPE;
		data.value.boolVal = true;
		json.setValue (DELETED, data);

		curlPost (url, json.writeJSON(""));
	}

	url = "http://" + LOCALHOST + ':';
	url.append (std::to_string (DEFAULT_COUCH_PORT));
	url += '/' + TARGET_DB[NODES] + '/' + std::to_string(getID ());

	if (CURLE_OK == curlRead (url, oss))
	{
		// Web page successfully written to string
		parser (oss.str());
		json = parser.getObject ();
		oss.str ("");

		data.type = jsonParser::BOOL_TYPE;
		data.value.boolVal = true;
		json.setValue (DELETED, data);

		curlPost (url, json.writeJSON(""));
	}

	pushUpdates (ADMIN);
	pushUpdates (NODES);

	stillGreetingNodes = false;

	pGreeter->join ();
	delete pGreeter;

}

/*******************************************************************************
 * Method:			setIP
 *
 * Description:	Set the IP address of the current node
 *
 * Parameters:	newIP - the new IP address
 *
 * Returned:		None
 ******************************************************************************/
void dataExOnTheCouch::setIP (std::string newIP)
{
	this->myIP = newIP;
}

/*******************************************************************************
 * Method:			getIP
 *
 * Description:	Get the IP address of the current node
 *
 * Parameters:	None
 *
 * Returned:		ip - the IP address
 ******************************************************************************/
std::string dataExOnTheCouch::getIP () const
{
	return this->myIP;
}

/*******************************************************************************
 * Method:			greetNewNode
 *
 * Description:	Reads from the socket and looks for nodes that are just entering
 * 							the network and shouting their ip addresses at everyone on the
 * 							multicast. If we get an ip address we add dummy place holders
 * 							for the new node into nodeIPAddr and nodes and we send all of
 * 							the admin docs to the new node. This is meant to be run as a
 * 							thread in the background
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void dataExOnTheCouch::greetNewNode ()
{
	std::string message;
	JSON json;
	jsonData data;
	int nextID = NO_ID;
	location loc;
	loc.x = 0;
	loc.y = 0;
	loc.theID.intID = NO_ID;
	node newNode (NO_ID, loc);

	while (stillGreetingNodes)
	{
		message = nachoCast.receive ();

		//the message should at least be a null terminator
		if ('/0' != message[0])
		{
			//let's go find the next available ID for the new guy
			for (auto & entry : nodeIPAddr)
			{
				if (entry.first > nextID)
				{
					nextID = entry.first + 1;
				}
			}

			//add this new node
			nodeIPAddr[nextID] = message;
			newNode.setID(nextID);
			nodes[nextID] = newNode;

			//now send admin docs to the new guy
			data.type = jsonParser::STR_TYPE;
			data.value.strVal.clear ();
			data.value.strVal.append ("http://");
			data.value.strVal.append (message);
			data.value.strVal.push_back (':');
			data.value.strVal.append (std::to_string (DEFAULT_COUCH_PORT));
			data.value.strVal.push_back ('/');

			data.value.strVal.append (TARGET_DB [ADMIN]);

			json.setValue (TARGET, data);

			data.value.strVal.clear ();
			data.value.strVal = TARGET_DB [ADMIN];

			json.setValue (SOURCE, data);

			curlPost ('/' + REPLICATE, json.writeJSON("")); //admin docs sent
		}
	}
}

/*******************************************************************************
 * Method:			ping
 *
 * Description: Send a message to node. This amounts to changing the message
 * 							field in a specified node's admin document and checking back to
 * 							see if there is a response.
 *
 * Parameters: 	message - the message text and the destination(s)
 *
 * Returned:		None
 ******************************************************************************/
virtual void dataExOnTheCouch::ping (Message message)
{
	std::string url = "";
	std::ostringstream oss;
	JSON json, msgField;
	jsonData data, entry;
	jsonParser parser;

	data.type = jsonParser::STR_TYPE;
	data.value.strVal = message.msg;
	msgField.setValue (MSG_TEXT, data);

	data.value.strVal = std::to_string (getID ());
	msgField.setValue (MSG_SRC, data);

	entry.type = jsonParser::OBJ_TYPE;
	entry.value.pObject = &msgField;

	for (int nodeID : message.dest)
	{
		if (0 == msgField.getData (MSG_TEXT).value.strVal.compare (HELLO))
		{
			setPingStatus (nodeID, false);
		}

		url = url + "http://" + LOCALHOST + ':' + DEFAULT_COUCH_PORT + '/';
		url = url + TARGET_DB[ADMIN] + '/' + std::to_string (nodeID);

		if(CURLE_OK == curlRead(url, oss))
		{
			// Web page successfully written to string
			parser (oss.str());
			json = parser.getObject();
			oss.str("");

			//adminDBRevisions[nodeID] = (json.getData(REVISION)).value.strVal;

			data = json.getData(MESSAGE);
			data.value.array.push_back (entry);

			json.setValue (MESSAGE, data);

			curlPost (url, json.writeJSON(""));

			pushUpdates (ADMIN);
		}
		else
		{
			std::cout << "Error reading from DB\n";
		}
	}
}

/*******************************************************************************
 * Method:			checkMessages
 *
 * Description: Check the message field of the current node's CouchDB document
 * 							and handle the message accordingly if there is one.
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
virtual void dataExOnTheCouch::checkMessages ()
{
	std::string url = "";
	std::ostringstream oss;
	JSON json;
	jsonData data;
	jsonParser parser;
	Message returnMessage;

	url = url + "http://" + LOCALHOST + ':' + DEFAULT_COUCH_PORT + '/';
	url = url + TARGET_DB[ADMIN] + '/' + std::to_string (getID ());

	if(CURLE_OK == curlRead(url, oss))
	{
		// Web page successfully written to string
		parser (oss.str());
		json = parser.getObject();

		data = json.getData(MESSAGE);

		//read all of the messages out of the message queue and handle accordingly
		for (auto & entry : data.value.array)
		{
			switch ((entry.value.pObject->getData(MSG_TEXT)).value.strVal)
			{
				case HELLO:
					returnMessage.msg = ACK;
					returnMessage.dest.push_back (
							(entry.value.pObject->getData(MSG_SRC)).value.intVal);

					ping (returnMessage);
					break;

				case GOODBYE:
					nodeIPAddr.erase (nodeIPAddr.find (
							(entry.value.pObject->getData(MSG_SRC)).value.intVal));

					nodeDBRevisions.erase (nodeDBRevisions.find (
							(entry.value.pObject->getData(MSG_SRC)).value.intVal));

					dropNode ((entry.value.pObject->getData(MSG_SRC)).value.intVal);
					break;

				case STOP:
					setIsAlive (false);
					break;

				case START:
					setIsAlive (true);
					break;

				case ACK:
					setPingStatus (
							(entry.value.pObject->getData(MSG_SRC)).value.intVal, true);
					break;
			}
		}

		//clear the message queue
		data.value.array.clear ();
		json.setValue (MESSAGE, data);

		curlPost (url, json.writeJSON(""));

		//update messages
		pushUpdates (ADMIN);
	}

}

/*******************************************************************************
 * Method:			pushUpdates
 *
 * Description:	Synchronize the current node's version of the databases with the
 * 							other nodes in the network. Depending on the flag, different
 * 							databases or documents will be shared.
 *
 * Parameters:	flag - an int that is used to influence the behavior of the
 * 										 function
 *
 * Returned:		None
 ******************************************************************************/
virtual void dataExOnTheCouch::pushUpdates (int flag)
{
	JSON json;
	jsonData data, entry;


	for (auto & host : nodeIPAddr)
	{
		data.type = jsonParser::STR_TYPE;
		data.value.strVal.clear ();
		data.value.strVal.append ("http://");
		data.value.strVal.append (host.second);
		data.value.strVal.push_back (':');
		data.value.strVal.append (std::to_string (DEFAULT_COUCH_PORT));
		data.value.strVal.push_back ('/');

		switch (flag)
		{
			/*
			 * We'll need to update each document in each db. This should happen
			 * very rarely, so we don't need to worry about conflicts
			 */
			case ADMIN:
				data.value.strVal.append (TARGET_DB [ADMIN]);

				json.setValue (TARGET, data);

				data.value.strVal.clear ();
				data.value.strVal = TARGET_DB [ADMIN];

				json.setValue (SOURCE, data);
				break;

			/*
			 * We only change our own node information in each db instance
			 */
			case NODES:
				updateCouchFromNode ();

				data.value.strVal.append (TARGET_DB [NODES]);

				json.setValue (TARGET, data);

				data.value.strVal.clear ();
				data.value.strVal = TARGET_DB [NODES];

				json.setValue (SOURCE, data);

				entry.type = jsonParser::STR_TYPE;
				entry.value.strVal = std::to_string (getID ());
				data.type = jsonParser::VEC_TYPE;
				data.value.array.push_back(entry);

				json.setValue (DOC_IDS, data);
				break;

			/*
			 * Since each node can only update a particular device document,
			 * we can safely replicate the whole database without conflicts.
			 */
			case DEVICES:
				updateCouchFromDevs ();

				data.value.strVal.append (TARGET_DB [DEVICES]);

				json.setValue (TARGET, data);

				data.value.strVal.clear ();
				data.value.strVal = TARGET_DB [DEVICES];

				json.setValue (SOURCE, data);
				break;

			default:
				break;
		}


		curlPost ('/' + REPLICATE, json.writeJSON(""));
	}

}

/*******************************************************************************
 * Method:			pullUpdates
 *
 * Description: Update the current node's databases based on the flag. When we
 * 							pull updates we want to update all of the documents in every
 * 							database, but we don't necessarily care where the data comes
 * 							from. This leads to two results:
 * 								1) The databases are eventually consistent
 * 								2) We need to make sure that we don't pull from the same node
 * 								   each time we want to update.
 * 							The second result we solve by choosing a random node to pull
 * 							from each time we want to pull updates.
 *
 * Parameters:	flag - an int that influences how we pull data from other nodes
 *
 * Returned:		None
 ******************************************************************************/
virtual void dataExOnTheCouch::pullUpdates (int flag)
{
	JSON json;
	jsonData data;
	std::map<int, std::string>::iterator host;
	do
	{
		host = nodeIPAddr.begin ();
		std::advance (host, (rand() % nodeIPAddr.size ()));
	} while (getID () == host->first);


	data.type = jsonParser::STR_TYPE;
	data.value.strVal.clear ();
	data.value.strVal.append ("http://");
	data.value.strVal.append (host->second);
	data.value.strVal.push_back (':');
	data.value.strVal.append (std::to_string (DEFAULT_COUCH_PORT));
	data.value.strVal.push_back ('/');

	switch (flag)
	{
		case ADMIN:
			data.value.strVal.append (TARGET_DB [ADMIN]);

			json.setValue (SOURCE, data);

			data.value.strVal.clear ();
			data.value.strVal = TARGET_DB [ADMIN];

			json.setValue (TARGET, data);
			break;

		case NODES:
			data.value.strVal.append (TARGET_DB [NODES]);

			json.setValue (SOURCE, data);

			data.value.strVal.clear ();
			data.value.strVal = TARGET_DB [NODES];

			json.setValue (TARGET, data);
			break;

		case DEVICES:
			data.value.strVal.append (TARGET_DB [DEVICES]);

			json.setValue (SOURCE, data);

			data.value.strVal.clear ();
			data.value.strVal = TARGET_DB [DEVICES];

			json.setValue (TARGET, data);
			break;
	}

	curlPost ('/' + REPLICATE, json.writeJSON(""));

	switch (flag)
	{
		case NODES:
			updateNodesFromCouch ();
			break;
		case DEVICES:
			updateDevsFromCouch ();
			break;
	}

}

/*******************************************************************************
 * Method:			updateNodesFromCouch
 *
 * Description:	Update the node objects from documents in CouchDB
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void dataExOnTheCouch::updateNodesFromCouch ()
{
	std::string url = "";
	std::ostringstream oss;
	JSON json;
	jsonData data;
	jsonParser parser;
	location loc;
	distMeasurement dist;

	for (auto & thisNode : nodes)
	{
		url = url + "http://" + LOCALHOST + ':' + DEFAULT_COUCH_PORT + '/';
		url = url + TARGET_DB[NODES] + '/' + std::to_string (thisNode.first);

		if (CURLE_OK == curlRead(url, oss))
		{
			// Web page successfully written to string
			parser (oss.str ());
			json = parser.getObject ();

			//store revision id
			nodeDBRevisions[thisNode.first] = (json.getData (REVISION)).value.strVal;

			data = json.getData (LOCATION);
			loc.theID.intID = thisNode.first;
			loc.x = data.value.pObject->getData (X_COOR).value.floatVal;
			loc.y = data.value.pObject->getData (Y_COOR).value.floatVal;

			thisNode.second.setLocation (loc);

			data = json.getData (MEASUREMENTS);

			for (auto & entry : data.value.array)
			{
				dist.devID = entry.value.pObject->getData (ID).value.strVal;
				dist.dist = entry.value.pObject->getData (DISTANCE).value.floatVal;

				thisNode.second.setMeasurement (dist);
			}

		}
	}

}

/*******************************************************************************
 * Method:			updateDevsFromCouch
 *
 * Description:	Update the device objects from documents in CouchDB
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void dataExOnTheCouch::updateDevsFromCouch ()
{
	std::string url = "";
	std::ostringstream oss;
	JSON json;
	jsonData data;
	jsonParser parser;
	location loc;
	distMeasurement dist;

	for (auto & thisDev : devices)
	{
		url = url + "http://" + LOCALHOST + ':' + DEFAULT_COUCH_PORT + '/';
		url = url + TARGET_DB[DEVICES] + '/' + std::to_string (thisDev.first);

		if (CURLE_OK == curlRead(url, oss))
		{
			// Web page successfully written to string
			parser (oss.str ());
			json = parser.getObject ();

			//store revision id
			devDBRevisions[thisDev.first] = (json.getData (REVISION)).value.strVal;

			data = json.getData (LOCATION);
			loc.theID.strID = thisDev.first;
			loc.x = data.value.pObject->getData (X_COOR).value.floatVal;
			loc.y = data.value.pObject->getData (Y_COOR).value.floatVal;

			thisDev.second.setLocation (loc);

		}
	}
}

/*******************************************************************************
 * Method:			updateCouchFromNodes
 *
 * Description:	Update the document in node_db corresponding to the current
 * 							node. We will use the revision id we saved earlier.
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void dataExOnTheCouch::updateCouchFromNode ()
{
	std::string url = "";
	JSON json, jsonLoc, jsonDist;
	jsonData data, entry;
	location loc;
	std::vector<refMeasurement> nodeMeasurements;

	url = url + "http://" + LOCALHOST + ':' + DEFAULT_COUCH_PORT + '/';
	url = url + TARGET_DB[DEVICES] + '/' + std::to_string (getID ());

	//set the ID
	data.type = jsonParser::STR_TYPE;
	data.value.strVal = std::to_string (getID ());
	json.setValue (ID, data);

	//use the revision ID from the last read
	data.value.strVal = nodeDBRevisions[getID ()];
	json.setValue (REVISION, data);

	//set the location
	loc = nodes[getID ()].getLocation ();
	entry.type = jsonParser::FLT_TYPE;
	entry.value.floatVal = loc.x;
	jsonLoc.setValue (X_COOR, entry);
	entry.value.floatVal = loc.y;
	jsonLoc.setValue (Y_COOR, entry);

	data.type = jsonParser::OBJ_TYPE;
	data.value.pObject = &jsonLoc;
	json.setValue (LOCATION, data);

	//now start building up an array of device measurements because we don't know
	//what is different between this and CouchDB we will just update everything
	nodeMeasurements = nodes[getID ()].getAllMeasurements ();
	data.type = jsonParser::VEC_TYPE;

	for (auto & distEntry : nodeMeasurements)
	{
		entry.type = jsonParser::STR_TYPE;
		entry.value.strVal = distEntry.devDist.devID;

		jsonDist.setValue (ID, entry);

		entry.type = jsonParser::FLT_TYPE;
		entry.value.floatVal = distEntry.devDist.dist;

		jsonDist.setValue (DISTANCE, entry);

		entry.type = jsonParser::OBJ_TYPE;
		entry.value.pObject = &jsonDist;

		data.value.array.push_back (entry);
	}

	curlPost (url, json.writeJSON(""));

}

/*******************************************************************************
 * Method:			updateCouchFromDevs
 *
 * Description:	Updates the documents in dev_db for all of the devices that
 * 							this node updated since the last push.
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void dataExOnTheCouch::updateCouchFromDevs ()
{
	//use stored revision id
	std::string url = "";
	JSON json, jsonLoc;
	jsonData data, entry;
	location loc;
	std::vector<refMeasurement> nodeMeasurements;

	for (auto & thisDev : devsUpdatedSinceLastPush)
	{
		url = url + "http://" + LOCALHOST + ':' + DEFAULT_COUCH_PORT + '/';
		url = url + TARGET_DB[DEVICES] + '/' + thisDev.getID();

		data.type = jsonParser::STR_TYPE;
		data.value.strVal = thisDev.getID ();
		json.setValue (ID, data);

		//use the revision ID from the last read
		data.value.strVal = devDBRevisions[thisDev.getID ()];
		json.setValue (REVISION, data);

		//set the location
		loc = devices[thisDev.getID ()].getLocation ();
		entry.type = jsonParser::FLT_TYPE;
		entry.value.floatVal = loc.x;
		jsonLoc.setValue (X_COOR, entry);
		entry.value.floatVal = loc.y;
		jsonLoc.setValue (Y_COOR, entry);

		data.type = jsonParser::OBJ_TYPE;
		data.value.pObject = &jsonLoc;
		json.setValue (LOCATION, data);

		curlPost (url, json.writeJSON(""));
	}

	//reset
	devsUpdatedSinceLastPush.clear ();

}

/*******************************************************************************
 * Method:			dataWrite
 *
 * Description: A write call-back function used by curl_easy_setup on curlRead
 *
 * Parameters:	buf - the buffer of things to write
 * 							size - the size of the items in the buffer
 * 							nmemb - the number of items in the buffer
 * 							userp - the stream to which we want to write
 *
 * Returned:		size_t - the size of the stream written
 ******************************************************************************/
static size_t dataWrite (void* buf, size_t size, size_t nmemb, void* userp)
{
	if (userp)
	{
		std::ostream& os = *static_cast <std::ostream*> (userp);
		std::streamsize len = size * nmemb;
		if (os.write (static_cast <char*> (buf), len))
		{
			return len;
		}
	}

	return 0;
}

/*******************************************************************************
 * Method:			curlRead
 *
 * Description:	Reads the contents located at the url into the output stream.
 *
 * Parameters:	url - the page contents we are grabbing
 * 							os - the output stream we are sending the data to
 * 							timeout - the time we are willing to wait for our things
 *
 * Returned:		CURLcode - the result of running the read command
 ******************************************************************************/
CURLcode dataExOnTheCouch::curlRead (const std::string& url, std::ostream& os,
																		 long timeout = 30)
{
	CURLcode code (CURLE_FAILED_INIT);
	CURL* curl = curl_easy_init ();

	if (curl)
	{
		if (CURLE_OK == (code =
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &dataWrite))
				&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L))
				&& CURLE_OK == (code =
						curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L))
				&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FILE, &os))
				&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout))
				&& CURLE_OK
						== (code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str())))
		{
			code = curl_easy_perform (curl);
		}
		curl_easy_cleanup (curl);
	}
	return code;
}

/*******************************************************************************
 * Method:			curlPost
 *
 * Description:	Post the given json text to the given url.
 *
 * Parameters:	url - the url to which we are sending our data
 * 							json - the json we want posted
 *
 * Returned:		CURLcode - the result of running the post command
 ******************************************************************************/
CURLcode dataExOnTheCouch::curlPost(const std::string& url,
																		const std::string& json)
{
	CURLcode code (CURLE_FAILED_INIT);
	CURL* curl = curl_easy_init ();
	struct curl_slist* headers = NULL;
	headers = curl_slist_append (headers, "Content-Type: application/json");

	if (curl)
	{
		if (CURLE_OK
				== (code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json))
				&& CURLE_OK == (code =
				curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers))
				&& CURLE_OK
						== (code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str())))
		{
			code = curl_easy_perform (curl);
		}
		curl_easy_cleanup (curl);
	}

	curl_slist_free_all (headers);

	return code;

}

