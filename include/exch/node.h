/*******************************************************************************
File:				node.h
Author: 		Josh Siva
Date:				2/25/14
Project:		NachoNet
Purpose:		Defines the node object which is an abstracted view of a node in
						NachoNet
*******************************************************************************/

#pragma once
#include <string>
#include <vector>
#include <map>
#include "../dist/dist.h"
#include "../loc/loc.h"

class node
{
  public:
    node (int id, location loc);
    node (int id = NO_ID);
    ~node ();
    int getID () const;
    void setID (int id);
    location getLocation () const;
    void setLocation (location loc);
    void setMeasurement (distMeasurement devDist);
    refMeasurement getMeasurement (std::string devID);
    distMeasurement getBasicMeasurement (std::string devID);
    std::vector<refMeasurement> getAllMeasurements ();
    std::vector<distMeasurement> getAllBasicMeasurements ();
    void clearMeasurements ();

    static const int NO_ID = -1;

  private:
    int id;
    location loc;
    std::map<std::string, distMeasurement> measurements;

};

