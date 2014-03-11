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

multicast::multicast (int port, std::string localIface,
											std::string multicastGroup)
{
	char loopBack = 0;

	problem = false;

	sd = socket (AF_INET, SOCK_DGRAM, 0);

	if (sd < 0)
	{
		//error
		problem = true;
	}

	//Configure the receiver side
	memset ((char *) &localSock, 0, sizeof(localSock));
  localSock.sin_family = AF_INET;
  localSock.sin_port = htons(port);
  localSock.sin_addr.s_addr  = INADDR_ANY;

  if (bind (sd, (struct sockaddr*)&localSock, sizeof(localSock)))
  {
  	//error
  	problem = true;
  }

  group.imr_multiaddr.s_addr = inet_addr(multicastGroup.c_str());
  group.imr_interface.s_addr = inet_addr(localIface.c_str());

  if (setsockopt (sd, IPPROTO_IP, IP_ADD_MEMBERSHIP,(char *)&group,
  								sizeof(group)) < 0)
  {
  	//error
  	problem = true;
  }



	//Configure the transmitter side
	memset((char *) &groupSock, 0, sizeof(groupSock));
  groupSock.sin_family = AF_INET;
  groupSock.sin_addr.s_addr = inet_addr(multicastGroup.c_str());
  groupSock.sin_port = htons(port);

  if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP,
                (char *)&loopBack, sizeof(loopBack)) < 0)
  {
    //error
  	problem = true;
  }

  localInterface.s_addr = inet_addr(localIface.c_str ());
  if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF,
                 (char *)&localInterface,
                 sizeof(localInterface)) < 0)
  {
  	//error
  	problem = true;
  }
}

multicast::~multicast ()
{

}

void multicast::transmit (char data[BUF_LENGTH])
{
	if (!problem)
	{
		if (sendto(sd, data, BUF_LENGTH, 0,(struct sockaddr*)&groupSock,
						 sizeof(groupSock)) < 0)
		{
			//error
			problem = true;
		}
	}


}

std::string multicast::receive ()
{
	std::string message;

	if (!problem)
	{
		if (read(sd, databuf, datalen) < 0)
		{
			//error
		}
	}

	return message;
}

void shutdown ()
{

}


