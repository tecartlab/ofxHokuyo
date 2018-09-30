#include "Sensor.h"



sensor::Event::Event()
{
}


sensor::Event::~Event()
{
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

	panel->loadFromFile(name + ".xml");

	broadcaster.setup(mBroadcastIP.get(), mBroadcastPort.get());
	listener.setup(mListeningPort.get());
}

bool sensor::SensorField::update(std::vector<glm::vec3> data)
{
	// lets first check if some messages for us have arrived...
	updateListener();

	// now do our job
	for (int i = 0; i < data.size(); i++) {
		// first check if the point is inside the sensorfield:	
		if (limitUp.get() < data[i].y && data[i].y < limitDown.get()) {
			if (limitLeft.get() < data[i].x && data[i].x < limitRight.get()) {

				//std::cout << "got event... " << "\n";

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

