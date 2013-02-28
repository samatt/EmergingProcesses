#include "testApp.h"
using namespace ofxCv;
using namespace cv;

void testApp::setup() {
	ofSetVerticalSync(true);
    ofImage distortionMap;
    distortionMap.loadImage("hard_noise.png");
    ofSetBackgroundAuto(false);
    loadCameras();
    //set up a slit scan with a maximum capacity of frames
    // in the distortion buffer
    slitScan.setup(640,480,30,OF_IMAGE_COLOR);
    
    // initialize connection
    for(int i = 0; i < NUM_CAMERAS; i++) {
        IPCameraDef& cam = getNextCamera();
        
        ofxSharedIpVideoGrabber c( new ofxIpVideoGrabber());
        c->setCameraName(cam.name);
        c->setURI(cam.url);
        c->connect(); // connect immediately
        
        // if desired, set up a video resize listener
        ofAddListener(c->videoResized, this, &testApp::videoResized);
        
        ipGrabber.push_back(c);
        
        //diff[i].allocate(c->getWidth(), c->getHeight(), OF_IMAGE_COLOR);
        
    }
    slitScan.setDelayMap(distortionMap);
    //blending means the edges between the scans are feathered
    slitScan.setBlending(true);
    //time delay is the deepest in history the delay can go
    //and width is the number of frames the distortion will encompass
    //note that the delay cannot be more than the total capacity
    slitScan.setTimeDelayAndWidth(15, 15);

	cam.initGrabber(640, 480);
	
	// imitate() will set up previous and diff
	// so they have the same size and type as cam
//	imitate(previousIP, ipGrabber[0]->getPixelsRef());
//    imitate(diffIP, ipGrabber[0]->getPixelsRef());
    imitate(previous, cam);
	imitate(diff, cam);
}
//--------------------------------------------------------------
void testApp::loadCameras() {
    
    // all of these cameras were found using this google query
    // http://www.google.com/search?q=inurl%3A%22axis-cgi%2Fmjpg%22
    // some of the cameras below may no longer be valid.
    
    // to define a camera with a username / password
    //ipcams.push_back(IPCameraDef("http://148.61.142.228/axis-cgi/mjpg/video.cgi", "username", "password"));
    
	ofLog(OF_LOG_NOTICE, "---------------Loading Streams---------------");
    
	ofxXmlSettings XML;
    
	if( XML.loadFile("streams.xml") ){
        
        XML.pushTag("streams");
		string tag = "stream";
		
		int nCams = XML.getNumTags(tag);
		
		for(int n = 0; n < nCams; n++) {
            
            IPCameraDef def;
            
			def.name = XML.getAttribute(tag, "name", "", n);
			def.url = XML.getAttribute(tag, "url", "", n);
			def.username = XML.getAttribute(tag, "username", "", n);
			def.password = XML.getAttribute(tag, "password", "", n);
			
			string logMessage = "STREAM LOADED: " + def.name +
			" url: " +  def.url +
			" username: " + def.username +
			" password: " + def.password;
            
            ofLog(OF_LOG_NOTICE, logMessage);
            
            ipcams.push_back(def);
            
		}
		
		XML.popTag();
		
        
		
	} else {
		ofLog(OF_LOG_ERROR, "Unable to load streams.xml.");
	}
	ofLog(OF_LOG_NOTICE, "-----------Loading Streams Complete----------");
    
    
    nextCamera = ipcams.size();
}

//--------------------------------------------------------------

IPCameraDef& testApp::getNextCamera() {
    nextCamera = (nextCamera + 1) % ipcams.size();
    return ipcams[nextCamera];
}

//--------------------------------------------------------------
void testApp::videoResized(const void * sender, ofResizeEventArgs& arg) {
    // find the camera that sent the resize event changed
    for(int i = 0; i < NUM_CAMERAS; i++) {
        if(sender == &ipGrabber[i]) {
            stringstream ss;
            ss << "videoResized: ";
            ss << "Camera connected to: " << ipGrabber[i]->getURI() + " ";
            ss << "New DIM = " << arg.width << "/" << arg.height;
            ofLogVerbose("testApp") << ss.str();
        }
    }
}
void testApp::update() {
    for(size_t i = 0; i < NUM_CAMERAS; i++) {
        
        // int i = totalFrames % ipGrabber.size();
        ipGrabber[i]->update();
        if(ipGrabber[i]->isFrameNew()) {
            // take the absolute difference of prev and cam and save it inside diff
            absdiff(previous, ipGrabber[i]->getPixelsRef(), diff);
            diff.update();
            
            // like ofSetPixels, but more concise and cross-toolkit
            copy(ipGrabber[i]->getPixelsRef(), previous);
            
            // mean() returns a Scalar. it's a cv:: function so we have to pass a Mat
            diffMean = mean(toCv(diff));
            
            // you can only do math between Scalars,
            // but it's easy to make a Scalar from an int (shown here)
  //          diffMean *= Scalar(10);
            //if(grabber.isFrameNew()){
            slitScan.addImage(diff.getPixels());
            //}
        }
    }

}

void testApp::draw() {
	ofSetColor(255);
	//cam.draw(0, 0);
	//diff.draw(0, 0);
    //ipGrabber[0]->draw(0, 0);
	slitScan.getOutputImage().draw(0, 0);
//    int row = 0;
//    int col = 0;
//    
//    int x = 0;
//    int y = 0;
//    
//    int w = ofGetWidth() / NUM_COLS;
//    int h = ofGetHeight() / NUM_ROWS;
//    
//    float totalKbps = 0;
//    float totalFPS = 0;
//    
//    for(size_t i = 0; i < NUM_CAMERAS; i++) {
//        //for(size_t i = 0; i < ipGrabber.size(); i++) {
//        x = col * w;
//        y = row * h;
//        
//        // draw in a grid
//        row = (row + 1) % NUM_ROWS;
//        if(row == 0) {
//            col = (col + 1) % NUM_COLS;
//        }
//        
//        
//        ofPushMatrix();
//        ofTranslate(x,y);
//        //ipGrabber[i]->draw(0, 0,w,h);
//        
//        //ofSetColor(255,255,255,255);
//        //slits[i].getOutputImage().draw(0, 0, w, h);
//      //  diff[i].draw(0,0,w,h);
//        ofPopMatrix();
//    }
	// use the [] operator to get elements from a Scalar
//	float diffRed = diffMean[0];
//	float diffGreen = diffMean[1];
//	float diffBlue = diffMean[2];
//	
//	ofSetColor(255, 0, 0);
//	ofRect(0, 0, diffRed, 10);
//	ofSetColor(0, 255, 0);
//	ofRect(0, 15, diffGreen, 10);
//	ofSetColor(0, 0, 255);
//	ofRect(0, 30, diffBlue, 10);
}

void testApp::keyPressed(int key){
    if(key == ' ') {
        // initialize connection
        for(int i = 0; i < NUM_CAMERAS; i++) {
            ofRemoveListener(ipGrabber[i]->videoResized, this, &testApp::videoResized);
            ofxSharedIpVideoGrabber c( new ofxIpVideoGrabber());
            IPCameraDef& cam = getNextCamera();
            c->setUsername(cam.username);
            c->setPassword(cam.password);
            URI uri(cam.url);
            c->setURI(uri);
            c->connect();
            
            ipGrabber[i] = c;
            
        }
    }
    
    if(key == 'f'){
        ofToggleFullscreen();
    }
}
