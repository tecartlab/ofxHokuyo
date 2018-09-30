
#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxGuiExtended.h"

#pragma once
namespace sensor
{
	class Event
	{
	public:
		Event();
		~Event();

		void prepare();
		bool update();
		bool cleanup();

		void draw();
		void send();
	
	protected:
		bool isAlive;

		glm::vec2 center;
		int size;

		int lifespan;
	};

	class SensorField
	{
	public:
		SensorField();
		~SensorField();

		void setup(ofxGui &gui, string name);
		bool update(std::vector<glm::vec3> data);
		void drawField();

		void save();

		ofxGuiPanel *panel;

	protected:

		bool updateListener();
		string sensor::SensorField::getOscMsgAsString(ofxOscMessage m);

		ofxOscSender broadcaster;
		ofxOscReceiver listener;

		vector<Event> events;

		ofxGuiGroup *broadcastGroup;
		ofxGuiGroup *listenerGroup;
		ofxGuiGroup *fieldGroup;

		ofParameter<string> mBroadcastIP;
		ofParameter<int> mBroadcastPort;
		ofParameter<int> mListeningPort;

		ofParameter<int> limitUp;
		ofParameter<int> limitDown;
		ofParameter<int> limitLeft;
		ofParameter<int> limitRight;

	};
}
