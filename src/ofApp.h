#pragma once

#include "ofxNetwork.h"
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxHokuyo.h"

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
 
	lidar::ofxHokuyo lidarOne;

    bool mShowGraph;

    float lidarScale;
    
    float upperBorder, lowerBorder, leftBorder, rightBorder;

};
