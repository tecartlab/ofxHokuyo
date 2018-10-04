#include "Sensor.h"


sensor::Event::Event(int ID, glm::vec2 pos, int size, int generationSize, int lifeSpan)
{
	mID = ID;
	mCenter = pos;
	mLastCenter = pos;
	mSize = size;
	mGenerationSize = generationSize;
	mBreathSize = lifeSpan;
	mCountDown = mBreathSize;
	mLifeCycles = ofGetElapsedTimeMillis();
	mIsDying = false;
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
	return (!mIsDying && glm::distance(pos, mCenter) <= mGenerationSize)?true: false;
}

void sensor::Event::update(glm::vec2 pos, int size, float smoothPos, float smoothSize)
{
	mCenter = pos * (1.0 - smoothPos) + mCenter * smoothPos;
	mSize = size * (1.0 - smoothSize) + mSize * smoothSize;;
	mCountDown = mBreathSize;
}

bool sensor::Event::cleanup()
{
	if (mCountDown == 0) {
		mIsDying = true;
	}
	return  (mCountDown < 0)?true: false;
}

bool sensor::Event::isDying()
{
	return mIsDying;
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

int sensor::Event::getElapsedMillis()
{
	return ofGetElapsedTimeMillis() - mLifeCycles;
}

void sensor::Event::draw()
{
	ofDrawCircle(mCenter, mSize);
}

sensor::SensorField::SensorField()
{
	fieldID.set("sensorFieldID", 0, 0, 10);

	limitUp.set("UpperLimit [mm]", -500, 0, -4000);
	limitDown.set("LowerLimit [mm]", -2500, 0, -4000);
	limitLeft.set("LeftLimit [mm]", -2000, -8000, 8000);
	limitRight.set("RightLimit [mm]", 2000, -8000, 8000);

	eventSize.set("Search size", 100, 0, 1000);
	eventRayGap.set("Ray Gap", 5, 0, 20);
	eventBreathSize.set("Cycles to death", 10, 0, 100);
	smoothingPos.set("Smooth Pos", 0.5, 0.0, 1.0);
	smoothingSize.set("Smooth Size", 0.5, 0.0, 1.0);
}

sensor::SensorField::~SensorField()
{
}

void sensor::SensorField::setup(ofxGui &gui, string name)
{
	panel = gui.addPanel();

	panel->loadTheme("theme/theme_light.json");
	panel->setName(name);

	panel->add<ofxGuiIntInputField>(fieldID);

	fieldGroup = panel->addGroup("SensorField");
	fieldGroup->add(limitUp);
	fieldGroup->add(limitDown);
	fieldGroup->add(limitLeft);
	fieldGroup->add(limitRight);

	sensitivityGroup = panel->addGroup("Sensitivity");
	sensitivityGroup->add(eventSize);
	sensitivityGroup->add(eventRayGap);
	sensitivityGroup->add(eventBreathSize);
	sensitivityGroup->add(smoothingPos);
	sensitivityGroup->add(smoothingSize);

	panel->loadFromFile(name + ".xml");


	myfont.load("verdana.ttf", 100);
}

bool sensor::SensorField::update(std::vector<glm::vec3> data)
{
	panel->setHidden(true);

	int minID = 0;

	// we prepare all the previous events
	for (int e = 0; e < events.size(); e++) {
		events[e].prepare();
		minID = (events[e].getID() >= minID)? events[e].getID() + 1: minID;
	}

	int rayGap = 0;
	int counter = 0;
	glm::vec2 hit = glm::vec2();
	glm::vec2 firsthit = glm::vec2();
	glm::vec2 lastthit = glm::vec2();
	// now do our job
	for (int i = 0; i < data.size(); i++) {
		// first check if the point is inside the sensorfield:	
		if (limitDown.get() < data[i].y && data[i].y < limitUp.get() &&
			limitLeft.get() < data[i].x && data[i].x < limitRight.get()) {

				//std::cout << "got event... " << "\n";
			if (counter == 0) {
				firsthit = glm::vec2(data[i].x, data[i].y);
			}
			else {
				lastthit = glm::vec2(data[i].x, data[i].y);
			}
			counter++;
			hit += glm::vec2(data[i].x, data[i].y);
		}
		else {
			if (counter > 0 && rayGap++ > eventRayGap.get()) {
				hit /= counter;
				//std::cout << "got event at " << hit.x << " "  << hit.y << " size: " << counter << "\n";

				int size = glm::distance(firsthit, lastthit);

				for (int e = 0; e < events.size(); e++) {
					if (events[e].isSame(hit)) {
						events[e].update(hit, size, smoothingPos.get(), smoothingSize.get());
						counter = 0;
						hit = glm::vec2();
						break;
					}
				}
				if (counter > 0) {
					// no previous event was found, so create a new one.
					events.push_back(Event(minID, hit, size, eventSize.get(), eventBreathSize.get()));
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

	return (events.size() > 0) ? true : false;
}

void sensor::SensorField::broadcastEvents(ofxOscSender sender, int frameNumber)
{
	ofxOscMessage sensorbox;
	sensorbox.setAddress("/sensorfield/frame/start");
	sensorbox.addIntArg(fieldID.get());
	sensorbox.addIntArg(frameNumber);
	sender.sendMessage(sensorbox);

	for (int e = 0; e < events.size(); e++) {
		if (!events[e].isDying()) {
			sensorbox.clear();
			sensorbox.setAddress("/sensorfield/event");
			sensorbox.addIntArg(fieldID.get());
			sensorbox.addIntArg(events[e].getID());
			sensorbox.addIntArg(events[e].getElapsedMillis());
			sensorbox.addIntArg(events[e].getCenter().x);
			sensorbox.addIntArg(events[e].getCenter().y);
			sensorbox.addIntArg(events[e].getSize());

			sender.sendMessage(sensorbox);
		}
		else {
			sensorbox.clear();
			sensorbox.setAddress("/sensorfield/event/end");
			sensorbox.addIntArg(fieldID.get());
			sensorbox.addIntArg(events[e].getID());
			sensorbox.addIntArg(events[e].getElapsedMillis());

			sender.sendMessage(sensorbox);
		}
	}
	sensorbox.clear();
	sensorbox.setAddress("/sensorfield/frame/end");
	sensorbox.addIntArg(fieldID.get());
	sensorbox.addIntArg(frameNumber);
	sender.sendMessage(sensorbox);
}

void sensor::SensorField::broadcastBox(ofxOscSender sender)
{
	ofxOscMessage sensorbox;
	sensorbox.setAddress("/sensorfield/box");
	sensorbox.addIntArg(fieldID.get());
	sensorbox.addIntArg(limitLeft.get());
	sensorbox.addIntArg(limitRight.get());
	sensorbox.addIntArg(limitUp.get());
	sensorbox.addIntArg(limitDown.get());

	sender.sendMessage(sensorbox);
}

void sensor::SensorField::drawGui()
{
	panel->setHidden(false);
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
	ofNoFill();
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

