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
#include "../tools/DataExAdmin.h"
#include <iterator>

typedef struct Message
{
  std::vector<int> dest; //we can specify multiple destinations here
  std::string msg;
} Message;


class dataEx
{
	friend class DataExAdmin;
  public:
    dataEx ();
    virtual ~dataEx ();
    int getNumNodes ();
    int getNumDevs ();
    void setID (int id);
    int getID () const;
    void setIsAlive (bool state);
    bool alive () const;
    void setPingStatus (int nodeID, bool status);
    bool lastPingResult (int nodeID);

    std::string getDevForUpdate ();

    std::vector<refMeasurement> getMeasurements (std::string id);
    void updateDevLocation (std::string id, location loc);
    void updateDevMeasurement (distMeasurement dist);
    std::vector<location> getDeviceLocations ();
    std::vector<location> getNodeLocations ();
    void clearDevices ();
    void clearNodes ();

    virtual void addNode (node newNode);
    virtual void dropNode (int id);
    virtual void addDevice (device newDev);
    virtual void dropDevice (std::string id);

    virtual void ping (Message message) = 0; //push msg
    virtual void checkMessages () = 0; //handles messages
    virtual void pushUpdates (int flag) = 0;//id what flag will be needed but
    virtual void pullUpdates (int flag) = 0;//some data needs to get passed

    //MESSAGES TO SEND
    static const std::string HELLO;			//check if node is alive
		static const std::string GOODBYE;  //tell other nodes I'm
																									 //leaving
		static const std::string ACK;					//acknowledge a HELLO
		static const std::string STOP;				//tell other nodes to stop
		static const std::string START;			//tell other nodes to start

		//STATES
		static const std::string RUNNING;  //state of running node
		static const std::string DEAD;				//state of dead node

  protected:
    std::map<int, node> nodes;
    std::map<std::string, device> devices;
    std::vector<device> devsUpdatedSinceLastPush;
  private:
    std::map<int, bool> aliveOnLastPing;
    int myID;
    bool isAlive;
};
