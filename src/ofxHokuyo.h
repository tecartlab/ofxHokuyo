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

	void exit();

	/**
	NOTICE: call calculateEuclidian once a new frame has been received
			before you get the updated Euclidian vector

	@returns a vector with all the rays, where each ray is a vec3 where
		x -> posX
		y -> posX
		z -> distance
	*/
	vector<glm::vec3> & getEuclidian();

	/**
	calculate the euclidian space point for each ray reflection,
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
	@param angleOffset in deg from the negative y axis clockwise
	@param mirror mirrors the result on the y axis.
	*/
	bool calculateEuclidian(int startAngle, int endAngle, int angleOffset, bool mirror);

	/**
	draws the lines of the rays. 
	NOTICE: call calculateEuclidian once a new frame has been received
	*/
    void drawRays();
    
protected:
    std::string mIP = "192.168.0.10";
    int mPort = 10940;
 
	int mScanStartAngle;
    int mScanEndAngle;
    
	ofxTCPClient mTCPClient;
	std::string mSendMessage;
	std::string mReceiveMessage;

	vector<int> mScanRaw;
	vector<glm::vec3> mScanEuclidean;

	int mScanTimeStamp;

	int frameCount;

	float lidarScale;
};
    
};

#endif
