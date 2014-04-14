/*******************************************************************************
File:				multicast.cpp
Author: 		Josh Siva
Date:				3/10/14
Project:		NachoNet
Purpose:		Implements the behavior of the multicast object whose job it is to
						broadcast an initial datagram with the node's IP address so that
						a node in the network will know with data needs to be synchronized.
						Datagrams should be padded to BUF_LENGTH to avoid grabbing multiple
						datagrams in the buffer
*******************************************************************************/

#include "../../include/exch/multicast.h"

const std::string multicast::MULTICAST_GROUP = "225.1.1.1";
const std::string multicast::LOCAL_INTERFACE = "127.0.0.1";

/*******************************************************************************
 * Constructor:	multicast
 *
 * Description:	Prepares for a multicast  by opening up a socket and setting up
 * 							the appropriate socket options for reading from and writing to
 * 							the socket
 *
 * Parameters:	port - the port to which we want to bind the socket
 * 							localIfaceAddr - the local interface we want associated with the
 * 															 multicast
 * 							multicastGroupAddr - the address of the multicast group
 *
 * Returned:		None
 ******************************************************************************/
multicast::multicast (int port, std::string localIfaceAddr,
											std::string multicastGroupAddr)
{
	char loopBack = 0;
	int reuse = 1;

	problem = false;

	sndSD = socket (AF_INET, SOCK_DGRAM, 0);

	if (sndSD < 0)
	{
		//error
		perror ("send socket");
		problem = true;
	}

	//Configure the transmitter side
	memset((char *) &groupSock, 0, sizeof(groupSock));
  groupSock.sin_family = AF_INET;
  groupSock.sin_addr.s_addr = inet_addr(multicastGroupAddr.c_str());
  groupSock.sin_port = htons(port);

  if (setsockopt (sndSD, IPPROTO_IP, IP_MULTICAST_LOOP,
                (char *)&loopBack, sizeof(loopBack)) < 0)
  {
    //error
  	perror ("socket options writer");
  	problem = true;
  	close (sndSD);
  }

  localInterface.s_addr = inet_addr(localIfaceAddr.c_str ());
  if (setsockopt(sndSD, IPPROTO_IP, IP_MULTICAST_IF,
                 (char *)&localInterface,
                 sizeof(localInterface)) < 0)
  {
  	//error
  	perror ("socket options writer");
  	problem = true;
  	close (sndSD);
  }


  //Configure the receiver side
  rcvSD = socket (AF_INET, SOCK_DGRAM, 0);
  if (rcvSD < 0)
	{
		//error
		perror ("receive socket");
		problem = true;
	}

  /*struct timeval tv;
  //tv.tv_sec = 2;   30 Secs Timeout
	if (setsockopt(rcvSD, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv,
			sizeof(struct timeval)) < 0)
	{
				 perror("setsockopt: rcvtimeo");
				 problem = true;
	}*/

  //If we wanted to make this socket non-blocking
  //fcntl (rcvSD, F_SETFL, O_NONBLOCK);

  if (setsockopt(rcvSD, SOL_SOCKET, SO_REUSEADDR,(char *)&reuse,
  		sizeof(reuse)) < 0)
  {
		perror("setting SO_REUSEADDR");
		close(rcvSD);
  }


  memset ((char *) &localSock, 0, sizeof(localSock));
  localSock.sin_family = AF_INET;
  localSock.sin_port = htons(port);
  localSock.sin_addr.s_addr  = htonl (INADDR_ANY);

  if (bind (rcvSD, (struct sockaddr*)&localSock, sizeof(localSock)))
  {
  	//error
  	perror ("binding socket");
  	problem = true;
  	close (rcvSD);
  }

  group.imr_multiaddr.s_addr = inet_addr(multicastGroupAddr.c_str());
  group.imr_interface.s_addr = inet_addr(localIfaceAddr.c_str());

  if (setsockopt (rcvSD, IPPROTO_IP, IP_ADD_MEMBERSHIP,(char *)&group,
  								sizeof(group)) < 0)
  {
  	//error
  	perror ("socket options reader");
  	problem = true;
  	close (rcvSD);
  }

}

/*******************************************************************************
 * Destroyer!:	~multicast
 *
 * Description:	Call shutdown
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
multicast::~multicast ()
{
	kill ();
}

/*******************************************************************************
 * Method:			transmit
 *
 * Description:	Send data of BUF_LENGTH that is padded with NULL terminators to
 * 							the length to everyone listening on the multicast.
 *
 * Parameters:	data - the data to send
 *
 * Returned:		None
 ******************************************************************************/
void multicast::transmit (char data[BUF_LENGTH])
{
	if (!problem)
	{
		if (sendto (sndSD, data, BUF_LENGTH, 0,(struct sockaddr*)&groupSock,
						 sizeof(groupSock)) < 0)
		{
			//error
			problem = true;
			close (sndSD);
		}
	}
}

/*******************************************************************************
 * Method:			receive
 *
 * Description:	Read BUF_LENGTH from the socket.
 *
 * Parameters:	None
 *
 * Returned:		string - the contents of socket
 ******************************************************************************/
std::string multicast::receive ()
{
	char dataBuf[BUF_LENGTH];
	std::string message;

	//fill the buffer with null terminators so that we know what an empty message
	//looks like
	for (int i = 0; i < BUF_LENGTH; i++)
	{
		dataBuf[i] = '\0';
	}

	if (!problem)
	{
		if (read (rcvSD, dataBuf, BUF_LENGTH) < 0)
		//if (recvTimeout (dataBuf, BUF_LENGTH, 5) < 0)
		{

			//error
			problem = true;
			close (rcvSD);
		}

		message = dataBuf;
	}

	return message;
}

/*******************************************************************************
 * Method:			kill
 *
 * Description:	Close the sockets
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void multicast::kill ()
{
	shutdown (rcvSD, SHUT_RD);

	close (sndSD);
	close (rcvSD);
}

/*******************************************************************************
 * Method:			recvTimeout
 *
 * Description: A version of receive that uses select to implement a non
 * 							blocking recv that uses a timeout.
 *
 * Parameters:	buffer - the buffer into which the socket contents are written
 * 							bufLen - the length of the buffer
 * 							timeout - the length of time to read before timing out
 *
 * Returned:		int - see recv error codes
 ******************************************************************************/
int multicast::recvTimeout (char * buffer, int bufLen, int timeout)
{
	fd_set readSet;
	int result, returnVal, iof = -1;
	struct timeval time;

	FD_ZERO (&readSet);
	FD_SET (rcvSD, &readSet);

	time.tv_sec = timeout;
	time.tv_usec = 0;

	result = select (rcvSD + 1, &readSet, NULL, NULL, &time);

	if (0 > result)
	{
		returnVal = -1;
	}
	else if (0 < result && FD_ISSET (rcvSD, &readSet))
	{
		iof = fcntl (rcvSD, F_GETFL, 0);

		if (-1 != iof)
		{
			fcntl (rcvSD, F_SETFL, iof | O_NONBLOCK);
		}

		result = read (rcvSD, buffer, bufLen);

		if (-1 != iof)
		{
			fcntl (rcvSD, F_SETFL, iof);
		}

		returnVal = result;
	}
	else
	{
		//perror ("read timeout");
	}

	return returnVal;

}


