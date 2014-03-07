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
	 "message" : {"msg" : message, "src" : ipAddr}
 }
 *******************************************************************************/

#include "../../include/exch/dataExOnTheCouch.h"

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
 * Method:			setIP
 *
 * Description:	Set the IP address of the current node
 *
 * Parameters:	newIP - the new IP address
 *
 * Returned:		None
 ******************************************************************************/
void dataExOnTheCouch::setIP (ip newIP)
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
ip dataExOnTheCouch::getIP () const
{
	return this->myIP;
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
	jsonData data;
	jsonParser parser;

	data.type = jsonParser::STR_TYPE;
	data.value.strVal = message.msg;
	msgField.setValue (MSG_TEXT, data);

	data.value.strVal = std::to_string (myIP.addr);
	msgField.setValue (MSG_SRC, data);

	data.type = jsonParser::OBJ_TYPE;
	data.value.pObject = &msgField;

	for (int nodeID : message.dest)
	{
		url = url + "http://" + LOCALHOST + ':' + DEFAULT_COUCH_PORT + '/';
		url = url + TARGET_DB[ADMIN] + '/' + std::to_string (nodeID);

		if(CURLE_OK == curl_read(url, oss))
		{
			// Web page successfully written to string
			parser (oss.str());
			json = parser.getObject();
			oss.str("");

			//adminDBRevisions[nodeID] = (json.getData(REVISION)).value.strVal;

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

	url = url + "http://" + LOCALHOST + ':' + DEFAULT_COUCH_PORT + '/';
	url = url + TARGET_DB[ADMIN] + '/' + std::to_string (nodeID);

	if(CURLE_OK == curl_read(url, oss))
	{
		// Web page successfully written to string
		parser (oss.str());
		json = parser.getObject();

		data = json.getData(MESSAGE);

		switch (data.value.pObject->getData(MSG_TEXT))
		{
			case HELLO:
				//send ack then clear message
				break;

			case GOODBYE:
				//remove node from map
				break;

			case STOP:
				setIsAlive (false);
				//clear message field
				break;

			case START:
				setIsAlive (true);
				//clear message field
				break;
		}
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
		data.value.strVal.append (std::to_string (host.second));
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
				data.value.strVal.append (TARGET_DB [NODES]);

				json.setValue (TARGET, data);

				data.value.strVal.clear ();
				data.value.strVal = TARGET_DB [NODES];

				json.setValue (SOURCE, data);

				entry.type = jsonParser::STR_TYPE;
				entry.value.strVal = std::to_string (getID ());
				data.type = jsonParser::VEC_TYPE;
				entry.value.array.push_back(entry);

				json.setValue (DOC_IDS, data);
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
	std::map<int, ip>::iterator host = nodeIPAddr.begin ();
	std::advance (host, (rand() % nodeIPAddr.size ()));

	data.type = jsonParser::STR_TYPE;
	data.value.strVal.clear ();
	data.value.strVal.append ("http://");
	data.value.strVal.append (std::to_string (host->second));
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
	//store revision id
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
void dataExOnTheCouch::updateDevsFromCouch ()
{
	//store revision id
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
	//use stored revision id
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
void dataExOnTheCouch::updateCouchFromDevs ()
{
	//use stored revision id
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
static size_t data_write (void* buf, size_t size, size_t nmemb, void* userp)
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
 * Method:
 *
 * Description:
 *
 * Parameters:
 *
 * Returned:
 ******************************************************************************/
CURLcode dataExOnTheCouch::curlRead (const std::string& url, std::ostream& os,
																		 long timeout = 30)
{
	CURLcode code (CURLE_FAILED_INIT);
	CURL* curl = curl_easy_init ();

	if (curl)
	{
		if (CURLE_OK == (code =
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_write))
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
 * Method:
 *
 * Description:
 *
 * Parameters:
 *
 * Returned:
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

