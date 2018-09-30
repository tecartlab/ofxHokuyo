#include "Sensor.h"


sensor::Event::Event(int ID, glm::vec2 pos, int size, int lifeSpan)
{
	mID = ID;
	mCenter = pos;
	mLastCenter = pos;
	mSize = size;
	mBreathSize = lifeSpan;
	mCountDown = mBreathSize;
	mLifeCycles = 0;
}

sensor::Event::~Event()
{
}

void sensor::Event::prepare()
{
	mCountDown--;
}

bool sensor::Event::isSame(glm::vec2 pos)
{
	return (glm::distance(pos, mCenter) <= mSize)?true: false;
}

void sensor::Event::update(glm::vec2 pos, float smoothing)
{
	mCenter = pos;
	mLifeCycles++;
	mCountDown = mBreathSize;
}

bool sensor::Event::cleanup()
{
	return  (mCountDown < 0)?true: false;
}

float sensor::Event::isDying()
{
	return mCountDown / mBreathSize;
}

int sensor::Event::getID()
{
	return mID;
}

glm::vec2 sensor::Event::getCenter()
{
	return mCenter;
}

int sensor::Event::getSize()
{
	return mSize;
}

int sensor::Event::getLifeCycles()
{
	return mLifeCycles;
}

void sensor::Event::draw()
{
	ofDrawCircle(mCenter, mSize);
}

sensor::SensorField::SensorField()
{
	mBroadcastIP.set("Broadcast IP", "127.0.0.1");
	mBroadcastPort.set("Broadcast Port", 11111, 11110, 11119);
	mListeningPort.set("Listening Port", 11121, 11120, 11129);

	limitUp.set("UpperLimit [mm]", 500, 0, 4000);
	limitDown.set("LowerLimit [mm]", 2500, 0, 4000);
	limitLeft.set("LeftLimit [mm]", -2000, -8000, 8000);
	limitRight.set("RightLimit [mm]", 2000, -8000, 8000);

	eventSize.set("Search size", 100, 0, 1000);
	eventRayGap.set("Ray Gap", 5, 0, 20);
	eventBreathSize.set("Cycles to death", 10, 0, 100);
	smoothing.set("Smoothing", 0.5, 0.0, 1.0);
}

sensor::SensorField::~SensorField()
{
	broadcaster.clear();
	listener.stop();
}

void sensor::SensorField::setup(ofxGui &gui, string name)
{
	panel = gui.addPanel();

	panel->loadTheme("theme/theme_light.json");
	panel->setName(name);

	broadcastGroup = panel->addGroup("Broadcast");
	broadcastGroup->add<ofxGuiTextField>(mBroadcastIP);
	broadcastGroup->add<ofxGuiIntInputField>(mBroadcastPort);

	listenerGroup = panel->addGroup("Listener");
	listenerGroup->add<ofxGuiIntInputField>(mListeningPort);

	fieldGroup = panel->addGroup("SensorField");
	fieldGroup->add(limitUp);
	fieldGroup->add(limitDown);
	fieldGroup->add(limitLeft);
	fieldGroup->add(limitRight);

	sensitivityGroup = panel->addGroup("Sensitivity");
	sensitivityGroup->add(eventSize);
	sensitivityGroup->add(eventRayGap);
	sensitivityGroup->add(eventBreathSize);
	sensitivityGroup->add(smoothing);

	panel->loadFromFile(name + ".xml");

	broadcaster.setup(mBroadcastIP.get(), mBroadcastPort.get());
	listener.setup(mListeningPort.get());

	myfont.load("verdana.ttf", 100);
}

bool sensor::SensorField::update(std::vector<glm::vec3> data)
{
	// lets first check if some messages for us have arrived...
	updateListener();

	int minID = 0;

	// we prepare all the previous events
	for (int e = 0; e < events.size(); e++) {
		events[e].prepare();
		minID = (events[e].getID() >= minID)? events[e].getID() + 1: minID;
	}

	int rayGap = 0;
	int counter = 0;
	glm::vec2 hit = glm::vec2();
	// now do our job
	for (int i = 0; i < data.size(); i++) {
		// first check if the point is inside the sensorfield:	
		if (limitUp.get() < data[i].y && data[i].y < limitDown.get() &&
			limitLeft.get() < data[i].x && data[i].x < limitRight.get()) {

				//std::cout << "got event... " << "\n";
				counter++;
				hit += glm::vec2(data[i].x, data[i].y);
		}
		else {
			if (counter > 0 && rayGap++ > eventRayGap.get()) {
				hit /= counter;
				//std::cout << "got event at " << hit.x << " "  << hit.y << " size: " << counter << "\n";

				for (int e = 0; e < events.size(); e++) {
					if (events[e].isSame(hit)) {
						events[e].update(hit, smoothing.get());
						counter = 0;
						hit = glm::vec2();
						break;
					}
				}
				if (counter > 0) {
					// no previous event was found, so create a new one.
					events.push_back(Event(minID, hit, eventSize.get(), eventBreathSize.get()));
				}
				counter = 0;
				rayGap = 0;
				hit = glm::vec2();
			}
		}
	}

	// we clean up all the events
	for (int e = events.size() - 1; e >= 0; e--) {
		if (events[e].cleanup()) {
			events.erase(events.begin() + e);
		}
	}

	if (events.size() > 0) {
		for (int e = 0; e < events.size(); e++) {
			if (events[e].isDying() == 1.0) {
				ofxOscMessage sensorbox;
				sensorbox.setAddress("/event");
				sensorbox.addIntArg(events[e].getID());
				sensorbox.addIntArg(events[e].getLifeCycles());
				sensorbox.addIntArg(events[e].getCenter().x);
				sensorbox.addIntArg(events[e].getCenter().y);

				broadcaster.sendMessage(sensorbox);
			}
		}
	}

	return false;
}

void sensor::SensorField::drawField()
{
	ofSetColor(255, 0, 0);
	ofDrawLine(limitLeft.get(), limitUp.get(), limitRight.get(), limitUp.get());
	ofDrawLine(limitLeft.get(), limitDown.get(), limitRight.get(), limitDown.get());
	ofDrawLine(limitLeft.get(), limitDown.get(), limitLeft.get(), limitUp.get());
	ofDrawLine(limitRight.get(), limitDown.get(), limitRight.get(), limitUp.get());
}

void sensor::SensorField::drawEvents()
{
	for (int e = 0; e < events.size(); e++) {
		ofSetColor(255. * events[e].isDying(), 0, 0);
		events[e].draw();
	}
}

void sensor::SensorField::drawEventLabels()
{
	for (int e = 0; e < events.size(); e++) {
		ofSetColor(255 * events[e].isDying(), 0, 0);
		myfont.drawString("ID:" + ofToString(events[e].getID()), events[e].getCenter().x + events[e].getSize(), events[e].getCenter().y + events[e].getSize());
	}
}

void sensor::SensorField::save()
{
	panel->saveToFile(panel->getName() + ".xml");
}

bool sensor::SensorField::updateListener()
{
	while (listener.hasWaitingMessages()) {
		// get the next message
		ofxOscMessage m;
		listener.getNextMessage(m);
		//Log received message for easier debugging of participants' messages:
		ofLog(OF_LOG_NOTICE, "Server recvd msg " + getOscMsgAsString(m) + " from " + m.getRemoteIp());

		// check the address of the incoming message
		if (m.getAddress() == "/refresh") {
			//Identify host of incoming msg
			ofxOscMessage sensorbox;
			sensorbox.setAddress("/sensorbox");
			sensorbox.addIntArg(limitUp.get());
			sensorbox.addIntArg(limitDown.get());
			sensorbox.addIntArg(limitLeft.get());
			sensorbox.addIntArg(limitRight.get());

			broadcaster.sendMessage(sensorbox);
			ofLogVerbose("Sensor received /refresh message");
		}
		else if (m.getAddress() == "/ping") {
			ofxOscMessage sensorbox;
			sensorbox.setAddress("/ping");
			sensorbox.addIntArg(1);

			broadcaster.sendMessage(sensorbox);
			ofLogVerbose("Sensor received /ping message");
		}
		// handle getting random OSC messages here
		else {
			ofLogWarning("Server got weird message: " + m.getAddress());
		}
	}
	return false;
}

string sensor::SensorField::getOscMsgAsString(ofxOscMessage m) {
	string msg_string;
	msg_string = m.getAddress();
	msg_string += ":";
	for (int i = 0; i < m.getNumArgs(); i++) {
		// get the argument type
		msg_string += " " + m.getArgTypeName(i);
		msg_string += ":";
		// display the argument - make sure we get the right type
		if (m.getArgType(i) == OFXOSC_TYPE_INT32) {
			msg_string += ofToString(m.getArgAsInt32(i));
		}
		else if (m.getArgType(i) == OFXOSC_TYPE_FLOAT) {
			msg_string += ofToString(m.getArgAsFloat(i));
		}
		else if (m.getArgType(i) == OFXOSC_TYPE_STRING) {
			msg_string += m.getArgAsString(i);
		}
		else {
			msg_string += "unknown";
		}
	}
	return msg_string;
}

