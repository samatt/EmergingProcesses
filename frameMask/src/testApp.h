#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxSlitScan.h"
#include "ofxAutoControlPanel.h"
#include "ofxXmlSettings.h"


class testApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    ofVideoGrabber cam;
    ofxCv::FlowFarneback farneback;
    ofFloatImage canvas;
    Boolean firstFrame;
    ofxAutoControlPanel panel;
    
    vector<ofFloatImage> buffer;
    int oldestFrameIndex;

    ofFloatImage accumulation;
    int totalFrames;
    
    ofVec2f dampenedFlow;
    
};
