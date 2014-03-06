/*******************************************************************************
 File:				dataExOnTheCouch.cpp
 Author: 		Josh Siva
 Date:				3/5/14
 Project:		NachoNet
 Purpose:		Implements the behavior of the CouchDB data exchange module. This
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
 * Method:			pushUpdates
 *
 * Description:	Synchronize the current node's version of the databases with the
 * 							other nodes in the network. Depending on the flag, different
 * 							databases or documents will be shared.
 *
 * Parameters:
 *
 * Returned:
 ******************************************************************************/
virtual void dataExOnTheCouch::pushUpdates (int flag)
{
	JSON json;
	jsonData data;
	switch (flag)
	{
		case ADMIN:
			for (auto & entry : nodeIPAddr)
			{
				data.type = jsonParser::STR_TYPE;
				data.value.strVal.clear ();
				data.value.strVal.append ("http://");
				data.value.strVal.append (std::to_string (entry.second));
				data.value.strVal.push_back (':');
				data.value.strVal.append (std::to_string (DEFAULT_COUCH_PORT));
				data.value.strVal.push_back ('/');
				data.value.strVal.append (TARGET_DB [ADMIN]);

				json.setValue (TARGET, data);

				data.value.strVal.clear ();
				data.value.strVal = TARGET_DB [ADMIN];

				json.setValue (SOURCE, data);

				curlPost ('/' + REPLICATE, json.writeJSON(""));
			}

			break;

		case NODES:

			break;

		case DEVICES:
			break;

		case ALL:
			break;

		default:
			break;
	}
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
virtual void dataExOnTheCouch::pullUpdates (int flag)
{
	switch (flag)
	{
		case ADMIN:
			break;

		case NODES:
			break;

		case DEVICES:
			break;

		case ALL:
			break;

		default:
			break;
	}
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

