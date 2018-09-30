#pragma once

#include "ofxNetwork.h"
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxHokuyo.h"
#include "Sensor.h"

#define LIDARRANGE 1081

#define N_CAMERAS 5

#define VIEWGRID_WIDTH  132
#define MENU_WIDTH      1000
#define VIEWPORT_HEIGHT 480

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
	void createHelp();
	string help;

	void setupViewports();
	ofRectangle viewMain;
	ofRectangle viewGrid[N_CAMERAS];

	ofEasyCam mainCam;
	ofCamera previewCams[N_CAMERAS];

	lidar::ofxHokuyo lidarOne;

	sensor::SensorField sensorOne;

	int iMainCamera;

    bool mShowGraph, mShowHelp;

    float lidarScale;
    
	ofxGui gui;

};
