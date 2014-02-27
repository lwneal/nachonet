/*******************************************************************************
File:				dataEx.h
Author: 		Josh Siva
Date:				2/27/14
Project:		NachoNet
Purpose:		Defines the abstract data exchange object that is responsible for
						sharing information across nodes
*******************************************************************************/

#pragma once
#include "node.h"
#include "device.h"

typedef struct Message
{
  std::vector<int> dest;
  std::string msg;
} Message;


class dataEx
{
  public:
    dataEx ();
    virtual ~dataEx ();
    int getNumNodes ();
    int getNumDevs ();
    void setID (int id);
    int getID () const;
    std::string getDevForUpdate ();
    std::vector<refMeasurement> getMeasurements (std::string id);
    void updateDevLocation (std::string id, location loc);
    void clearDevices ();
    void clearNodes ();

    virtual void addNode (node newNode);
    virtual void dropNode (int id);
    virtual void addDevice (device newDev);
    virtual void dropDevice (std::string id);

    virtual void pingAll (Message message) = 0; //send msg to everyone in net
    virtual void ping (Message message) = 0; //push msg and pull to check
    virtual void pushUpdates () = 0;
    virtual void pullUpdates () = 0;

    static const int NO_ID = -1;

    //MESSAGES TO SEND
    static const std::string HELLO = "HELLO";			//check if node is alive
		static const std::string GOODBYE = "GOODBYE";  //tell other nodes I'm
																									 //leaving
		static const std::string ACK = "ACK";					//acknowledge a HELLO
		static const std::string STOP = "STOP";				//tell other nodes to stop
		static const std::string START = "START";			//tell other nodes to start

		//STATES
		static const std::string RUNNING = "RUNNING";  //state of running node
		static const std::string DEAD = "DEAD";				//state of dead node

  protected:
    std::map<int, node> nodes;
    std::map<std::string, device> devices;
  private:
    int lastNodeUsed;
    int myID;
};
