#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    lidarScale = 0.1;
    upperBorder = 1000;
    lowerBorder = 2000;
    leftBorder = 2000;
    rightBorder = 2000;
    
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	mShowGraph = true;
    
	lidarOne.setup("192.168.0.11", 10940);

	lidarOne.startSensing();    
  
}

//--------------------------------------------------------------
void ofApp::update()
{
	if (lidarOne.update()) {
		lidarOne.calculateEuclidian(90, 270);
		// whatever you wanna do if a new frame has arrived....
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if(mShowGraph){
        ofSetColor(255, 0, 0);
        float up = ofGetHeight() / 2.0 + upperBorder * lidarScale;
        float down = ofGetHeight() / 2.0 + lowerBorder * lidarScale;
        float left = ofGetWidth() / 2. - leftBorder * lidarScale;
        float right = ofGetWidth() / 2. + rightBorder * lidarScale;
        
        ofDrawLine(left, up, right, up);
        ofDrawLine(left, down, right, down);
        ofDrawLine(left, down, left, up);
        ofDrawLine(right, down, right, up);     

		ofSetColor(255, 0, 255);

		glPushMatrix();

		glTranslatef(ofGetWidth() / 2., ofGetHeight() / 2.0, 0);

		lidarOne.drawLines(lidarScale);

		glPopMatrix();
    }

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    mShowGraph = !mShowGraph;

/*
 string message="";
 for(unsigned int i=0; i<stroke.size(); i++){
 message+=ofToString(stroke[i].x)+"|"+ofToString(stroke[i].y)+"[/p]";
 }
 udpConnection.Send(message.c_str(),message.length());
 */


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
