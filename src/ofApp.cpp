#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    lidarRangeIndex = 0;
    lidarScale = 0.1;
    upperBorder = 1000;
    lowerBorder = 2000;
    leftBorder = 2000;
    rightBorder = 2000;
    
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
    mDecodeMessage = false;
	mShowGraph = true;
    
    mLeftOverMessage = "";
	mReceiveMessage = "initialValue";

    mTCPClient.setup("192.168.0.11", 10940);
    mTCPClient.setMessageDelimiter("\n");
    
    std::cout << "connecting";
    while(mTCPClient.isConnected() == false)
    {
        std::cout << ".";
        ofSleepMillis(10);
    }
    std::cout << "\nconnected\n";
    
    std::cout << "\nget version info\n";

    //version acquisition ("VV")
    mSendMessage = "VV\n";
    mTCPClient.send(mSendMessage);

    std::cout << "\nstart sensor\n";

    // measurement start ("BM");
    mSendMessage = "BM\n";
    mTCPClient.send(mSendMessage);

    std::cout << "\nsetup done\n";
    
    string message = "";
    do{
        message = mTCPClient.receive();
        std::cout << message << "\n";
    }while(message.find("BM") != 0);
}

//--------------------------------------------------------------
void ofApp::update()
{
    
   if(mTCPClient.isConnected() == true)
    {
        string message;

        do
        {
            message = mTCPClient.receive();

            if(message.find(mSendMessage.substr(0, mSendMessage.length() - 1)) == 0){
                //std::cout << "Echo detected.. " << "\n";
				//for some strange reason the device is only sending the last few packages once the next GD command was received. 
				// it turns out that when the GD echo is received, all packages have arrived and parsing can get started...

				if (mReceiveMessage.length() > 0 && frameCount > 0) {
					//std::cout << "complete message (" << mReceiveMessage.length() << ")" << "\n";
					//std::cout << "frameCount (" << frameCount << ")" << "\n";

					char h30 = '0';

					unsigned int i;
					lidarRangeIndex = 0;

					for (i = 0; i < LIDARRANGE; i++) {
						char val1 = mReceiveMessage[(i * 3)];
						char val2 = mReceiveMessage[(i * 3) + 1];
						char val3 = mReceiveMessage[(i * 3) + 2];

						int val1h = val1 - h30;
						int val2h = val2 - h30;
						int val3h = val3 - h30;

						int final = (val1h << 12) + (val2h << 6) + val3h;

						lidarRange[i] = final;
					}
					//....

					mReceiveMessage = "";
					frameCount = 0;
				}
            }else if(message.find("00P") == 0){
				// toss this
                //std::cout << "empty detected.. " << "\n";
            }else if(message.length() == 5){
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
            }else if(message.length() > 0){
				mReceiveMessage = mReceiveMessage + message.substr(0, message.length() - 1);
                //std::cout << "m("<< message.length() << ") -->" << message << "<--\n";
                frameCount++;
            }else{
                //std::cout << "empty...\n";
            }
        }
        while (message.length() > 0);
  
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

 
    } else {
        std::cout << "connection got terminated... " << "\n";
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if(mShowGraph){
        ofSetColor(255, 0, 0);
        glPushMatrix();
        float up = ofGetHeight() / 2.0 + upperBorder * lidarScale;
        float down = ofGetHeight() / 2.0 + lowerBorder * lidarScale;
        float left = ofGetWidth() / 2. - leftBorder * lidarScale;
        float right = ofGetWidth() / 2. + rightBorder * lidarScale;
        
        ofDrawLine(left, up, right, up);
        ofDrawLine(left, down, right, down);
        ofDrawLine(left, down, left, up);
        ofDrawLine(right, down, right, up);
        
        glPopMatrix();
        
        float posX, posY;
        
        for(unsigned int i = 0; i < LIDARRANGE ;i++ ){
            ofSetColor(255, 0, 255);
            glPushMatrix();
            //glTranslatef(ofGetWidth() / 2., ofGetHeight() / 2.0, 0);
            //ofRotateDeg(0.25 * i + 225.);
            posX = ofGetWidth() / 2. + lidarRange[i] * lidarScale * sin(ofDegToRad(0.25 * i - 135.));
            posY = ofGetHeight() / 2. +lidarRange[i] * lidarScale *  cos(ofDegToRad(0.25 * i - 135.));

            if(posX < right && posX > left && posY < down && posY > up){
				ofSetColor(0, 0, 0);
			}

            ofDrawLine(ofGetWidth() / 2., ofGetHeight() / 2., posX, posY);
            glPopMatrix();
        }
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
