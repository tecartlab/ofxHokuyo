#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    lidarRangeIndex = 0;
    
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
    mDecodeMessage = false;
    
    mLeftOverMessage = "";
    
    mTCPClient.setup("192.168.0.10", 10940);
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
}

//--------------------------------------------------------------
void ofApp::update()
{
    if(mTCPClient.isConnected() == true)
    {
        string message = mTCPClient.receive();
        
        if(message.length() > 0)
        {
            if(mDecodeMessage == true){
                if(mReceiveMessage.length() == 0 && message.length() > 60){
                    mReceiveMessage = message;
                } else if(mReceiveMessage.length() > 0){
                    mReceiveMessage = message;
                }
            } else {
                std::cout << "received " << mReceiveMessage << "\n";
            }
        }
    }
    
    if(mDecodeMessage == true && mReceiveMessage.length() > 32)
    {
        mReceiveMessage = mLeftOverMessage + mReceiveMessage;
        
        char h30 = '0';
    
        std::cout << "received " << mReceiveMessage << "\n";
        std::cout << "received length " << mReceiveMessage.length() << "\n";
        
        unsigned int i;
        
        for( i = 0; (i * 3 + 3) < mReceiveMessage.length() ;i++ ){
            std::cout << "value id " << lidarRangeIndex;
            char val1 = mReceiveMessage[(i * 3)];
            char val2 = mReceiveMessage[(i * 3) + 1];
            char val3 = mReceiveMessage[(i * 3) + 2];
            //std::cout << "| val1 " << val1;
            //std::cout << "| val2 " << val2;
            //std::cout << "| val3 " << val3;
            
            int val1h = val1 - h30;
            int val2h = val2 - h30;
            int val3h = val3 - h30;
            
            //std::cout << "| val1h " << val1h;
            //std::cout << "| val2h " << val2h;
            //std::cout << "| val3h " << val3h;
            
            int final = (val1h << 12) + (val2h << 6) + val3h;
            std::cout << " | " << final << "\n";
            lidarRange[lidarRangeIndex++] = final;
        }
        
        mLeftOverMessage = "";
        for(i = i * 3 ; i < mReceiveMessage.length() - 1; i++){
            mLeftOverMessage = mLeftOverMessage + mReceiveMessage[i];
        }
        /*
         if(mLeftOverMessage.length() > 0){
         std::cout << " >> leftover " << mLeftOverMessage << "\n";
         } else {
         std::cout << " >> no leftover " << "\n";
         }
         */
        if(lidarRangeIndex >= LIDARRANGE - 1){
            lidarRangeIndex = 0;
            mDecodeMessage = false;
            mReceiveMessage = "";
        }
    }
    else {
        mLeftOverMessage = "";
    }

    //std::cout << "connected " << mTCPClient.isConnected() << "\n";
    
//    int recDataCount = mUDPSendConnection.PeekReceive();
//    if(recDataCount > 0)
//    {
//        char udpMessage[100000];
//        mUDPSendConnection.Receive(udpMessage,100000);
//        string message=udpMessage;
//        
//        std::cout << "received " << message << "\n";
//    }
    
//    int recDataCount = mUDPReceiveConnection.PeekReceive();
//    if(recDataCount > 0)
//    {
//        char udpMessage[100000];
//        mUDPReceiveConnection.Receive(udpMessage,100000);
//        string message=udpMessage;
//        
//        std::cout << "received " << message << "\n";
//    }
    
    
/*
 char udpMessage[100000];
 udpConnection.Receive(udpMessage,100000);
 string message=udpMessage;
 if(message!=""){
 stroke.clear();
 float x,y;
 vector<string> strPoints = ofSplitString(message,"[/p]");
 for(unsigned int i=0;i<strPoints.size();i++){
 vector<string> point = ofSplitString(strPoints[i],"|");
 if( point.size() == 2 ){
 x=atof(point[0].c_str());
 y=atof(point[1].c_str());
 stroke.push_back(ofPoint(x,y));
 }
 }
 }
 */
}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{

    mDecodeMessage = true;
    mLeftOverMessage = "";

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
