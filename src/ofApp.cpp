#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	/***************************************************/
	// GUI
	/***************************************************/

	panel = gui.addPanel();

	panel->loadTheme("theme/theme_light.json");
	panel->setName("Lidar server");

	broadcastGroup = panel->addGroup("Broadcast");
	broadcastGroup->add<ofxGuiTextField>(mBroadcastIP.set("Broadcast IP", "127.0.0.1"));
	broadcastGroup->add<ofxGuiIntInputField>(mBroadcastPort.set("Broadcast Port", 11111, 11110, 11119));

	listenerGroup = panel->addGroup("Listener");
	listenerGroup->add<ofxGuiIntInputField>(mListeningPort.set("Listening Port", 11121, 11120, 11129));

	lidarGroup = panel->addGroup("Lidar");
	lidarGroup->add(mLidarMirror0.set("Mirror 0", false));
	lidarGroup->add(mLidarMirror1.set("Mirror 1", false));
	lidarGroup->add(mLidarMirror2.set("Mirror 2", false));
	lidarGroup->add(mLidarMirror3.set("Mirror 3", false));
	lidarGroup->add(mLidarMirror4.set("Mirror 4", false));


	panel->loadFromFile("server.xml");

	/***************************************************/
	// OSC
	/***************************************************/

	broadcaster.setup(mBroadcastIP.get(), mBroadcastPort.get());
	listener.setup(mListeningPort.get());

    lidarScale = 0.1;
    
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	mShowGraph = true;
 
	/***************************************************/
	// Lidar
	/***************************************************/

	lidar10.setup("192.168.0.10", 10940);
	lidar11.setup("192.168.0.11", 10940);
	lidar12.setup("192.168.0.12", 10940);
	lidar13.setup("192.168.0.13", 10940);
	lidar14.setup("192.168.0.14", 10940);

	lidar10.startSensing();
	lidar11.startSensing();
	lidar12.startSensing();
	lidar13.startSensing();
	lidar14.startSensing();

	/***************************************************/
	// SensorField
	/***************************************************/

	sensor0.setup(gui, "sensor_0");
	sensor1.setup(gui, "sensor_1");
	sensor2.setup(gui, "sensor_2");
	sensor3.setup(gui, "sensor_3");
	sensor4.setup(gui, "sensor_4");
	sensor5.setup(gui, "sensor_5");

	setupViewports();
}

//--------------------------------------------------------------
void ofApp::setupViewports() {
	//call here whenever we resize the window

	sensor0.panel->setWidth(MENU_WIDTH / 6);
	sensor1.panel->setWidth(MENU_WIDTH / 6);
	sensor2.panel->setWidth(MENU_WIDTH / 6);
	sensor3.panel->setWidth(MENU_WIDTH / 6);
	sensor4.panel->setWidth(MENU_WIDTH / 6);
	sensor5.panel->setWidth(MENU_WIDTH / 6);

	sensor0.panel->setPosition(ofGetWidth() - MENU_WIDTH / 6 * 6, 20);
	sensor1.panel->setPosition(ofGetWidth() - MENU_WIDTH / 6 * 5, 20);
	sensor2.panel->setPosition(ofGetWidth() - MENU_WIDTH / 6 * 4, 20);
	sensor3.panel->setPosition(ofGetWidth() - MENU_WIDTH / 6 * 3, 20);
	sensor4.panel->setPosition(ofGetWidth() - MENU_WIDTH / 6 * 2, 20);
	sensor5.panel->setPosition(ofGetWidth() - MENU_WIDTH / 6 * 1, 20);

	//--
	// Define viewports

	float xOffset = VIEWGRID_WIDTH; //ofGetWidth() / 3;
	float yOffset = VIEWPORT_HEIGHT / N_CAMERAS;

	viewMain.x = xOffset;
	viewMain.y = 0;
	viewMain.width = ofGetWidth() - xOffset - MENU_WIDTH; //xOffset * 2;
	viewMain.height = VIEWPORT_HEIGHT;

	for (int i = 0; i < N_CAMERAS; i++) {

		viewGrid[i].x = 0;
		viewGrid[i].y = yOffset * i;
		viewGrid[i].width = xOffset;
		viewGrid[i].height = yOffset;
	}

	//
	//--
}

//--------------------------------------------------------------
void ofApp::update()
{
	if (lidar10.update()) {
		if (lidar10.calculateEuclidian(90, 270, 0, mLidarMirror0.get())) {
			if (sensor0.update(lidar10.getEuclidian())) {
				sensor0.broadcastEvents(broadcaster, lidar10.getTimeStamp());
			}
		}
	}

	if (lidar11.update()) {
		if (lidar11.calculateEuclidian(90, 270, 0, mLidarMirror1.get())) {
			if (sensor1.update(lidar11.getEuclidian())) {
				sensor1.broadcastEvents(broadcaster, lidar11.getTimeStamp());
			}
		}
	}

	if (lidar12.update()) {
		if (lidar12.calculateEuclidian(90, 270, 0, mLidarMirror2.get())) {
			if (sensor2.update(lidar12.getEuclidian())) {
				sensor2.broadcastEvents(broadcaster, lidar12.getTimeStamp());
			}
		}
	}

	if (lidar13.update()) {
		if (lidar13.calculateEuclidian(90, 270, 0, mLidarMirror3.get())) {
			if (sensor3.update(lidar13.getEuclidian())) {
				sensor3.broadcastEvents(broadcaster, lidar13.getTimeStamp());
			}
		}
	}

	if (lidar14.update()) {
		if (lidar14.calculateEuclidian(90, 270, 0, mLidarMirror4.get())) {
			if (sensor4.update(lidar14.getEuclidian())) {
				sensor4.broadcastEvents(broadcaster, lidar14.getTimeStamp());
			}
			if (sensor5.update(lidar14.getEuclidian())) {
				sensor5.broadcastEvents(broadcaster, lidar14.getTimeStamp());
			}
		}
	}

	// listen to messages and respond to them
	updateListener();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if(mShowGraph){


		mainCam.begin(viewMain);
		glPushMatrix();

		//glTranslatef(0, 200, 0);
		//glRotatef(180, 0, 0, 1);
		glScalef(lidarScale, lidarScale, lidarScale);

		switch (iMainCamera) {
		case 0:
			lidar10.drawRays();
			sensor0.drawField();
			sensor0.drawEvents();
			sensor0.drawEventLabels();
			break;
		case 1:
			lidar11.drawRays();
			sensor1.drawField();
			sensor1.drawEvents();
			sensor1.drawEventLabels();
			break;
		case 2:
			lidar12.drawRays();
			sensor2.drawField();
			sensor2.drawEvents();
			sensor2.drawEventLabels();
			break;
		case 3:
			lidar13.drawRays();
			sensor3.drawField();
			sensor3.drawEvents();
			sensor3.drawEventLabels();
			break;
		case 4:
			lidar14.drawRays();
			sensor4.drawField();
			sensor4.drawEvents();
			sensor4.drawEventLabels();
			break;
		case 5:
			lidar14.drawRays();
			sensor5.drawField();
			sensor5.drawEvents();
			sensor5.drawEventLabels();
			break;
		}

		glPopMatrix();
		mainCam.end();

    }

	if (mShowHelp) {
		ofDrawBitmapString(help, 20, VIEWPORT_HEIGHT + 20);
	}

	ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), ofGetWidth() - 200, 10);

}

bool ofApp::updateListener()
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
			sensor0.broadcastBox(broadcaster);
			sensor1.broadcastBox(broadcaster);
			sensor2.broadcastBox(broadcaster);
			sensor3.broadcastBox(broadcaster);
			sensor4.broadcastBox(broadcaster);
			sensor5.broadcastBox(broadcaster);
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
void ofApp::exit()
{
	broadcaster.clear();
	listener.stop();
	lidar10.stopSensing();
	lidar11.stopSensing();
	lidar12.stopSensing();
	lidar13.stopSensing();
	lidar14.stopSensing();
	lidar10.exit();
	lidar11.exit();
	lidar12.exit();
	lidar13.exit();
	lidar14.exit();
}

void ofApp::createHelp() {
	help = string("press v -> to show visualizations\n");
	help += "press s -> to save current settings.\n";
	help += "press l -> to load last saved settings\n";
	help += "press 1 - 6 -> to change the viewport\n";
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	switch (key) {
	case ' ':
		break;

	case'p':
		break;

	case'v':
		mShowGraph = !mShowGraph;
		break;

	case 'o':
		break;

	case 't':
		break;

	case 'r':
		break;

	case 'k':
		break;

	case 's':
		sensor0.save();
		sensor1.save();
		sensor2.save();
		sensor3.save();
		sensor4.save();
		sensor5.save();
		panel->saveToFile("server.xml");
		break;

	case 'l':
		break;

	case 'm':
		break;

	case 'h':
		mShowHelp = !mShowHelp;
		if (mShowHelp) {
			createHelp();
		}
		break;

	case '>':
	case '.':
		//farThreshold ++;
		//if (farThreshold > 255) farThreshold = 255;
		break;

	case '<':
	case ',':
		//farThreshold --;
		//if (farThreshold < 0) farThreshold = 0;
		break;

	case '+':
	case '=':
		//nearThreshold ++;
		//if (nearThreshold > 255) nearThreshold = 255;
		break;

	case '-':
		//nearThreshold --;
		//if (nearThreshold < 0) nearThreshold = 0;
		break;

	case 'w':
		//kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
		break;

	case '0':
		//kinect.setLed(ofxKinect::LED_OFF);
		break;

	case '1':
		iMainCamera = 0;
		//kinect.setLed(ofxKinect::LED_GREEN);
		break;

	case '2':
		iMainCamera = 1;
		//kinect.setLed(ofxKinect::LED_YELLOW);
		break;

	case '3':
		iMainCamera = 2;
		//kinect.setLed(ofxKinect::LED_RED);
		break;

	case '4':
		iMainCamera = 3;
		//kinect.setLed(ofxKinect::LED_BLINK_GREEN);
		break;

	case '5':
		iMainCamera = 4;
		//kinect.setLed(ofxKinect::LED_BLINK_YELLOW_RED);
		break;

	case '6':
		iMainCamera = 5;
		//kinect.setLed(ofxKinect::LED_BLINK_YELLOW_RED);
		break;

	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

string ofApp::getOscMsgAsString(ofxOscMessage m) {
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
