#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    lidarRangeIndex = 0;
    lidarScale = 0.001;
    upperBorder = 1000;
    lowerBorder = 2000;
    leftBorder = 2000;
    rightBorder = 2000;
    
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
    mDecodeMessage = false;
    
    mLeftOverMessage = "";
    
    mTCPClient.setup("192.168.0.11", 10940);
    mTCPClient.setMessageDelimiter("\n");
    
    std::cout << "connecting";
    while(mTCPClient.isConnected() == false)
    {
        std::cout << ".";
        usleep(1000000);
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
                std::cout << "Echo detected.. " << "\n";
            }else if(message.find("00P") == 0){
                std::cout << "empty detected.. " << "\n";
            }else if(message.length() == 5){
                std::cout << "timestamp detected: " << message << "\n";
                frameCount = 0;
            }else if(message.length() > 0){
                std::cout << "m("<<message.length()<<"):>" << message << "<\n";
                mReceiveMessage = mReceiveMessage + message;
                frameCount++;
            }else{
                std::cout << "empty...\n";
            }
        }
        while (message.length() > 0);
        
        if(mReceiveMessage.length() > 0){
            std::cout << "complete message ("<<mReceiveMessage.length()<<")" << "\n";
            std::cout << "frameCount ("<<frameCount<<")" << "\n";
            //frameCount = 0;
        }
        
        /*
         
        frameCount++;
        string message = "";
        
        if(!mDecodeMessage || frameCount > 10){
            mDecodeMessage = true;
            mLeftOverMessage = "";

            std::cout << "last range " << lidarRangeIndex << "\n";

            lidarRangeIndex = 0;

            std::cout << "distance acquisition " << "\n";

            //Distance acquisition ("GD")
            mSendMessage = "GD"; // retrieve distance data
            mSendMessage += "0000"; // start step: 0
            mSendMessage += "1080"; // end step: 100
            mSendMessage += "00"; // cluster count: 0
            mSendMessage += "\n";
            
            mTCPClient.send(mSendMessage);
            
            waitForEcho = true;
            waitForTimeStamp = true;

            frameCount = 0;
        }

        if(mDecodeMessage){
            message = mTCPClient.receive();
            if(waitForEcho){
                if(message.length() > 0){
                    std::cout << "waitForEcho message " << message << "\n";
                    waitForEcho = false;
                }
            }else if(waitForTimeStamp){
                if(message.length() > 0){
                    std::cout << "waitForTimeStamp message " << message << "\n";
                    waitForTimeStamp = false;
                }
            } else{
                int dataIndex = 0;
                int value = 0;
                int readingIndex = 0;
                int lineIndex = 0;
                string readingData;
                int bytesInReading = 3; //SCIP2.0 models reply with 3byte readings; 1.0 use 2 bytes
                
                // read all data coming back until we don’t get anymore
                do {
                    if (message.length() < 0)
                        break;
                    
                    // Grab the readings from the reply, remove the checksum byte
                    readingData = message.substr(0, message.length() - 1);
                    
                    // look for Line feed to get next line in the reading
                    while (readingData[lineIndex] != '\0') {
                        
                        // Decode the byte and accumulate
                        value <<= 6;
                        value &=~0x3f;
                        value |= readingData[lineIndex] - 0x30;
                        ++ lineIndex;
                        
                        readingIndex = (readingIndex+1)% bytesInReading;
                        
                        //if this if the final byte of a distance reading, store it into an array of distances
                        if (readingIndex == 0 && dataIndex < 1) {
                            lidarRange[dataIndex++] = (float) value / 1000.0f;
                            value = 0;
                        } // end decode while
                    } // end readline if
                    message = mTCPClient.receive();
                } while (message.length() > 0); // end read data while
                
                /*
                // the lidar sends its data in packages not larger than 65 bytes
                
                mLeftOverMessage = "";
                
                int zeroCounter = 2;
                // if there is a new package
                while(message.length() > 0){
                    // only go parsing messages if we expect them
                    if(mDecodeMessage == true){
                        if(mReceiveMessage.length() == 0 && message.length() > 60){
                            // the first package has to be larger than 60 -
                            // this assumes that all other non-data packages
                            // sent by the lidar are never bigger than 60 bytes
                            mReceiveMessage = message;
                        } else if(mReceiveMessage.length() > 0){
                            // keep going if there was a previous package
                            mReceiveMessage = message;
                        }
                    } else {
                        std::cout << "rx " << message << "\n";
                    }
                    
                    message = "";
                    
                    if(mDecodeMessage == true && mReceiveMessage.length() > 0)
                    {
                        mReceiveMessage = mLeftOverMessage + mReceiveMessage;
                        
                        char h30 = '0';
                        
                        unsigned int i;
                        
                        for( i = 0; (i * 3 + 3) < mReceiveMessage.length() ;i++ ){
                            char val1 = mReceiveMessage[(i * 3)];
                            char val2 = mReceiveMessage[(i * 3) + 1];
                            char val3 = mReceiveMessage[(i * 3) + 2];
                            
                            int val1h = val1 - h30;
                            int val2h = val2 - h30;
                            int val3h = val3 - h30;
                            
                            int final = (val1h << 12) + (val2h << 6) + val3h;
                            
                            if( final == 0 && zeroCounter != 0 ){
                                zeroCounter--;
                            }
                            if(zeroCounter == 0){
                                lidarRange[lidarRangeIndex++] = final;
                            }
                        }
                        
                        mLeftOverMessage = "";
                        
                        // get all leftover bytes except the last one from the package
                        for(i = i * 3 ; i < mReceiveMessage.length() - 1; i++){
                            mLeftOverMessage = mLeftOverMessage + mReceiveMessage[i];
                        }
                        
                        if(lidarRangeIndex >= LIDARRANGE - 1){
                            // if we have reached our limits, reset
                            std::cout << "..reached range acquisition " << "\n";
                            mDecodeMessage = false;
                            mReceiveMessage = "";
                            break;
                        }
                    } else {
                        mLeftOverMessage = "";
                    }
                    message = mTCPClient.receive();
                }//while loop
         
            }
        }
        */

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

            //81 =0
            if(i == 500){
                std::cout << "lidar raw value: " << lidarRange[i] << "\n";
            }
            if(499 <= i && i <= 501){
                ofSetColor(0, 0, 0);
            }
            
            if(lidarRange[i] == 0){
                //std::cout << "lidar index is zero: " << i << "\n";
            }

            if(posX < right && posX > left && posY < down && posY > up){
            }

            ofDrawLine(ofGetWidth() / 2., ofGetHeight() / 2., posX, posY);
            glPopMatrix();
        }
    }

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    //mShowGraph = !mShowGraph;

    //Distance acquisition ("GD")
    mSendMessage = "GD"; // retrieve distance data
    mSendMessage += "0000"; // start step: 0
    mSendMessage += "1080"; // end step: 100
    mSendMessage += "00"; // cluster count: 0
    mSendMessage += "\n";
    
    mTCPClient.send(mSendMessage);

    
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
