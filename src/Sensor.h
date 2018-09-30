
#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxGuiExtended.h"

#pragma once
namespace sensor
{
	class Event
	{
	public:
		Event(int ID, glm::vec2 pos, int size, int lifeSpan);
		~Event();

		void prepare();
		bool isSame(glm::vec2 pos);
		void update(glm::vec2 pos, float smoothing);
		bool cleanup();
		float isDying();
		int getID();
		glm::vec2 getCenter();
		int getSize();
		int getLifeCycles();

		void draw();
		void send();

	protected:
		int mID;

		int mCountDown;
		int mBreathSize;

		int mLifeCycles;

		glm::vec2 mCenter;
		glm::vec2 mLastCenter;
		int mSize;
	};

	class SensorField
	{
	public:
		SensorField();
		~SensorField();

		void setup(ofxGui &gui, string name);
		bool update(std::vector<glm::vec3> data);
		void drawField();
		void drawEvents();
		void drawEventLabels();

		void save();

		ofxGuiPanel *panel;

	protected:
		ofTrueTypeFont myfont;

		bool updateListener();
		string getOscMsgAsString(ofxOscMessage m);

		ofxOscSender broadcaster;
		ofxOscReceiver listener;

		vector<Event> events;

		ofxGuiGroup *broadcastGroup;
		ofxGuiGroup *listenerGroup;
		ofxGuiGroup *fieldGroup;
		ofxGuiGroup *sensitivityGroup;

		ofParameter<string> mBroadcastIP;
		ofParameter<int> mBroadcastPort;
		ofParameter<int> mListeningPort;

		ofParameter<int> limitUp;
		ofParameter<int> limitDown;
		ofParameter<int> limitLeft;
		ofParameter<int> limitRight;

		ofParameter<int> eventSize;
		ofParameter<int> eventRayGap;
		ofParameter<int> eventBreathSize;
		ofParameter<float> smoothing;

	};
}
