/*******************************************************************************
File:				dataCollect.cpp
Author: 		Josh Siva
Date:				1/26/14
Project:		NachoNet
Purpose:		Implements the behavior of the data collection module
*******************************************************************************/

#include "../../include/collect/dataCollect.h"
#include <climits>
#include <algorithm>


/*******************************************************************************
 * Constructor: dataCollect
 *
 * Description: initializes the debug variable and the ready to read variable
 *
 * Parameters:	debug - true if in debug mode, false otherwise
 *
 * Returned: 		None
 ******************************************************************************/
dataCollect::dataCollect (bool debug)
{
	setDebug (debug);
	readyToRead = false;
}

/*******************************************************************************
 * Method: 			getSS
 *
 * Description: Returns a vector of ssMeasurements
 *
 * Parameters:	None
 *
 * Returned: 		Vector of ssMeasurements
 ******************************************************************************/
std::vector<ssMeasurement> dataCollect::getSS () const
{
	ssMeasurement temp;
	std::vector<ssMeasurement>  returnVector;
	std::vector<int> sort;


	for (auto &item : buffer)
	{
		temp.devID = item.first;

		for (int i = 0; i < CONTAINER_SIZE; i++)
		{
			sort.push_back((item.second).data[i]);
		}

		std::sort (sort.begin(), sort.end());

		temp.ss = sort[MEDIAN];

		returnVector.push_back(temp);
	}

	return returnVector;
}

/*******************************************************************************
 * Method: 			setDebug
 *
 * Description: Sets the debug member variable
 *
 * Parameters:	debug - true if in debug mode, false otherwise
 *
 * Returned: 		None
 ******************************************************************************/
void dataCollect::setDebug (bool debug)
{
	this->debug = debug;
}

/*******************************************************************************
 * Method: 			setReadyToRead
 *
 * Description: Sets the readyToRead member variable which is used to indicate
 * 							when it's okay to grab ssMeasurements
 *
 * Parameters:	ready - true if it's okay to start reading, false otherwise
 *
 * Returned: 		None
 ******************************************************************************/
void dataCollect::setReadyToRead (bool ready)
{
	this->readyToRead = ready;
}

/*******************************************************************************
 * Method: 			isReadyToRead
 *
 * Description: Checks to see if it's okay to start reading ssMeasurements from
 * 							the buffer
 *
 * Parameters:	None
 *
 * Returned: 		True if it's okay to start reading, false otherwise
 ******************************************************************************/
bool dataCollect::isReadyToRead () const
{
	return this->readyToRead;
}

/*******************************************************************************
 * Method: 			isDebug
 *
 * Description: Checks to see if this is in debug mode
 *
 * Parameters:	None
 *
 * Returned: 		True if it's in debug mode, false otherwise
 ******************************************************************************/
bool dataCollect::isDebug() const
{
	return this->debug;
}

/*******************************************************************************
 * Method: 			clearBuffer
 *
 * Description: Clears the measurement buffer
 *
 * Parameters:	None
 *
 * Returned: 		None
 ******************************************************************************/
void dataCollect::clearBuffer ()
{
	buffer.clear ();
}

/*******************************************************************************
 * Method: 			update
 *
 * Description: Update the buffer with the key id with the value ss. If the id
 * 							does not exist then it will be created in the buffer. If the id
 * 							does exist then the new measurement is added to the measurement
 * 							array at 0, shifting the other values down.
 *
 * Parameters:	None
 *
 * Returned: 		None
 ******************************************************************************/
void dataCollect::update(std::string id, int ss)
{
	std::map<std::string, recent>::iterator bufferIter = buffer.find (id);

	if (buffer.end() == bufferIter) // this is the first measurement for this dev
	{
		buffer [id].data[0] = ss;

		for (int i = 1; i < CONTAINER_SIZE; i++)
		{
			buffer [id].data[i] = 0;
		}

	}
	else // we need to update a dev
	{
		for (int i = CONTAINER_SIZE - 1; i > 0; i--)
		{
			(bufferIter->second).data[i] = (bufferIter->second).data[i - 1];
		}

		(bufferIter->second).data[0] = ss;
	}
}



