/*******************************************************************************
File:				MapOnTheCouch.cpp
Author: 		Josh Siva
Date:				3/25/14
Project:		NachoNet
Purpose:		Implements the Map object on the couch
*******************************************************************************/

#include "../../include/map/MapOnTheCouch.h"

const std::string MapOnTheCouch::RES_LOCATION =
		"127.0.0.1:5984/map/resource";
const std::string MapOnTheCouch::FILE_NAME = "fileName";
const std::string MapOnTheCouch::IMG_DIM = "imgDimensions";
const std::string MapOnTheCouch::MAX_DIM = "maxDimensions";
const std::string MapOnTheCouch::X = "x";
const std::string MapOnTheCouch::Y = "y";
const std::string MapOnTheCouch::WIDTH = "width";
const std::string MapOnTheCouch::HEIGHT = "height";
const std::string MapOnTheCouch::REVISION ="_rev";

void MapOnTheCouch::save ()
{
	std::ostringstream oss;
	std::string url = RES_LOCATION;
	JSON resource, maxDim, imgDim;
	jsonParser parser;
	jsonData data, objData;
	int width = 0, height = 0;
	FILE *pFile = NULL;

	pFile = fopen (getMapFileName ().c_str (), "rb");

	if (NULL == pFile)
	{
		std::cout << "Error opening file.\n";
	}
	else
	{
		std::cout << "Opened file. Processing...\n";

		if (!getImageSize (pFile, &width, &height))
		{
			std::cout << "Error getting image size.\n";
		}

		std::cout << "Processed. Getting data from couch...\n";

		if (CURLE_OK == curlRead (RES_LOCATION, oss))
		{
			// Web page successfully written to string
			parser.init (oss.str ());
			resource = parser.getObject ();

			data.type = jsonParser::STR_TYPE;
			data.value.strVal = getMapFileName ();
			resource.setValue (FILE_NAME, data);

			data.type = jsonParser::FLT_TYPE;
			data.value.floatVal = getMaxX ();
			maxDim.setValue (X, data);

			data.value.floatVal = getMaxY ();
			maxDim.setValue (Y, data);

			objData.type = jsonParser::OBJ_TYPE;
			objData.value.pObject = &maxDim;
			resource.setValue (MAX_DIM, objData);

			data.type = jsonParser::INT_TYPE;
			data.value.intVal = width;
			imgDim.setValue (WIDTH, data);

			data.value.intVal = height;
			imgDim.setValue (HEIGHT, data);

			objData.type = jsonParser::OBJ_TYPE;
			objData.value.pObject = &imgDim;
			resource.setValue (IMG_DIM, objData);

			std::cout << "Data retrieved. Pushing updates to couch...\n";

			curlPutJSON (RES_LOCATION, resource.writeJSON (""));

			//we have to specify the which revision of the doc to which we
			//want to attach the image
			url += "/image?rev=";
			url += resource.getData (REVISION).value.strVal;

			curlPutImage (url);

			std::cout << "Opeation complete.\n";
		}

		fclose (pFile);
	}
}

void MapOnTheCouch::load ()
{
	std::ostringstream oss;
	JSON json;
	jsonParser parser;
	jsonData data;
	float x, y;

	if (CURLE_OK == curlRead (RES_LOCATION, oss))
	{
		// Web page successfully written to string
		parser.init (oss.str ());
		json = parser.getObject ();

		setMapFile (json.getData (FILE_NAME).value.strVal);
		x = json.getData (MAX_DIM).value.pObject->getData (X).value.floatVal;
		y = json.getData (MAX_DIM).value.pObject->getData (Y).value.floatVal;

		setMaxDimensions (x,y);
	}
}



/*******************************************************************************
 * Function:		readByte
 *
 * Description: Read a byte from a file. Based on the routine at:
 * 							http://carnage-melon.tom7.org/stuff/jpegsize.html
 *
 * 							"This routine is as public domain as is legally allowed."
 *
 * Parameters:	ch - a byte of data
 * 							pFile - the file we're reading from
 *
 * Returned:		None
 ******************************************************************************/
static void readByte (char ch, FILE * pFile)
{
	((ch) = getc ((pFile)));
}

/*******************************************************************************
 * Function:		readWord
 *
 * Description: Read a word from a file. Based on the routine at:
 * 							http://carnage-melon.tom7.org/stuff/jpegsize.html
 *
 * 							"This routine is as public domain as is legally allowed."
 *
 * Parameters:	ch - a byte of data
 * 							pFile - the file we're reading from
 *
 * Returned:		None
 ******************************************************************************/
static void readWord (char ch, FILE *pFile)
{
	int c = 0, d = 0;
	if ((c = getc ((pFile))) == EOF || (d = getc ((pFile))) == EOF)
	{
	}
	else
	{
		(ch) = (c << 8) + (d);
	}
}

/*******************************************************************************
 * Method:			getImageSize
 *
 * Description: Get the dimensions of a .jpeg image. Based on the routine at:
 * 							http://carnage-melon.tom7.org/stuff/jpegsize.html
 *
 * 							"This routine is as public domain as is legally allowed."
 *
 * Parameters:	pWidth - a pointer to int that holds the width of the image
 * 							pHeight - pointer to int that holds the height of the image
 *
 * Returned:		"Returns 0 on failure, nonzero on success"
 ******************************************************************************/
int MapOnTheCouch::getImageSize (FILE * pFile, int * pWidth, int * pHeight)
{
	int marker = 0;
	int dummy = 0;

	if (getc (pFile) != 0xFF || getc (pFile) != 0xD8)
	{
		return 0;
	}

	while (true)
	{
		std::cout << marker << " ";

		int discardedBytes = 0;
		readByte (marker, pFile);
		while (marker != 0xFF)
		{
			discardedBytes++;
			readByte (marker, pFile);
		}

		do
		{
			readByte (marker, pFile);
		} while (0xFF == marker);

		if (0 != discardedBytes)
		{
			return 0;
		}

		switch (marker)
		{
			case 0xC0:
			case 0xC1:
			case 0xC2:
			case 0xC3:
			case 0xC5:
			case 0xC6:
			case 0xC7:
			case 0xC9:
			case 0xCA:
			case 0xCB:
			case 0xCD:
			case 0xCE:
			case 0xCF:
			{
				readWord (dummy, pFile);	/* usual parameter length count */
				readByte (dummy, pFile);
				readWord ((*pHeight), pFile);
				readWord ((*pWidth), pFile);
				readByte (dummy, pFile);

				return 1;
				break;
			}

			case 0xDA:
			case 0xD9:
				return 0;

			default:
			{
				int length;

				readWord (length, pFile);

				if (length < 2)
				{
					return 0;
				}

				length -= 2;
				while (length > 0)
				{
					readByte (dummy, pFile);
					length--;
				}
			}
			break;
		}
		std::cout << "(" << *pWidth << ", " << *pHeight << ")\n";
	}

	return 1; //to hide warning. this is unreachable

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
CURLcode MapOnTheCouch::curlRead (const std::string& url, std::ostream& os,
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
 * Method:			curlPutImage
 *
 * Description:	Put the image located at the fileName at the indicated url
 *
 * Parameters:	url - the url to which we are sending our data
 *
 *
 * Returned:		CURLcode - the result of running the post command
 ******************************************************************************/
CURLcode MapOnTheCouch::curlPutImage(const std::string& url)
{
	CURLcode code (CURLE_FAILED_INIT);
	CURL* curl = curl_easy_init ();
	struct curl_slist* headers = NULL;
	FILE *pFile = NULL;
	headers = curl_slist_append (headers, "Content-Type: image/jpeg");

	pFile = fopen (getMapFileName ().c_str (), "wb");

	if (NULL == pFile)
	{
		std::cout << "Error opening file\n";
	}
	else
	{
		if (curl)
		{
			if (CURLE_OK
						== (code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, pFile))
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

		fclose (pFile);
	}

	return code;

}

/*******************************************************************************
 * Method:			curlPutJSON
 *
 * Description:	Put the given json text to the given url.
 *
 * Parameters:	url - the url to which we are sending our data
 * 							json - the json we want posted
 *
 * Returned:		CURLcode - the result of running the post command
 ******************************************************************************/
CURLcode MapOnTheCouch::curlPutJSON(const std::string& url,
																		const std::string& json)
{
	CURLcode code (CURLE_FAILED_INIT);
	CURL* curl = curl_easy_init ();
	struct curl_slist* headers = NULL;
	headers = curl_slist_append (headers, "Content-Type: application/json");

	if (curl)
	{
		if (CURLE_OK
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

