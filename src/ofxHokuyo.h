/** \file Lidar.h
*/

#ifndef _Lidar_h_
#define _Lidar_h_

#include <iostream>
#include "ofMain.h"
#include "ofxNetwork.h"
#include "ofThread.h"

#define LIDARMAX "1080"
#define LIDARRANGE 1081

namespace lidar
{
    class ofxHokuyo : public ofThread
{

public:
    ofxHokuyo();
    ~ofxHokuyo();
    
    void setup(std::string ip, int port);

    bool startSensing();
    bool stopSensing();
    
    bool update();
    void draw();
    
protected:
    std::string mIP = "192.168.0.10";
    int mPort = 10940;
    std::string mReceiveMessage;
 
	int mScanStartAngle;
    int mScanEndAngle;
    
    int mScanTimeStamp;
    std::vector<float> mScanValues;
  
	ofxTCPClient mTCPClient;
	std::string mSendMessage;
	std::string mReceiveMessage;

	int lidarRaw[LIDARRANGE];
	int lidarEuclidean[LIDARRANGE];

	int frameCount;

	float lidarScale;
};
    
};

#endif
