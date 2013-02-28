#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxSlitScan.h"
#include "ofxIpVideoGrabber.h"
#include "ofxXmlSettings.h"
#define NUM_CAMERAS 1
#define NUM_COLS 1
#define NUM_ROWS 1
class IPCameraDef {
public:
    IPCameraDef() {
    };
    IPCameraDef(string _uri) {
        url = _uri;
    }
    
    IPCameraDef(string _name, string _uri, string _username, string _password) {
        name = _name;
        url = _uri;
        username = _username;
        password = _password;
    }
    
    string name;
    string url;
    string username;
    string password;
};

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
    void keyPressed(int key);
    void loadCameras();
    IPCameraDef& getNextCamera();
    vector<ofxSharedIpVideoGrabber> ipGrabber;
    vector<IPCameraDef> ipcams;

    int nextCamera;
    void videoResized(const void * sender, ofResizeEventArgs& arg);
	ofVideoGrabber cam;
	ofPixels previous;
    	ofPixels previousIP;
	ofImage diff;
	ofImage diffIP;
    vector<ofImage> diffs;
	
	// a scalar is like an ofVec4f but normally used for storing color information
    ofxSlitScan slitScan;
	cv::Scalar diffMean;
};
