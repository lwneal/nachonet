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

const double dataExOnTheCouch::TIMEOUT = 10;

const std::string dataExOnTheCouch::LOCALHOST = "127.0.0.1";

const std::string dataExOnTheCouch::TARGET_DB[] =
																						{"admin_db", "node_db", "dev_db"};

const std::string dataExOnTheCouch::ALL_DOCS_Q = "_all_docs";

//these are the keys for all of the JSON pairs we need to use
const std::string dataExOnTheCouch::LOCATION = "location";
const std::string dataExOnTheCouch::ID = "_id";
const std::string dataExOnTheCouch::ALT_ID = "id";
const std::string dataExOnTheCouch::IP = "ip";
const std::string dataExOnTheCouch::X_COOR = "x";
const std::string dataExOnTheCouch::Y_COOR = "y";
const std::string dataExOnTheCouch::MEASUREMENTS = "measurements";
const std::string dataExOnTheCouch::DEV_ID = "devID";
const std::string dataExOnTheCouch::DISTANCE = "dist";
const std::string dataExOnTheCouch::REVISION = "_rev";
const std::string dataExOnTheCouch::RESPONSE_REV = "rev";
const std::string dataExOnTheCouch::SOURCE = "source";
const std::string dataExOnTheCouch::TARGET = "target";
const std::string dataExOnTheCouch::REPLICATE = "_replicate";
const std::string dataExOnTheCouch::DOC_IDS = "doc_ids";
const std::string dataExOnTheCouch::STATE = "state";
const std::string dataExOnTheCouch::MESSAGE = "message";
const std::string dataExOnTheCouch::MSG_TEXT = "msg";
const std::string dataExOnTheCouch::MSG_SRC = "src";
const std::string dataExOnTheCouch::DELETED = "_deleted";
const std::string dataExOnTheCouch::TOTAL_ROWS = "total_rows";
const std::string dataExOnTheCouch::ROWS = "rows";

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
dataExOnTheCouch::dataExOnTheCouch ()
{
	struct ifaddrs * 		pIfaceAddr = NULL;
	struct ifaddrs * 		pIface = NULL;
	void * 							pTempAddr = NULL;
	char 								addrBuffer[INET_ADDRSTRLEN];
	std::string 				myAddress, url;
	std::ostringstream 	oss, response;
	JSON 								json, jsonLoc;
	jsonData 						data, entry;
	jsonParser 					parser;
	location 						loc;
		loc.x = 0;
		loc.y = 0;
		loc.theID.intID = node::NO_ID;
	node 								newNode (node::NO_ID, loc);
	Message 						checkResponse;

	curl_global_init (CURL_GLOBAL_ALL);

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

		}
	}

	if (NULL != pIfaceAddr)
	{
		freeifaddrs (pIfaceAddr);
	}

	pNachoCast = new multicast (multicast::DEFAULT_PORT, myAddress,
															multicast::MULTICAST_GROUP);
	setIP (myAddress);

	discover ();

	//add ourselves to the node maps
	newNode.setID (getID ());

	nodes [getID ()] = newNode;
	nodeIPAddr [getID ()] = getIP ();

	//now add our docs to the database
	json.clear();
	data.type = jsonParser::STR_TYPE;
	data.value.strVal = std::to_string(getID ());
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

	//std::cout << "Adding to admin_db: " << json.writeJSON ("") << "\n";

	curlPut (url, json.writeJSON(""), response);
	url.clear ();

	//std::cout << response.str () << "\n";

	json.clear ();

	//Share our updates with everyone else
	pushUpdates (ADMIN);
	pushUpdates (NODES); //we create our node doc here

	//now set up our greeter for future new nodes
	stillGreetingNodes = true;
	pGreeter = new std::thread (&dataExOnTheCouch::greetNewNode,
															this);

	setIsAlive (false);

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
dataExOnTheCouch::~dataExOnTheCouch ()
{
	Message message;
	std::string url;
	std::ostringstream oss, response;
	JSON json;
	jsonData data;
	jsonParser *pParser;

	message.msg = GOODBYE;

	for (auto & entry : nodeIPAddr)
	{
		if (getID () != entry.first)
		{
			message.dest.push_back (entry.first);
		}
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
		pParser = new jsonParser (oss.str());
		json = pParser->getObject ();
		oss.str ("");

		data.type = jsonParser::BOOL_TYPE;
		data.value.boolVal = true;
		json.setValue (DELETED, data);

		curlPut (url, json.writeJSON(""), response);

		json.clear ();
		delete pParser;
	}
	url.clear ();

	url = "http://" + LOCALHOST + ':';
	url.append (std::to_string (DEFAULT_COUCH_PORT));
	url += '/' + TARGET_DB[NODES] + '/' + std::to_string(getID ());

	if (CURLE_OK == curlRead (url, oss))
	{
		// Web page successfully written to string
		pParser = new jsonParser (oss.str());
		json = pParser->getObject ();
		oss.str ("");

		data.type = jsonParser::BOOL_TYPE;
		data.value.boolVal = true;
		json.setValue (DELETED, data);

		curlPut (url, json.writeJSON(""), response);

		json.clear ();

		delete pParser;
	}

	url.clear ();

	pushUpdates (ADMIN);
	pushUpdates (NODES_END); //because we don't want the same update behavior
													 //We already set the doc to deleted, so we want to
													 //avoid updating from our local node data

	url = "http://" + LOCALHOST + ':';
	url.append (std::to_string (DEFAULT_COUCH_PORT));
	url += '/' + TARGET_DB[NODES];

	clearDB (url, oss);

	url.clear ();

	url = "http://" + LOCALHOST + ':';
	url.append (std::to_string (DEFAULT_COUCH_PORT));
	url += '/' + TARGET_DB[ADMIN];

	clearDB (url, oss);


	curl_global_cleanup ();

	stillGreetingNodes = false;

	pNachoCast->kill ();
	pGreeter->join ();
	delete pGreeter;

	delete pNachoCast;

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
	std::ostringstream response;
	std::string message, url;
	JSON json;
	jsonData data;
	int nextID = node::NO_ID;
	location loc;
	loc.x = 0;
	loc.y = 0;
	loc.theID.intID = node::NO_ID;
	node newNode (node::NO_ID, loc);


	while (stillGreetingNodes)
	{
		message = pNachoCast->receive ();

		//is the message more than just null terminators
		if (0 < message.size ())
		{
			//std::cout << "Heard from someone\n";
			//let's go find the next available ID for the new guy
			for (auto & entry : nodeIPAddr)
			{
				if (entry.first >= nextID)
				{
					nextID = entry.first + 1;
				}
			}

			//add this new node
			nodeIPAddr[nextID] = message;
			newNode.setID(nextID);
			nodes[nextID] = newNode;
			setPingStatus (nextID, true);

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

			url = "http://" + LOCALHOST + ':';
			url.append (std::to_string (DEFAULT_COUCH_PORT));
			url.append ('/' + REPLICATE);

			curlPost (url, json.writeJSON(""), response);//admin docs sent


			//std::cout << response.str () << "\n";
		}

		/*std::cout << "I know: ";

		for (auto & entry : this->nodeIPAddr)
		{
			std::cout << "(" << entry.first << ", " << entry.second << ")   ";
		}

		std::cout << "\n";*/
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
void dataExOnTheCouch::ping (Message message)
{
	std::string url = "";
	std::ostringstream oss, response;
	JSON json, msgField;
	jsonData data, entry;
	jsonParser parser;

	data.type = jsonParser::STR_TYPE;
	data.value.strVal = message.msg;
	msgField.setValue (MSG_TEXT, data);

	data.type = jsonParser::INT_TYPE;
	data.value.intVal = getID ();
	msgField.setValue (MSG_SRC, data);

	entry.type = jsonParser::OBJ_TYPE;
	entry.value.pObject = &msgField;

	for (int nodeID : message.dest)
	{

		//std::cout << msgField.getData (MSG_TEXT).value.strVal << " " << nodeID
								//<< "\n";

		if (0 == msgField.getData (MSG_TEXT).value.strVal.compare (HELLO))
		{
			setPingStatus (nodeID, false);
		}

		url += "http://" + LOCALHOST + ':' + std::to_string(DEFAULT_COUCH_PORT)
					+ '/' + TARGET_DB[ADMIN] + '/' + std::to_string (nodeID);

		//std::cout << url << "\n";

		if(CURLE_OK == curlRead(url, oss))
		{
			// Web page successfully written to string
			parser.init (oss.str());
			json = parser.getObject();
			oss.str("");

			data = json.getData(MESSAGE);
			data.value.array.push_back (entry);

			json.setValue (MESSAGE, data);

			curlPut (url, json.writeJSON(""), response);

			pushUpdates (ADMIN);

			data.value.array.clear (); // since json.clear assumes all objects are
			json.setValue (MESSAGE, data); //dynamic, we need to clear out the non-
																		 //dynamic objects

			json.clear ();
		}
		else
		{
			std::cout << "Error reading from DB\n";
		}

		url.clear ();
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
void dataExOnTheCouch::checkMessages ()
{
	std::string url = "", msg;
	std::ostringstream oss, response;
	JSON json;
	jsonData data;
	jsonParser parser;
	Message returnMessage;
	int src;

	url = url + "http://" + LOCALHOST + ':' + std::to_string (DEFAULT_COUCH_PORT)
				+ '/' + TARGET_DB[ADMIN] + '/' + std::to_string (getID ());

	if(CURLE_OK == curlRead(url, oss))
	{
		// Web page successfully written to string
		parser.init (oss.str());
		json = parser.getObject();

		data = json.getData(MESSAGE);

		//read all of the messages out of the message queue and handle accordingly
		for (auto & entry : data.value.array)
		{
			msg = entry.value.pObject->getData(MSG_TEXT).value.strVal;
			src = entry.value.pObject->getData(MSG_SRC).value.intVal;

			if (0 == msg.compare (HELLO))
			{
				if (nodeIPAddr.count (src))
				{
					returnMessage.msg = ACK;
					returnMessage.dest.push_back (
							(entry.value.pObject->getData(MSG_SRC)).value.intVal);

					ping (returnMessage);
				}
			}
			else if (0 ==	msg.compare (GOODBYE))
			{

				if (nodeIPAddr.count (src))
				{
					nodeIPAddr.erase (nodeIPAddr.find (src));
					dropNode (src);
				}

				if (nodeDBRevisions.count (src))
				{
					nodeDBRevisions.erase (nodeDBRevisions.find (src));
				}
			}
			else if (0 ==	msg.compare (STOP))
			{
				setIsAlive (false);
				setState (DEAD);
			}
			else if (0 == msg.compare (START))
			{
				setIsAlive (true);
				setState (RUNNING);
			}
			else if (0 == msg.compare (ACK))
			{
				if (nodeIPAddr.count (src))
				{
					setPingStatus (src, true);
				}
			}

			delete (entry.value.pObject);
			entry.value.pObject = NULL;
		}

		//clear the message queue
		data.value.array.clear ();
		json.setValue (MESSAGE, data);

		curlPut (url, json.writeJSON(""), response);

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
void dataExOnTheCouch::pushUpdates (int flag)
{
	JSON json;
	jsonData data, entry;
	std::ostringstream response;
	std::string url;


	//some simple processing before we go into the update loop
	switch (flag)
	{
		case NODES:
			entry.type = jsonParser::STR_TYPE;
			entry.value.strVal = std::to_string (getID ());
			data.type = jsonParser::VEC_TYPE;
			data.value.array.push_back(entry);

			json.setValue (DOC_IDS, data);

			updateCouchFromNode ();
			break;


		case DEVICES:
			updateCouchFromDevs ();
			break;
	}

	for (auto & host : nodeIPAddr)
	{

		if (getID () != host.first)
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

				case NODES:
				case NODES_END:

					data.value.strVal.append (TARGET_DB [NODES]);

					json.setValue (TARGET, data);

					data.value.strVal.clear ();
					data.value.strVal = TARGET_DB [NODES];

					json.setValue (SOURCE, data);
					break;

				/*
				 * Since each node can only update a particular device document,
				 * we can safely replicate the whole database without conflicts.
				 */
				case DEVICES:

					data.value.strVal.append (TARGET_DB [DEVICES]);

					json.setValue (TARGET, data);

					data.value.strVal.clear ();
					data.value.strVal = TARGET_DB [DEVICES];

					json.setValue (SOURCE, data);
					break;

				default:
					break;
			}


			url = "http://" + LOCALHOST + ':';
			url.append (std::to_string (DEFAULT_COUCH_PORT));
			url.append ('/' + REPLICATE);

			curlPost (url, json.writeJSON(""), response);
		}
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
void dataExOnTheCouch::pullUpdates (int flag)
{
	JSON json;
	jsonData data;
	std::map<int, std::string>::iterator host;
	std::ostringstream response;
	std::string url;

	if (!(1 == nodes.size () && getID () == nodes.begin ()->first))
	{

		do
		{
			host = nodeIPAddr.begin ();
			std::advance (host, (rand() % nodeIPAddr.size ()));
		} while (getID () == host->first || !lastPingResult (host->first));


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

		url = "http://" + LOCALHOST + ':';
		url.append (std::to_string (DEFAULT_COUCH_PORT));
		url.append ('/' + REPLICATE);

		curlPost (url, json.writeJSON(""), response);
	}

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
 * Method:			setState
 *
 * Description:	Set the state of this node in the admin database.
 *
 * Parameters:	state - either "RUNNING" or "DEAD"
 *
 * Returned:		None
 ******************************************************************************/
void dataExOnTheCouch::setState (std::string state)
{
	std::string url = "";
	std::ostringstream oss, response;
	JSON json;
	jsonData data;
	jsonParser parser;

	url = url + "http://" + LOCALHOST + ':' + std::to_string (DEFAULT_COUCH_PORT)
				+ '/' + TARGET_DB[ADMIN] + '/' + std::to_string (getID ());

	if(CURLE_OK == curlRead(url, oss))
	{
		// Web page successfully written to string
		parser.init (oss.str());
		json = parser.getObject();

		data.type = jsonParser::STR_TYPE;
		data.value.strVal = state;
		json.setValue (STATE, data);

		curlPut (url, json.writeJSON (""), response);
	}
}

/*******************************************************************************
 * Method:			discover
 *
 * Description:	Use multicast to tell other nodes in the network that I'm hear.
 * 							If we hear from someone we update our data from their DBs and
 * 							figure out what our ID is.
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void dataExOnTheCouch::discover ()
{
	std::ostringstream 	oss;
	std::string 				url, myAddress;
	jsonParser 		*			pParser = NULL;
	JSON 								json, jsonDoc;
	jsonData 						data, entry;
	location 						loc;
		loc.x = 0;
		loc.y = 0;
		loc.theID.intID = node::NO_ID;
	node 								newNode (node::NO_ID, loc);
	char 								message[multicast::BUF_LENGTH];
	int 								totalRows, nextID = node::NO_ID;

	myAddress = getIP ();

	//prepare our broadcast message
	for (int i = 0; i < multicast::BUF_LENGTH; i++)
	{
		message[i] = '\0';
	}

	memcpy (message, myAddress.c_str (), myAddress.length ());

	//broadcast our IP address to anybody listening
	pNachoCast->transmit (message);

	sleep (TIMEOUT); //wait for responses


	//now we need to check if anyone got back to us
	url = "http://" + LOCALHOST + ':';
	url.append (std::to_string (DEFAULT_COUCH_PORT));
	url += '/' + TARGET_DB[ADMIN] + '/' + ALL_DOCS_Q;

	if (CURLE_OK == curlRead (url, oss))
	{
		// Web page successfully written to string
		pParser = new jsonParser (oss.str());
		json = pParser->getObject ();
		oss.str ("");
		delete pParser;
		pParser = NULL;
	}

	totalRows = json.getData (TOTAL_ROWS).value.intVal;
	//we heard back so we need to go through admin_db and set up our local data
	//to prepare for the first full sync
	if (0 < totalRows)
	{
		std::cout << "Found other node(s). Updating...\n";

		data = json.getData (ROWS);

		for (int i = 0; i < totalRows; i++)
		{
			entry = data.value.array[i];

			url.clear ();
			url = "http://" + LOCALHOST + ':';
			url.append (std::to_string (DEFAULT_COUCH_PORT));
			url += '/' + TARGET_DB[ADMIN] + '/';
			url	+= entry.value.pObject->getData (ALT_ID).value.strVal;

			if (CURLE_OK == curlRead (url, oss))
			{
				// Web page successfully written to string
				pParser = new jsonParser (oss.str());
				jsonDoc = pParser->getObject ();
				oss.str ("");
				delete pParser;
				pParser = NULL;

				nodeIPAddr[atoi(jsonDoc.getData(ID).value.strVal.c_str ())]
									 = jsonDoc.getData(IP).value.strVal;

				nodes[atoi(jsonDoc.getData(ID).value.strVal.c_str ())]
									 = newNode;

				//figure out our ID
				if (atoi(jsonDoc.getData(ID).value.strVal.c_str ()) >= nextID)
				{
					nextID = atoi(jsonDoc.getData(ID).value.strVal.c_str ())
									 + 1;
				}

				setPingStatus (atoi(jsonDoc.getData(ID).value.strVal.c_str ()), true);

				jsonDoc.clear ();
			}
		}

		json.clear ();

		setID (nextID);

		pullUpdates (ADMIN);
		pullUpdates (NODES);
		pullUpdates (DEVICES);


	}
	else //if (TIMEOUT < duration) we didn't hear back so we are the first node
	{
		std::cout << "FiR57 p0As7!\n";
		setID (0);
	}

	if (NULL != pParser)
	{
		json.clear ();
		delete pParser;
	}

	/*std::cout << "I know: ";

	for (auto & entry : this->nodeIPAddr)
	{
		std::cout << "(" << entry.first << ", " << entry.second << ")   ";
	}

	std::cout << "\n";*/
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
	jsonParser * pParser;
	location loc;
	distMeasurement dist;

	for (auto & thisNode : nodes)
	{
		url = "http://" + LOCALHOST + ':'
					+ std::to_string (DEFAULT_COUCH_PORT) + '/'+ TARGET_DB[NODES] + '/'
					+ std::to_string (thisNode.first);

		if (CURLE_OK == curlRead(url, oss))
		{
			// Web page successfully written to string
			pParser = new jsonParser (oss.str ());
			json = pParser->getObject ();
			oss.str("");
			delete pParser;

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

			json.clear ();

		}

		url.clear ();

		url = "http://" + LOCALHOST + ':'
					+ std::to_string (DEFAULT_COUCH_PORT) + '/'+ TARGET_DB[ADMIN] + '/'
					+ std::to_string (thisNode.first);

		//we need to make sure that all nodes have the ip addresses of all other
		//nodes. Multicast uses UDP so it's possible that not everyone has seen
		//the new node
		if (CURLE_OK == curlRead(url, oss))
		{
			// Web page successfully written to string
			pParser = new jsonParser (oss.str ());
			json = pParser->getObject ();
			oss.str("");

			nodeIPAddr[atoi(json.getData(ID).value.strVal.c_str ())]
									 = json.getData(IP).value.strVal;

			json.clear ();
			delete pParser;

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
	jsonParser * pParser;
	location loc;
	distMeasurement dist;

	for (auto & thisDev : devices)
	{
		url = "http://" + LOCALHOST + ':'
					+ std::to_string (DEFAULT_COUCH_PORT) + '/' + TARGET_DB[DEVICES] + '/'
					+ thisDev.first;

		if (CURLE_OK == curlRead(url, oss))
		{
			// Web page successfully written to string
			pParser = new jsonParser (oss.str ());
			json = pParser->getObject ();
			oss.str("");

			//store revision id
			devDBRevisions[thisDev.first] = (json.getData (REVISION)).value.strVal;

			data = json.getData (LOCATION);
			loc.theID.strID = thisDev.first;
			loc.x = data.value.pObject->getData (X_COOR).value.floatVal;
			loc.y = data.value.pObject->getData (Y_COOR).value.floatVal;

			thisDev.second.setLocation (loc);
			json.clear ();
			delete pParser;

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
	std::ostringstream response;
	std::string url = "";
	JSON json, jsonLoc, *pJsonDist, jsonResponse;
	jsonParser parser;
	jsonData data, entry;
	location loc;
	std::vector<distMeasurement> measurements;

	url = "http://" + LOCALHOST + ':' + std::to_string (DEFAULT_COUCH_PORT)
				+ '/' + TARGET_DB[NODES] + '/' + std::to_string (getID ());

	//set the ID
	data.type = jsonParser::STR_TYPE;
	data.value.strVal = std::to_string (getID ());
	json.setValue (ID, data);

	//use the revision ID from the last read
	if (nodeDBRevisions.count (getID ()))
	{
		data.value.strVal = nodeDBRevisions[getID ()];
		json.setValue (REVISION, data);
	}

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
	measurements = nodes[getID ()].getAllBasicMeasurements ();
	data.type = jsonParser::VEC_TYPE;

	pJsonDist = new JSON[measurements.size ()];

	int i = 0;
	for (auto & distEntry : measurements)
	{
		entry.type = jsonParser::STR_TYPE;
		entry.value.strVal = distEntry.devID;

		pJsonDist[i].setValue (ID, entry);

		entry.type = jsonParser::FLT_TYPE;
		entry.value.floatVal = distEntry.dist;

		pJsonDist[i].setValue (DISTANCE, entry);

		entry.type = jsonParser::OBJ_TYPE;
		entry.value.pObject = &pJsonDist[i];

		data.value.array.push_back (entry);
		i++;
	}

	json.setValue (MEASUREMENTS, data);

	//std::cout << json.writeJSON ("") << "\n";
	curlPut (url, json.writeJSON(""), response);

	delete [] pJsonDist;

	parser.init (response.str ());
	jsonResponse = parser.getObject ();

	nodeDBRevisions[getID ()] = jsonResponse.getData (RESPONSE_REV).value.strVal;

	jsonResponse.clear ();

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
	std::ostringstream response;
	std::string url = "";
	JSON json, jsonLoc, jsonResponse;
	jsonParser parser;
	jsonData data, entry;
	location loc;
	std::vector<refMeasurement> nodeMeasurements;

	for (auto & thisDev : devsUpdatedSinceLastPush)
	{
		url = "http://" + LOCALHOST + ':' + std::to_string(DEFAULT_COUCH_PORT)
					+ '/' + TARGET_DB[DEVICES] + '/' + thisDev.getID();

		data.type = jsonParser::STR_TYPE;
		data.value.strVal = thisDev.getID ();
		json.setValue (ID, data);

		//use the revision ID from the last read

		if (devDBRevisions.count (thisDev.getID ()))
		{
			data.value.strVal = devDBRevisions[thisDev.getID ()];
			json.setValue (REVISION, data);
		}

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

		curlPut (url, json.writeJSON(""), response);

		parser.init (response.str ());
		jsonResponse = parser.getObject ();

		devDBRevisions[thisDev.getID ()] =
				jsonResponse.getData (RESPONSE_REV).value.strVal;

		jsonResponse.clear ();

		url.clear ();
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
																		 long timeout)
{
	CURLcode code (CURLE_FAILED_INIT);
	CURL* curl = curl_easy_init ();

	//std::cout << url << "\n";

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
 * Method:			curlPut
 *
 * Description:	Put the given json text to the given url.
 *
 * Parameters:	url - the url to which we are sending our data
 * 							json - the json we want posted
 *
 * Returned:		CURLcode - the result of running the post command
 ******************************************************************************/
CURLcode dataExOnTheCouch::curlPut(const std::string& url,
																		const std::string& json,
																		std::ostream& os)
{
	CURLcode code (CURLE_FAILED_INIT);
	CURL* curl = curl_easy_init ();
	struct curl_slist* headers = NULL;
	headers = curl_slist_append (headers, "Content-Type: application/json");

	//std::cout << url << "\n   " << json << "\n";
	if (curl)
	{
		if (CURLE_OK
					== (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &dataWrite))
				&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &os))
				&& CURLE_OK
					== (code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str()))
				&& CURLE_OK
					== (code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers))
				&& CURLE_OK
					== (code = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"))
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
																		const std::string& json,
																		std::ostream& os)
{

	//std::cout << url << "\n   " << json << "\n";
	CURLcode code (CURLE_FAILED_INIT);
	CURL* curl = curl_easy_init ();
	struct curl_slist* headers = NULL;
	headers = curl_slist_append (headers, "Content-Type: application/json");

	if (curl)
	{
		if (CURLE_OK
					== (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &dataWrite))
				&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &os))
				&& CURLE_OK
					== (code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str()))
				&& CURLE_OK
					== (code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers))
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

/*******************************************************************************
 * Method:			clearDB
 *
 * Description:	Deletes and recreates the database located at the URL. This is
 * 							intended to be used in the destroyer!
 *
 * Parameters:	url - the url at which the database is located
 * 							os - output stream for capturing return information (can be used
 * 									 for debugging)
 *
 * Returned:		None
 ******************************************************************************/
void dataExOnTheCouch::clearDB (std::string url, std::ostream& os)
{
	CURLcode code (CURLE_FAILED_INIT);
	CURL* curl = curl_easy_init ();

	if (curl)
	{
		if (CURLE_OK
					== (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &dataWrite))
				&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &os))
				&& CURLE_OK
					== (code = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE"))
				&& CURLE_OK
						== (code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str())))
		{
			code = curl_easy_perform (curl);
		}
		curl_easy_cleanup (curl);
	}

	curl = curl_easy_init ();

	if (curl)
	{
		if (CURLE_OK
					== (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &dataWrite))
				&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &os))
				&& CURLE_OK
					== (code = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"))
				&& CURLE_OK
						== (code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str())))
		{
			code = curl_easy_perform (curl);
		}
		curl_easy_cleanup (curl);
	}
}
