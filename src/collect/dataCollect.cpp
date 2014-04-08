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
#include <iostream>


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
 * Description: Returns a vector of ssMeasurements but excludes the devices
 * 							for which there are fewer than CONTAINER_SIZE measurements
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

		sort.clear();

		if (0 != (item.second).data[CONTAINER_SIZE - 1])
		{
			returnVector.push_back(temp);
		}
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
 * Method: 			isReadyToRead
 *
 * Description: Checks to see if it's okay to start reading ssMeasurements from
 * 							the buffer. Once 3/4 of the devices have 5 measurements then
 * 							they are considered ready to be read
 *
 * Parameters:	None
 *
 * Returned: 		True if it's okay to start reading, false otherwise
 ******************************************************************************/
bool dataCollect::isReadyToRead ()
{
	unsigned int numReadyToRead = 0;

	if (!readyToRead)
	{
		for (auto &item : buffer)
		{
			if (0 != item.second.data[CONTAINER_SIZE - 1])
			{
				numReadyToRead++;
			}
		}

		if (numReadyToRead >= READY_CUTOFF * buffer.size())
		{
			readyToRead = true;
		}

		if(isDebug ())
		{
			std::cout << "Total ready to read: " << numReadyToRead << "    ";
			std::cout << "Out of: " << buffer.size () << "\n";
		}
	}


	return readyToRead;
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
	readyToRead = false;
}

/*******************************************************************************
 * Method: 			update
 *
 * Description: Update the buffer with the key id with the value ss. If the id
 * 							does not exist then it will be created in the buffer. If the id
 * 							does exist then the new measurement is added to the measurement
 * 							array at 0, shifting the other values down.
 *
 * Parameters:	id - the MAC address of the device
 * 							ss - the signal strength from the device
 *
 * Returned: 		None
 ******************************************************************************/
void dataCollect::update(std::string id, int ss)
{
	std::map<std::string, recentData>::iterator bufferIter = buffer.find (id);
	recentData tmp;

	if (buffer.end() == bufferIter) // this is the first measurement for this dev
	{
		tmp.data[0] = ss;

		for (int i = 1; i < CONTAINER_SIZE; i++)
		{
			tmp.data[i] = EMPTY;
		}

		buffer[id] = tmp;

	}
	else // we need to update a dev
	{
		tmp = bufferIter->second;

		for (int i = CONTAINER_SIZE - 1; i > 0; i--)
		{
			tmp.data[i] = tmp.data[i - 1];
		}

		tmp.data[0] = ss;

		bufferIter->second = tmp;

	}

	if (isDebug ())
	{
		std::cout << "-----recent: ";

		for (int i = 0; i < CONTAINER_SIZE; i++)
		{
			std::cout << tmp.data[i] << " ";
		}

		std::cout << "\n";

	}

}

void dataCollect::garbageCollect ()
{
	for (std::map<std::string, recentData>::iterator iter = buffer.begin ();
			 iter != buffer.end ();)
	{
		if (0 == iter->second.data[CONTAINER_SIZE - 1])
		{
			iter = buffer.erase (iter);
		}
		else
		{
			++iter;
		}
	}
}



