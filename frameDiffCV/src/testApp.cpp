#include "testApp.h"
using namespace ofxCv;
using namespace cv;

void testApp::setup() {
	ofSetVerticalSync(true);
    firstFrame = true;
    
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
	
	panel.addToggle("useFarneback", true);
	panel.addSlider("winSize", 32, 4, 64, true);
	panel.addSlider("maxLevel", 3, 0, 8, true);
	
	panel.addSlider("maxFeatures", 200, 1, 1000);
	panel.addSlider("qualityLevel", 0.01, 0.001, .02);
	panel.addSlider("minDistance", 4, 1, 16);
    
    
    //init slitScan stuff
    ofImage distortionMap;
    distortionMap.loadImage("hard_noise.png");
    slitScan.setup(640,480,30,OF_IMAGE_COLOR);
    slitScan.setDelayMap(distortionMap);
    slitScan.setBlending(true);
    slitScan.setTimeDelayAndWidth(15, 15);
    
    
    // initialize I[P cams and connection
    loadCameras();
    for(int i = 0; i < NUM_CAMERAS; i++) {
        IPCameraDef& cam = getNextCamera();
        
        ofxSharedIpVideoGrabber c( new ofxIpVideoGrabber());
        c->setCameraName(cam.name);
        c->setURI(cam.url);
        c->connect(); // connect immediately
        
        // if desired, set up a video resize listener
        ofAddListener(c->videoResized, this, &testApp::videoResized);
        ipGrabber.push_back(c);
    }
    diffIP.allocate(640, 480, OF_IMAGE_COLOR);
	cam.initGrabber(640, 480);
	curFlow= &farneback;
    //TODO: Fix this so that i dont need to use a grabber.
    imitate(previous, cam);
    imitate(diff, cam);
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
//--------------
void testApp::update() {
    for(size_t i = 0; i < NUM_CAMERAS; i++) {
        
        // int i = totalFrames % ipGrabber.size();
        ipGrabber[i]->update();
        cam.update();
        //if(ipGrabber[i]->isFrameNew()) {
        if(cam.isFrameNew()) {
            //Optical Flow Stuff
            
            if(panel.getValueB("useFarneback")) {
                curFlow = &farneback;
                farneback.setPyramidScale( panel.getValueF("pyrScale") );
                farneback.setNumLevels( panel.getValueF("levels") );
                farneback.setWindowSize( panel.getValueF("winsize") );
                farneback.setNumIterations( panel.getValueF("iterations") );
                farneback.setPolyN( panel.getValueF("polyN") );
                farneback.setPolySigma( panel.getValueF("polySigma") );
                farneback.setUseGaussian(panel.getValueB("OPTFLOW_FARNEBACK_GAUSSIAN"));
               
                farneback.calcOpticalFlow(cam);
                diff = cam.getPixelsRef();

                //farneback.calcOpticalFlow(ipGrabber[i]->getPixelsRef());
                diff = ipGrabber[i]->getPixelsRef();
                
                if(!firstFrame){
                    for(int y=0;y<480;y+=10){
                        for(int x=0;x<640;x+=10){
                            ofVec2f flowPoint;
                            flowPoint = farneback.getFlowPosition(x, y);
                            farneback.getFlowOffset(x, y);
                            ofColor color =  diff.getPixelsRef().getColor(flowPoint.x, flowPoint.y);
                            colors[y*640+x] = color;
                            diffIP.setColor(flowPoint.x, flowPoint.y, color);
                            
                            
                        }
                    }
                }

                diffIP.reloadTexture();
                
                firstFrame = false;
                
            } else {
                curFlow = &pyrLk;
                pyrLk.setMaxFeatures( panel.getValueI("maxFeatures") );
                pyrLk.setQualityLevel( panel.getValueF("qualityLevel") );
                pyrLk.setMinDistance( panel.getValueF("minDistance") );
                pyrLk.setWindowSize( panel.getValueI("winSize") );
                pyrLk.setMaxLevel( panel.getValueI("maxLevel") );
            }
            
            
            //check it out that that you can use Flow polymorphically
           // curFlow->calcOpticalFlow(ipGrabber[i]->getPixelsRef());
                
            //Absolute differencing
            //            absdiff(previous, ipGrabber[i]->getPixelsRef(), diff);
            //            diff.update();
            //            copy(ipGrabber[i]->getPixelsRef(), previous);
            //            diffMean = mean(toCv(diff));
            //          diffMean *= Scalar(10);
            
            //            slitScan.addImage(diff.getPixels());
        }
    }
    
}

void testApp::draw() {
    ofBackground(0);
	ofSetColor(255);
    cam.draw(0, 0);

    farneback.draw();
    diffIP.draw(cam.getWidth(), 0);
    //curFlow->draw(0,0,640,480)
    //diffIP.draw(ipGrabber[0]->getWidth(), 0 );
	//diff.draw(0, 0);
    
	//slitScan.getOutputImage().draw(0, 0);
    
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
