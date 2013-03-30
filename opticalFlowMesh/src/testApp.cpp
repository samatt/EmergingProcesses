#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	glEnable(GL_DEPTH_TEST);
    firstFrame = true;
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(0);
    dampenedFlow.resize(76800);
	//initialize the video grabber
	vidGrabber.initGrabber(320,240);
    
    //Control Panel Setup
    panel.setup(ofGetWidth()/2, 800);
    panel.addPanel("Optical Flow");
    panel.addSlider("pyrScale", .5, 0, 1);
    panel.addSlider("levels", 4, 1, 8, true);
    panel.addSlider("winsize", 8, 4, 64, true);
    panel.addSlider("iterations", 2, 1, 8, true);
    panel.addSlider("polyN", 7, 5, 10, true);
    panel.addSlider("polySigma", 1.5, 1.1, 2);
    panel.addToggle("OPTFLOW_FARNEBACK_GAUSSIAN", false);
    panel.addSlider("winSize", 32, 4, 64, true);
    panel.addSlider("maxLevel", 3, 0, 8, true);
    panel.addSlider("maxFeatures", 200, 1, 1000);
    panel.addSlider("qualityLevel", 0.01, 0.001, .02);
    panel.addSlider("minDistance", 4, 1, 16);
    
	//this determines how much we push the meshes out
	extrusionAmount = 300.0;
    
	//store the width and height for convenience
	int width = vidGrabber.getWidth();
	int height = vidGrabber.getHeight();
    fbo.allocate(width, height);
    
	for (int y = 0; y < height; y++){
		for (int x = 0; x<width; x++){
			mainMesh.addVertex(ofPoint(x,y,0));	// mesh index = x + y*width
			// this replicates the pixel array within the camera bitmap...
			mainMesh.addColor(ofFloatColor(0,0,0));  // placeholder for colour data, we'll get this from the camera
		}
	}
	
	for (int y = 0; y<height-1; y++){
		for (int x=0; x<width-1; x++){
			mainMesh.addIndex(x+y*width);				// 0
			mainMesh.addIndex((x+1)+y*width);			// 1
			mainMesh.addIndex(x+(y+1)*width);			// 10
			
			mainMesh.addIndex((x+1)+y*width);			// 1
			mainMesh.addIndex((x+1)+(y+1)*width);		// 11
			mainMesh.addIndex(x+(y+1)*width);			// 10
		}
	}
    
}

//--------------------------------------------------------------
void testApp::update(){
	vidGrabber.update();
	if (vidGrabber.isFrameNew()){
        curFlow = &farneback;
        farneback.setPyramidScale( panel.getValueF("pyrScale") );
        farneback.setNumLevels( panel.getValueF("levels") );
        farneback.setWindowSize( panel.getValueF("winsize") );
        farneback.setNumIterations( panel.getValueF("iterations") );
        farneback.setPolyN( panel.getValueF("polyN") );
        farneback.setPolySigma( panel.getValueF("polySigma") );
        farneback.setUseGaussian(panel.getValueB("OPTFLOW_FARNEBACK_GAUSSIAN"));
        farneback.calcOpticalFlow(vidGrabber);
        if(!firstFrame){

            
            
            //this determines how far we extrude the mesh
            for(int y=0;y<vidGrabber.getHeight();y+=5){
                for(int x=0;x<vidGrabber.getWidth();x+=5){
                    //	for (int i=0; i<vidGrabber.getWidth()*vidGrabber.getHeight(); i++){
                    int index = x +vidGrabber.getWidth()*y;
                    ofFloatColor sampleColor = vidGrabber.getPixelsRef().getColor(x, y);
                    //now we get the vertex aat this position
                    //we extrude the mesh based on it's brightness
                    ofVec3f& vert = mainMesh.getVertices()[index];
                    //vert.z = sampleColor.getBrightness() * extrusionAmount;
                    //			mainMesh.setVertex(i, tmpVec);
                    
                    ofVec2f flowPoint;
                    flowPoint = farneback.getFlowPosition(x, y);
                    //cout<<flowPoint.x<<endl;
                    ofVec2f force =  ofVec2f(x,y)- flowPoint;
                    dampenedFlow[y*320+x] += (force - dampenedFlow[y*320+x])*0.1;
                    float mag =  dampenedFlow[y*320+x].length();
                    //float mag =  force.length();
                    sampleColor.set(255);
                    ofMap(mag, 0, 400, 0, 255);
                    sampleColor.a = mag;

                    mainMesh.setColor(index, sampleColor);
                }
            }
            
        }
        
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	ofEnableAlphaBlending();
	//You can either draw the mesh or the wireframe
	ofPushMatrix();
	ofTranslate(ofGetWidth()/2- vidGrabber.getWidth()/2, ofGetHeight()/2-vidGrabber.getHeight()/2);
	//ofRotate(ofGetMouseY(), 1, 0, 0);
//    	mainMesh.drawFaces();
    mainMesh.draw();
    //mainMesh.drawVertices();
	ofPopMatrix();
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    firstFrame = false;
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){
    
}