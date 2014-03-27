/*******************************************************************************
File:				Configure.cpp
Author: 		Josh Siva
Date:				3/27/14
Project:		NachoNet
Purpose:		Initializes the environment to prepare for running NachoNet. This
						can include checking for CouchDB to be running, initializing the
						databases, and adding dummy data from a file to fill out the db.


						Dummy data format:
						{
							"nodes" : [{node 1 data}, {node 2 data}, ...],
							"admin" : [{}, {}, ..],
							"devices" : [{dev 1 data}, {dev 2 data}, ...]
						}
*******************************************************************************/

#include "../../include/util/jsonParser.h"
#include "../../include/util/json.h"
#include <unistd.h>
#include <getopt.h>
#include <iostream>
#include <curl/curl.h>
#include <sstream>
#include <fstream>

const long TIMEOUT = 30;
const std::string NODES = "nodes";
const std::string ADMIN = "admin";
const std::string DEVS = "devices";

/*******************************************************************************
 * Function:		dataWrite
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
 * Function:		curlRead
 *
 * Description:	Reads the contents located at the url into the output stream.
 *
 * Parameters:	url - the page contents we are grabbing
 * 							os - the output stream we are sending the data to
 * 							timeout - the time we are willing to wait for our things
 *
 * Returned:		CURLcode - the result of running the read command
 ******************************************************************************/
static CURLcode curlRead (const std::string& url, std::ostream& os,
																		 long timeout)
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
 * Function:		curlPost
 *
 * Description:	Post the given json text to the given url.
 *
 * Parameters:	url - the url to which we are sending our data
 * 							json - the json we want posted
 *
 * Returned:		CURLcode - the result of running the post command
 ******************************************************************************/
static CURLcode curlPost(const std::string& url,
												 const std::string& json,
												 std::ostream& os)
{
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

bool checkForDB (std::string dbName, std::ostringstream &oss)
{
	JSON json;
	jsonParser * pParser = NULL;
	bool found = false;

	std::string url;
	url = "http://127.0.0.1:5984/" + dbName;
	if (CURLE_OK == curlRead (url, oss, TIMEOUT))
	{
		pParser = new jsonParser  (oss.str ());
		json = pParser->getObject();
		oss.str (std::string ());
		oss.clear ();
		delete pParser;

		if (0 != json.getData ("error").value.strVal.compare ("not_found"))
		{
			std::cout << "   Found " << dbName << "\n";
			found = true;
		}
	}

	return found;
}

void deleteDB (std::string dbName, std::ostream& os)
{
	CURLcode code (CURLE_FAILED_INIT);
	CURL* curl = curl_easy_init ();
	std::string url;
	url = "http://127.0.0.1:5984/" + dbName;

	std::cout << "   Deleting " << dbName << "...\n";

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
}

void createDB (std::string dbName, std::ostream& os)
{
	CURLcode code (CURLE_FAILED_INIT);
	CURL* curl = curl_easy_init ();
	std::string url;
	url = "http://127.0.0.1:5984/" + dbName;

	std::cout << "   Creating " << dbName << "...\n";

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

int main (int argc, char** argv)
{
	int optionChar;
	std::string fileName, url;
	std::ostringstream oss;
	std::ifstream inFile;
	JSON json, entry;
	jsonParser *pParser = NULL;
	bool newConfig = false, fill = false, databasesFound = false;
	char choice;

	while ((optionChar = getopt (argc, argv, "nf:")) != -1)
	{
		switch (optionChar)
		{
			case 'n':
				newConfig = true;
				break;

			case 'f':
				fill = true;
				fileName = optarg;
				break;

			case '?':
				if ('f' == optopt)
				{
					std::cout << "Option -f requires an argument.\n";
				}
				break;
		}
	}

	std::cout << "Checking for CouchDB...\n";

	//check for CouchDB to be running
	if (CURLE_OK == curlRead ("http://127.0.0.1:5984/", oss, TIMEOUT))
	{
		pParser = new jsonParser  (oss.str ());
		json = pParser->getObject();
		oss.str (std::string ());
		oss.clear ();
		delete pParser;

		std::cout << "CouchDB is running!\n";

		if (0 == json.getData("couchdb").value.strVal.compare ("Welcome"))
		{
			if (newConfig)
			{
				//check for databases
				std::cout << "Checking for databases...\n";

				databasesFound = checkForDB ("node_db", oss);
				databasesFound = checkForDB ("dev_db", oss);
				databasesFound = checkForDB ("admin_db", oss);

				if (databasesFound)//if databases exist prompt user for confirmation
				{
					do
					{
						std::cout << "Databases detected. Really delete databases? ";
						std::cin >> choice;
					} while ('y' != choice && 'n' != choice);

					if ('y' == choice)
					{
						//delete databases
						deleteDB ("node_db", oss);
						deleteDB ("dev_db", oss);
						deleteDB ("admin_db", oss);
						std::cout << "Databases deleted\n";
					}
				}

				std::cout << "Creating databases\n";
				//put new databases
				createDB ("node_db", oss);
				createDB ("dev_db", oss);
				createDB ("admin_db", oss);

			}
			else
			{
				//if we aren't doing a fresh config then we're pretty much done after
				//we've checked for CouchDB to be running

				if (fill)
				{
					//open file and throw data into CouchDB
					inFile.open (fileName.c_str ());

					if (!inFile)
					{
						std::cout << "Could not open file\n";
					}
					else
					{
						std::string fileContents ((std::istreambuf_iterator<char>(inFile)),
																			 std::istreambuf_iterator<char>());

						pParser = new jsonParser (fileContents);
						json = pParser->getObject ();

						std::cout << "Adding docs to node_db...\n";
						for (auto & entry : json.getData (NODES).value.array)
						{
							url = "http://127.0.0.1:5984/node_db/";
							url.append (entry.value.pObject->getData("_id").value.strVal);
							curlPost (url, entry.value.pObject->writeJSON (""),
												oss);
							oss.str (std::string ());
							oss.clear ();
							url.clear ();
						}

						std::cout << "Adding docs to admin_db...\n";
						for (auto & entry : json.getData (ADMIN).value.array)
						{
							url = "http://127.0.0.1:5984/admin_db/";
							url.append (entry.value.pObject->getData("_id").value.strVal);
							curlPost (url, entry.value.pObject->writeJSON (""),
												oss);
							oss.str (std::string ());
							oss.clear ();
							url.clear ();
						}

						std::cout << "Adding docs to dev_db...\n";
						for (auto & entry : json.getData (DEVS).value.array)
						{
							url = "http://127.0.0.1:5984/dev_db/";
							url.append (entry.value.pObject->getData("_id").value.strVal);
							curlPost (url, entry.value.pObject->writeJSON (""),
												oss);
							oss.str (std::string ());
							oss.clear ();
							url.clear ();
						}

						std::cout << "Done.\n";


						delete pParser;
					}
				}
			}
		}
	}
	else
	{
		std::cout << "CouchDB is not running. Go start it.\n";
	}

	std::cout << "Configuration complete\n";


	return 0;
}


