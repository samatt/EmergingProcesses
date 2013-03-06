#include "testApp.h"

using namespace ofxCv;
using namespace cv;
//--------------------------------------------------------------
void testApp::setup(){
    ofSetVerticalSync(true);
    oldestFrameIndex = 0;
    //dampenedFlow = ofVec2f(0,0);
    buffer.resize(50);
    ofBackground(0);
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
    firstFrame = true;
    cam.initGrabber(320, 240);
    canvas.allocate(320, 240, OF_IMAGE_COLOR_ALPHA);
    accumulation.allocate(
                          canvas.getWidth(), //WIDTH
                          canvas.getHeight(), //HEIGHT
                          canvas.getPixelsRef().getImageType());//PIX TYPE
    
//    for (int y = 0; y<240; y++) {
//        for(int  x =0; x<320;x++){
//            canvas.setColor(x, y, ofColor(0));
//        }
//        
//    }
    
}

//--------------------------------------------------------------
void testApp::update(){
    
    cam.update();
    
    if(cam.isFrameNew()){
        
        farneback.setPyramidScale( panel.getValueF("pyrScale") );
        farneback.setNumLevels( panel.getValueF("levels") );
        farneback.setWindowSize( panel.getValueF("winsize") );
        farneback.setNumIterations( panel.getValueF("iterations") );
        farneback.setPolyN( panel.getValueF("polyN") );
        farneback.setPolySigma( panel.getValueF("polySigma") );
        farneback.setUseGaussian(panel.getValueB("OPTFLOW_FARNEBACK_GAUSSIAN"));
        farneback.calcOpticalFlow(cam);
        if(!firstFrame){
            for (int y = 0; y<240; y++) {
                for(int  x =0; x<320;x++){
                    ofVec2f flowPoint ;
                    flowPoint= farneback.getFlowPosition(x, y);//Offset(x, y);
                    ofVec2f force =  ofVec2f(x,y)- flowPoint;
                    //                    force.normalized();
                    //force.normalized();
                    float mag =  force.length();
                    //cout<<mag<<endl;
                    ofMap(mag, 0, 400, 0, 255);
                    ofFloatColor c = cam.getPixelsRef().getColor(x, y);
                    c.a= mag;
                    //  ofEnableAlphaBlending();
                    canvas.setColor(x, y, c);
                    //                      ofDisableAlphaBlending();
                }
            }
        }
        
        for(int y = 0; y < canvas.getHeight(); y++){
            for (int x = 0; x < canvas.getWidth(); x++){
                //for a description of this algorithim, check out
                //http://en.wikipedia.org/wiki/Moving_average
                
                //first grab the colors
                ofFloatColor currentAverage = accumulation.getColor(x, y);
                ofFloatColor newPixel = canvas.getPixelsRef().getColor(x, y);

                //store the total average in a vector, since float colors are clamped to [0 - 1] range
                ofVec4f currentAccumulation = totalFrames * ofVec4f(currentAverage.r,currentAverage.g,currentAverage.b,currentAverage.a);

                currentAverage.r = (newPixel.r + currentAccumulation.x) / (totalFrames + 1);
                currentAverage.g = (newPixel.g + currentAccumulation.y) / (totalFrames + 1);
                currentAverage.b = (newPixel.b + currentAccumulation.z) / (totalFrames + 1);
                currentAverage.a =newPixel.a;
                //reset the modified current average into the array
                accumulation.setColor(x,y,currentAverage);
                //ofSetColor(currentAverage);
                //ofRect(500, 500, 900, 900);
            }
        }
//        if(oldestFrameIndex >= buffer.size()){
//            vector<ofImage> :: iterator it;
//            buffer.erase(it);
//            
//
//            
//        }
        buffer[oldestFrameIndex].setFromPixels(accumulation.getPixelsRef());
        oldestFrameIndex++;
        totalFrames++;
        accumulation.reloadTexture();
        canvas.reloadTexture();
        if(oldestFrameIndex >= buffer.size()){
            oldestFrameIndex = 0;
        }
        
    }
    
    //firstFrame = false;
}

//--------------------------------------------------------------
void testApp::draw(){
    ofEnableAlphaBlending();
 //   cam.draw(0, 0, 320, 240);
 //   farneback.draw();
    if(buffer[oldestFrameIndex].isAllocated()){
        buffer[oldestFrameIndex].draw(accumulation.getWidth(), 0);
    }
//    canvas.draw(0, 0);
    //ofDisableAlphaBlending();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    firstFrame =false;
    //   ofVec2f flowPoint= farneback.getFlowPosition(1, 1);
    //    cout<<flowPoint.x<<","<<flowPoint.y<<endl;
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
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