/*******************************************************************************
File:				dataCollect
Author: 		Josh Siva
Date:				1/26/13
Project:		NachoNet
Purpose:		The file defines the interface to the data collection module
*******************************************************************************/

#pragma once
#include <string>

typedef struct ssMeasurement
{
  std::string devID;
  int ss;
} ssMeasurement;

class dataCollect
{
  public:
    dataCollect(bool debug);
    virtual ~dataCollect () {}
    std::vector<ssMeasurement> getSS() const;
    virtual void readFromNetwork() = 0;
    void setDebug(bool);
    bool isDebug() const;
    void setReadyToRead (bool);
    bool isReadyToRead () const;
    void clearBuffer();
    const int CONTAINER_SIZE = 5;
    const int MEDIAN = 2;

  protected:
    void update(std::string id, int ss);

  private:
    bool debug;
    bool readyToRead;
    std::map<std::string, int[CONTAINER_SIZE]> buffer;//holds 5 most recent
																											//measurements
};

