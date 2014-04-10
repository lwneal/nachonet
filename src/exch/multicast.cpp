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
	struct timeval tv;

	tv.tv_sec = 5;
	tv.tv_usec = 0;

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

  fcntl (rcvSD, F_SETFL, O_NONBLOCK);

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
 * Description:	Close the socket
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
multicast::~multicast ()
{
	close (sndSD);
	close (rcvSD);
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
		if (recv(rcvSD, dataBuf, BUF_LENGTH, 0) < 0)
		{
			//error
			problem = true;
			close (rcvSD);
		}

		message = dataBuf;
	}

	return message;
}


