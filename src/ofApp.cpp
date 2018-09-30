#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    lidarScale = 0.1;
    
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	mShowGraph = true;
    
	lidarOne.setup("192.168.0.11", 10940);

	lidarOne.startSensing();    
  
	sensorOne.setup(gui, "sensorOne");

	setupViewports();
}

//--------------------------------------------------------------
void ofApp::setupViewports() {
	//call here whenever we resize the window

	sensorOne.panel->setWidth(MENU_WIDTH / 5);

	sensorOne.panel->setPosition(ofGetWidth() - MENU_WIDTH / 5 * 5, 20);
	//ofLog(OF_LOG_NOTICE, "ofGetWidth()" + ofToString(ofGetWidth()));

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
	if (lidarOne.update()) {
		if (lidarOne.calculateEuclidian(90, 270, 0, true)) {
			sensorOne.update(lidarOne.getEuclidian());
		}
		// whatever you wanna do if a new frame has arrived....
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if(mShowGraph){


		mainCam.begin(viewMain);
		glPushMatrix();

		glTranslatef(0, 200, 0);
		glRotatef(180, 1, 0, 0);
		glScalef(lidarScale, lidarScale, lidarScale);

		switch (iMainCamera) {
		case 0:
			lidarOne.drawRays();
			sensorOne.drawField();
			break;
		}

		glPopMatrix();
		mainCam.end();

    }

	if (mShowHelp) {
		ofDrawBitmapString(help, 20, VIEWPORT_HEIGHT + 20);
	}

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
		sensorOne.save();
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
