#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxXmlSettings.h"
#include "ofxAutoControlPanel.h"

class testApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
    
	float extrusionAmount;
	ofVboMesh mainMesh;
	
    ofFbo fbo;
	ofMesh mesh;
    
	ofVideoGrabber vidGrabber;
    ofxCv::FlowFarneback farneback;
	ofxCv::FlowPyrLK pyrLk;
    ofxCv::Flow* curFlow;
    ofxAutoControlPanel panel;
    vector<ofVec2f> dampenedFlow;
    Boolean firstFrame;
};
