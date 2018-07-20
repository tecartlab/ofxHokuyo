#pragma once

#include "ofxNetwork.h"
#include "ofMain.h"
#include "ofxOpenCv.h"

#define LIDARRANGE 1081

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
protected:
    ofxTCPClient mTCPClient;
    std::string mSendMessage;
    std::string mReceiveMessage;
    std::string mLeftOverMessage;
    bool mDecodeMessage;

    bool mShowGraph;

    int lidarRange[LIDARRANGE];
    int lidarRangeIndex;
    
    int frameCount;
    
    float lidarScale;
    
    float upperBorder, lowerBorder, leftBorder, rightBorder;

};
