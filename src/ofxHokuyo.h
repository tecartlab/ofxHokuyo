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

	/**
	calculates the euclidian space for each ray reflection, 
	assuming the sensor in the center and the main axis is y.

	 (min 45) startAngle  -y     endAngle (max 315)
				    \     |     /
				     \    |    /
					  \   |   /
					   \  |  /
						\ | /
						 \|/
	-x -----------------------------------------> x
	                      |
						  |
						  |
						  |
						  |
						  |
						  y

	@param startAngle in deg from the negative y axis clockwise (min 45, max 315)
	@param endAngle in deg from the negative y axis clockwise (min 45, max 315)
	*/
	vector<glm::vec2> & calculateEuclidian(int startAngle, int endAngle);

	/**
	draws the lines of the rays. 
	NOTICE: call calculateEuclidian once a new frame has been received
	*/
    void drawLines(float scale);
    
protected:
    std::string mIP = "192.168.0.10";
    int mPort = 10940;
 
	int mScanStartAngle;
    int mScanEndAngle;
    
	ofxTCPClient mTCPClient;
	std::string mSendMessage;
	std::string mReceiveMessage;

	vector<int> mScanRaw;
	vector<glm::vec2> mScanEuclidean;

	int mScanTimeStamp;

	int frameCount;

	float lidarScale;
};
    
};

#endif
