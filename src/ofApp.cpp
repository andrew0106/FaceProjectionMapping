#include "ofApp.h"

/*
 
 Created by Chia Yang Chang
 07/05/2020
 www.andrewtw.com
 
 */

//--------------------------------------------------------------
void ofApp::setup(){
    ofEnableSmoothing();
    ofBackground(0);
    myFacePar.setup();
    
    receiver.setup(PORT);
    
    activeCam=0;
    
    // Setup grabber
//    grabber.setGrabber(std::make_shared<ofxPS3EyeGrabber>());
    grabber.setDeviceID(0);
    grabber.setDesiredFrameRate(30);
    grabber.initGrabber(1280,720);
    
    copyImg.allocate(1280, 720, OF_IMAGE_COLOR);
    
    // set up kinect
    kinect.setRegistration(true);
    kinect.init();
    kinect.open();
    
    grayImage.allocate(kinect.width, kinect.height);
    grayThreshNear.allocate(kinect.width, kinect.height);
    grayThreshFar.allocate(kinect.width, kinect.height);
    
    
    kpt.loadCalibration("calibration.xml");
    
    projector.setup("presentation window", ofGetScreenWidth(), 0, 1280, 720, true);

    myGui.setup();
    
    // the code of comment out gui I switch to the osc controller
    
//    myGui.add(nVal1.set("Noise 1D",0.0025,0.,0.01));
//    myGui.add(nVal2.set("Noise 2D",0.0025,0.,0.01));
//    myGui.add(nVal3.set("Noise 3D",0.075,0.,0.5));
//    myGui.add(mAmount.set("Movement Amount",100,0,500));
    
    myGui.add(nearThreshold.set("nearThresh", 230, 0, 255));
//    myGui.add(farThreshold.set("farThresh", 10, 0, 255));
    myGui.add(minArea.set("minArea", 1000, 0, 5000));
    myGui.add(maxArea.set("maxArea", 70000, 15000, 150000));
    myGui.add(threshold.set("threshold", 15, 1, 100));
    myGui.add(persistence.set("persistence", 15, 1, 100));
    myGui.add(maxDistance.set("maxDistance", 32, 1, 100));
    
//    myGui.add(paraAmount.set("Particle Amount",1,0,500));
//    myGui.add(paraDist.set("Particle Distance",1,0,60));
    
//    myGui.add(lengthX.set("lengthX", 50, 50,300 ));
//    myGui.add(lengthY.set("lengthY", 50, 50,300));
//    myGui.add(adjust.set("adjust", 0, -200,200));

    
    // Setup tracker
    tracker.setup();
    fboSetup();
    
}

//--------------------------------------------------------------
void ofApp::fboSetup(){

    fbo.allocate(1280,720);
    ofClear(0,0,0,0);

}

//--------------------------------------------------------------
void ofApp::update(){
    
    // recieve osc 0 stop the cam
    if(activeCam==0){
        grabber.update();
    }
    
    oscRecieve();
    
    
    //update the parameter
    myDisplace.noise1D=nVal1;
    myDisplace.noise2D=nVal2;
    myDisplace.noise3D=nVal3;
    myDisplace.amount=mAmount;
    
    myFacePar.uiDist=paraDist;
    myFacePar.uiAmount=paraAmount;
    
    
    
    // Update tracker when there are new frames
    if(grabber.isFrameNew()){
        
        // flip the video let the tracker read
        
        flip = grabber.getPixels();
        flip.mirror(false,true);
        tracker.update(flip);
        
        vector<ofxFaceTracker2Instance> instances = tracker.getInstances();
        if (instances.size() == 0) {
            return;
        }
        
        box = instances[0].getBoundingBox();

        ofxFaceTracker2Landmarks landmarks = instances[0].getLandmarks();
        facePoints = landmarks.getImagePoints();
    
        // the method how to pull the feature form the tracker
        ofPolyline noseBase = landmarks.getImageFeature(ofxFaceTracker2Landmarks::NOSE_BASE);
        vector<vec3> tempNoise = noseBase.getVertices();
        
        ofPolyline outLine = landmarks.getImageFeature(ofxFaceTracker2Landmarks::FACE_OUTLINE);
        vector<vec3> tempOutLinePoints = outLine.getVertices();
        
        
        // trying to get the left eye's centre
        
        /*
        ofPolyline leftEye = landmarks.getImageFeature(ofxFaceTracker2Landmarks::LEFT_EYE);
        vector <vec3> leftEyePoints = leftEye.getVertices();
        vec3 total(0,0,0);
        
        if(leftEyePoints.size()>1){
            for(int i=0; i<leftEyePoints.size(); i++){
                total+=leftEyePoints[i];
            }
                Average = total/leftEyePoints.size();
        }
         */
    
        // get the face mesh for the tracker
        
        face = landmarks.getImageMesh();
        ofRectangle bb = instances[0].getBoundingBox();
        myDisplace.update(flip,face);
        myFaceDelaunay.update(flip,face,facePoints);
        myFacePar.update(face,facePoints,tempNoise,tempOutLinePoints,bb);
        
    }
        
        
    //kinect setting
    
    kinect.update();
    
    if(kinect.isFrameNew()) {
        // process kinect depth image
        grayImage.setFromPixels(kinect.getDepthPixels());
        grayThreshNear = grayImage;
        grayThreshFar = grayImage;
        grayThreshNear.threshold(nearThreshold, true);
        grayThreshFar.threshold(farThreshold);
        cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        grayImage.flagImageChanged();
        
        // set contour tracker parameters
        contourFinder.setMinArea(minArea);
        contourFinder.setMaxArea(maxArea);
        contourFinder.setThreshold(threshold);
        contourFinder.getTracker().setPersistence(persistence);
        contourFinder.getTracker().setMaximumDistance(maxDistance);
        
        // determine found contours
        contourFinder.findContours(grayImage);
    }
    
    fbo.begin();
    ofClear(0);
    
    // change the deplay mode
    if(displaceMode==1){
        myDisplace.draw();
    }else if(DelaunaryMode==1){
        myFaceDelaunay.draw();
    }else if(ParticlesMode==1){
        myFacePar.draw();
    }else if(debugMode==1){
        face.drawWireframe();
    }
    fbo.end();
    
    // copy the fbo and only crop the img within the face BoundingBox
    fbo.readToPixels(copyPixels);
    copyImg.setFromPixels(copyPixels);
    vec3 TopLeft = box.getMin();
    vec3 maxSize = box.getMax();
    vec2 cropSize = vec2(maxSize.x-TopLeft.x,maxSize.y-TopLeft.y);
    copyImg.crop(TopLeft.x,TopLeft.y,cropSize.x,cropSize.y+30);


}
//--------------------------------------------------------------
void ofApp::drawFbo(){
    
    // heres the place draw the face meshes wireframe to debug the position of each face meshes
    
    fbo.begin();
    ofClear(0);
    
        ofMesh finalMesh;
       finalMesh = face;

       for(auto& v : face.getVertices()) {
           ofVec2f vp = glm::vec2(v.x,v.y);
           finalMesh.addTexCoord(vp);
       }
       
       // modify mesh
       for(auto& v : finalMesh.getVertices()) {
           v += vec3(0,
                     0,
                     ofSignedNoise(v.x*nVal1,v.y*nVal2+v.z*nVal2,ofGetElapsedTimef()*nVal3)*mAmount);
       }
       
       finalMesh.drawWireframe();
    
    fbo.end();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    // mirror the grabber
    grabber.draw(grabber.getWidth(),0,-grabber.getWidth(),grabber.getHeight());


    fbo.draw(0,0);
    ofPushStyle();
    ofNoFill();
    ofDrawRectangle(box);
    ofPopStyle();
    
    
    myGui.draw();

    // the place drawing the presentation window
    // map contour using calibration and draw to presentation window
    
    ofPushMatrix();
    projector.begin();
    ofClear(0);
    float s = ofGetWidth()/kinect.getWidth();
    ofScale(s);
    
    // draw the contourFinder debug
    // contourFinder.draw();
    
    RectTracker& tracker = contourFinder.getTracker();
    
    for(int i = 0; i < contourFinder.size(); i++) {
        
        // get contour, label, center point, and age of contour
        vector<cv::Point> points = contourFinder.getContour(i);
        int label = contourFinder.getLabel(i);
        vector<ofPoint>center;
        center.push_back(toOf(contourFinder.getCenter(i)));
        int age = tracker.getAge(label);
        
        vector<ofRectangle> box;
        box.push_back(toOf(contourFinder.getBoundingRect(i)));
        
        
        ofPushStyle();
        ofSetRectMode(OF_RECTMODE_CENTER);
        
        
        // mapping the centerpoint to the collaborate point
        // this line of code is still testing but with the condition of my room is not big enought I cant test
        
        /*
        for(int j=0;j<center.size();j++){
//            ofVec3f worldPoint = kinect.getWorldCoordinateAt(center[j].x, center[j].y);
//            ofVec2f projectedPoint = kpt.getProjectedPoint(worldPoint);
//            // draw the fbo to the blob centre point and adjust the location
//            copyImg.draw(projectedPoint.x,projectedPoint.y+adjust,lengthX,lengthY+30);
//
//        }
         */

        
        
        // using counterBox to cauculate the center 
        
                for(int j=0;j<box.size();j++){
                    vec3 boxLeft = box[0].getTopRight();
                    vec3 boxRight = box[0].getTopLeft();
                    float boxXCenter = (boxLeft.x+boxRight.x)/2;
                    float boxTop = box[0].getTop();
                    
                    ofSetLineWidth(nWidth);
                    copyImg.draw(boxXCenter+adjust2,boxTop+adjust,lengthX,lengthY+30);
                }
        
        ofPopStyle();
        
    }
    projector.end();
    ofPopMatrix();
    
    // display the second window
    projector.show();
    
        
}

//--------------------------------------------------------------
void ofApp::oscRecieve(){
    
    // check for waiting messages
    while(receiver.hasWaitingMessages()){
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(m);
        if(m.getAddress() == "/debug/fader28"){
            
            farThreshold = m.getArgAsFloat(0);
            
        }else if(m.getAddress() == "/debug/fader25"){
            
            adjust = m.getArgAsFloat(0);
            
        }else if(m.getAddress() == "/debug/fader26"){
            
            lengthX = m.getArgAsFloat(0);
            
        }else if(m.getAddress() == "/debug/fader27"){
            
            lengthY = m.getArgAsFloat(0);
            
        }else if(m.getAddress() == "/debug/push1"){
            
            debugMode = m.getArgAsFloat(0);
            
        }else if(m.getAddress() == "/control/multitoggle2/1/1"){
            
            displaceMode = m.getArgAsFloat(0);
            
        }else if(m.getAddress() == "/control/multitoggle2/1/2"){
            
            DelaunaryMode = m.getArgAsFloat(0);
            
        }else if(m.getAddress() == "/control/multitoggle2/1/3"){
            
            ParticlesMode = m.getArgAsFloat(0);
            
        }else if(m.getAddress() == "/control/multifader2/2"){
            
            nVal1 = m.getArgAsFloat(0);
            
        }else if(m.getAddress() == "/control/multifader2/1"){
            
            nVal2 = m.getArgAsFloat(0);
            
        }else if(m.getAddress() == "/control/fader36"){
            
            nVal3 = m.getArgAsFloat(0);
            
        }else if(m.getAddress() == "/control/fader35"){
            
            mAmount = m.getArgAsFloat(0);
            
        }else if(m.getAddress() == "/control/fader39"){
            
            paraAmount = m.getArgAsFloat(0);
            
        }else if(m.getAddress() == "/control/fader40"){
            
            paraDist = m.getArgAsFloat(0);
            
        }else if(m.getAddress() == "/debug/toggle1"){
            
            activeCam = m.getArgAsFloat(0);
            
        }else if(m.getAddress() == "/debug/fader41"){
            
            adjust2 = m.getArgAsFloat(0);
            
        }else if(m.getAddress() == "/control/fader42"){
            
            nWidth = m.getArgAsFloat(0);
            
        }
        
        
        
    }

    
    
}
