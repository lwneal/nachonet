/*******************************************************************************
File:				multicast.h
Author: 		Josh Siva
Date:				3/10/14
Project:		NachoNet
Purpose:		Defines the multicast class which is used to broadcast and receive
						messages for NachoNet. This is used for adding new nodes to the
						network.
*******************************************************************************/

#pragma once
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <unistd.h>

class multicast
{
	public:
		static const std::string MULTICAST_GROUP;
		static const std::string LOCAL_INTERFACE;
		static const int DEFAULT_PORT = 55555;
		static const int BUF_LENGTH = 64;

		multicast (int port = DEFAULT_PORT,
							 std::string localIfaceAddr = LOCAL_INTERFACE,
							 std::string multicastGroupAddr = MULTICAST_GROUP);
		~multicast ();
		void transmit (char data[BUF_LENGTH]);
		std::string receive ();
		void shutdown ();

	private:
		bool problem;
		int sd;

		//write
		struct in_addr localInterface;
		struct sockaddr_in groupSock;

		//read
		struct sockaddr_in localSock;
		struct ip_mreq group;

};
