//
//  Lidar.cpp
//  Test0
//
//  Created by Daniel Bisig on 04/02/18.
//
//

#include "ofxHokuyo.h"

lidar::ofxHokuyo::ofxHokuyo()
{
}

lidar::ofxHokuyo::~ofxHokuyo()
{
	if (mTCPClient.isConnected()) {
		mTCPClient.close();
	}
}

void lidar::ofxHokuyo::setup(std::string ip, int port)
{
	std::string mIP = ip;
	int mPort = port;

	lidarScale = 0.1;

	mReceiveMessage = "initialValue";

	mScanRaw.resize(LIDARRANGE,0);
	mScanEuclidean.resize(LIDARRANGE, glm::vec3());

	mTCPClient.setup(mIP, mPort);
	mTCPClient.setMessageDelimiter("\n");

	std::cout << "connecting to ip:"<< mIP << " on port:("<< mPort  <<").";
	while (mTCPClient.isConnected() == false)
	{
		std::cout << ".";
		ofSleepMillis(10);
	}
	std::cout << "\n...connected\n";

	std::cout << "\nget version info\n";

	//version acquisition ("VV")
	mSendMessage = "VV\n";
	mTCPClient.send(mSendMessage);

	ofSleepMillis(10);

	string message = "";
	do {
		message = mTCPClient.receive();
		std::cout << message << "\n";
	} while (message.length() > 0);
}

bool lidar::ofxHokuyo::update()
{
	if (mTCPClient.isConnected() == true)
	{
		string message;

		do
		{
			message = mTCPClient.receive();

			if (message.find("GD") == 0) {
				//std::cout << "Grab Data Echo detected.. " << "\n";
				//for some strange reason the device is only sending the last few packages once the next GD command was received. 
				// it turns out that when the GD echo is received, all packages have arrived and parsing can get started...

				if (frameCount > 0) {
					if (mReceiveMessage.length() == (LIDARRANGE * 3)) {
						//std::cout << "complete message (" << mReceiveMessage.length() << ")" << "\n";
						//std::cout << "frameCount (" << frameCount << ")" << "\n";

						char h30 = '0';

						unsigned int i;

						for (i = 0; i < LIDARRANGE; i++) {
							char val1 = mReceiveMessage[(i * 3)];
							char val2 = mReceiveMessage[(i * 3) + 1];
							char val3 = mReceiveMessage[(i * 3) + 2];

							int val1h = val1 - h30;
							int val2h = val2 - h30;
							int val3h = val3 - h30;

							int final = (val1h << 12) + (val2h << 6) + val3h;

							mScanRaw[i] = final;
						}
						//....

						mReceiveMessage = "";
						frameCount = 0;

						return true;
					}
					else {
						mReceiveMessage = "";
						frameCount = 0;
						return false;
					}
				}

			}
			else if (message.find("00P") == 0) {
				// toss this
				//std::cout << "empty detected.. " << "\n";
			}
			else if (message.length() == 5) {
				//std::cout << "timestamp detected: " << message << "\n";
				char val1 = message[0];
				char val2 = message[1];
				char val3 = message[2];

				char h30 = '0';

				int val1h = val1 - h30;
				int val2h = val2 - h30;
				int val3h = val3 - h30;

				mScanTimeStamp = (val1h << 12) + (val2h << 6) + val3h;
				//std::cout << " timestamp -->" << final << "<--\n";

				frameCount = 0;
			}
			else if (message.length() > 0) {
				mReceiveMessage = mReceiveMessage + message.substr(0, message.length() - 1);
				//std::cout << "m("<< message.length() << ") -->" << message << "<--\n";
				frameCount++;
			}
			else {
				//std::cout << "empty...\n";
			}
		} while (message.length() > 0);

		if (mReceiveMessage.length() > 0) {
			// we call for the next frame once we received the first batch of data.
			// BE AWARE: At this point not all data of the last frame has been received,
			//           only after receiving the echo of the following call can we be
			//           sure all the data has been received
			//std::cout << "send GD..." << "\n";

			//Distance acquisition ("GD")
			mSendMessage = "GD"; // retrieve distance data
			mSendMessage += "0000"; // start step: 0
			mSendMessage += "1080"; // end step: 100
			mSendMessage += "00"; // cluster count: 0
			mSendMessage += "\n";

			mTCPClient.send(mSendMessage);
		}
	}
	else {
		std::cout << "connection got terminated... " << "\n";
	}	
	return false;
}

void lidar::ofxHokuyo::exit()
{
	mTCPClient.close();
}

vector<glm::vec3>& lidar::ofxHokuyo::getEuclidian()
{
	return mScanEuclidean;
}

int lidar::ofxHokuyo::getRawDistance(float angle)
{
	int index = (angle - 45.) / 0.25;
	return mScanRaw[( 0 <= index && index < mScanRaw.size())? index: 0];
}

bool lidar::ofxHokuyo::calculateEuclidian(int startAngle, int endAngle, float angleOffset, bool mirror)
{
	float start = (startAngle - angleOffset >= 45) ?startAngle - angleOffset - 45: 0;
	float end = (endAngle - angleOffset <= 315) ? endAngle - angleOffset - 45 : 270;
	if (end > start) {
		float angle = 0;
		for (unsigned int i = 0; i < mScanEuclidean.size(); i++) {
			angle = i / 4.;
			if (start < angle && angle < end) {
				float posX = mScanRaw[i] * sin(ofDegToRad(angle + 45. + angleOffset));
				float posY = mScanRaw[i] * cos(ofDegToRad(angle + 45. + angleOffset));
				mScanEuclidean[i] = glm::vec3((mirror) ? -posX : posX, posY, mScanRaw[i]);
			}
			else {
				mScanEuclidean[i] = glm::vec3(0, 0, mScanRaw[i]);
			}
		}
		return true;
	}
	else {
		ofLogError("calculateEuclidian() has bigger startAngle than endAngle");
	}
	return false;
}

void lidar::ofxHokuyo::drawRays()
{
	ofSetColor(255, 255, 255);
	for (unsigned int i = 0; i < mScanEuclidean.size(); i++) {
		ofDrawLine(0, 0, mScanEuclidean[i].x , mScanEuclidean[i].y);
	}
	ofSetColor(255, 0, 0);
	ofDrawLine(0, 0, 5000, 0);
	ofSetColor(0, 255, 0);
	ofDrawLine(0, 0, 0, 5000);

}

int lidar::ofxHokuyo::getTimeStamp()
{
	return mScanTimeStamp;
}


bool lidar::ofxHokuyo::startSensing()
{
	if (mTCPClient.isConnected()) {
		std::cout << "\nstart sensor...\n";

		// measurement start ("BM");
		mSendMessage = "BM\n";
		mTCPClient.send(mSendMessage);

		string message = "";
		do {
			message = mTCPClient.receive();
			std::cout << message << "\n";
		} while (message.find("BM") != 0);

		std::cout << "\n...started\n";

		return true;
	}
	return false;
}

bool lidar::ofxHokuyo::stopSensing()
{
	if (mTCPClient.isConnected()) {
		std::cout << "\nquit sensor...\n";

		// measurement start ("BM");
		mSendMessage = "QT\n";
		mTCPClient.send(mSendMessage);

		std::cout << "\n...stopped\n";

		return true;
	}
	return false;
}