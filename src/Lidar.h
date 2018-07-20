/** \file Lidar.h
*/

#ifndef _Lidar_h_
#define _Lidar_h_

#include <iostream>
#include "ofxNetwork.h"
#include "ofThread.h"

namespace dab
{

    class Lidar : public ofThread
{
public:
    Lidar();
    ~Lidar();
    
    void setup();
    void startSensing();
    void stopSensing();
    
protected:
    std::string mIP = "192.168.0.10";
    int mPort = 10940;
    std::string mSendString;
    std::string mReceiveString;
    std::string mReceiveMessage;
    int mScanStartAngle;
    int mScanEndAngle;
    
    int mScanTimeStamp;
    std::vector<float> mScanValues;
    bool mComIdle;
    int mComMinIdleDuration = 1000; // usec
        
};
    
};

#endif
