//
//  Lidar.cpp
//  Test0
//
//  Created by Daniel Bisig on 04/02/18.
//
//

#include "ofxHokuyo.h"

void lidar::ofxHokuyo::setup(std::string ip, int port)
{
	std::string mIP = ip;
	int mPort = port;

	lidarScale = 0.1;

	mReceiveMessage = "initialValue";

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
		std::cout << "\nstop sensor...\n";

		// measurement start ("BM");
		mSendMessage = "QT\n";
		mTCPClient.send(mSendMessage);

		string message = "";
		do {
			message = mTCPClient.receive();
			std::cout << message << "\n";
		} while (message.find("QT") != 0);

		std::cout << "\n...stopped\n";

		return true;
	}
	return false;
}

bool lidar::ofxHokuyo::update()
{
	if (mTCPClient.isConnected() == true)
	{
		string message;

		do
		{
			message = mTCPClient.receive();

			if (message.find(mSendMessage.substr(0, mSendMessage.length() - 1)) == 0) {
				//std::cout << "Echo detected.. " << "\n";
				//for some strange reason the device is only sending the last few packages once the next GD command was received. 
				// it turns out that when the GD echo is received, all packages have arrived and parsing can get started...

				if (mReceiveMessage.length() > 0 && frameCount > 0) {
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

						lidarRaw[i] = final;
					}
					//....

					mReceiveMessage = "";
					frameCount = 0;

					return true;
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

				int final = (val1h << 12) + (val2h << 6) + val3h;
				std::cout << " timestamp -->" << final << "<--\n";

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
			std::cout << "send GD..." << "\n";

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
