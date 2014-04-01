/*******************************************************************************
File:				dataCollect
Author: 		Josh Siva
Date:				1/26/14
Project:		NachoNet
Purpose:		The file defines the interface to the data collection module
*******************************************************************************/

#pragma once
#include <string>
#include <vector>
#include <map>

#define CONTAINER_SIZE 5

typedef struct ssMeasurement
{
  std::string devID;
  int ss;
} ssMeasurement;

typedef struct recentData
{
	int data[CONTAINER_SIZE];
} recentData;

class dataCollect
{
  public:
    dataCollect(bool debug);
    virtual ~dataCollect () {}
    std::vector<ssMeasurement> getSS() const;
    virtual void readFromNetwork() = 0;
    void setDebug(bool);
    bool isDebug() const;
    bool isReadyToRead ();
    void clearBuffer();
    const int MEDIAN = 2;
    const int EMPTY = 0;
    const float READY_CUTOFF = .75;

  protected:
   void update(std::string id, int ss);

  private:
    bool debug;
    bool readyToRead;
    std::map<std::string, recentData> buffer;//holds 5 most recent
																											//measurements
};

