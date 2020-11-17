// ofxFaceTrack2 example: SIMPLE EXAMPLE
//
// This example shows minimal usage of the ofxFaceTracker2 addon,
// doing simple landmark detection and pose estimation
//

#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxFaceTracker2.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxKinect.h"
#include "ofxKinectProjectorToolkit.h"
#include "ofxSecondWindow.h"
#include "ofxPS3EyeGrabber.h"
#include "ofxOsc.h"
#include "ofxDelaunay.h"
#include "Displace.h"
#include "FaceDelaunay.h"
#include "FaceParticles.h"

#define PORT 8001

using namespace ofxCv;
using namespace cv;

using namespace glm;

class ofApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    void getMeshes();
    void fboSetup();
    void drawFbo();
    void displace();
    void oscRecieve();
    
    ofxFaceTracker2 tracker;
    ofVideoGrabber grabber;
    
    ofxSecondWindow             projector;
    
    ofColor                     blobColors[12];
    ofxCv::ContourFinder        contourFinder;
    ofxKinectProjectorToolkit   kpt;
    ofxKinect                   kinect;
    
    ofxCvGrayscaleImage         bgImage;
    ofxCvGrayscaleImage         grayImage;
    ofxCvGrayscaleImage         grayThreshNear;
    ofxCvGrayscaleImage         grayThreshFar;

    vector<glm::vec2> facePoints;
    
    ofVec2f position;
    float scale;
    ofVec3f orientation;
    ofMatrix4x4 rotationMatrix;
    
    vec3 Average;
    
    Mat translation, rotation;
    ofMatrix4x4 pose;

    ofImage img;
    ofMesh triangulated;
    ofxDelaunay delaunay;
    
    ofMesh face;
    ofTexture texture;
    
    ofEasyCam cam;
    
    
    
    // gui parameter
    
//    ofParameter<float> nVal1,nVal2,nVal3, mAmount;
    
    ofParameter<float>          nearThreshold;
//    ofParameter<float>          farThreshold;
    ofParameter<float>          minArea;
    ofParameter<float>          maxArea;
    ofParameter<float>          threshold;
    ofParameter<float>          persistence;
    ofParameter<float>          maxDistance;
    
//    ofParameter<float>          lengthX,lengthY;
//    ofParameter<float>          adjust;
//    ofParameter<float>          paraAmount,paraDist;
    
    ofxPanel myGui;
    
    
    //osc recieve
    
    ofxOscReceiver receiver;
    
    float farThreshold;
    float lengthX,lengthY,adjust,adjust2;
    
    float paraAmount,paraDist;
    
    float nVal1,nVal2,nVal3,mAmount;
    float nWidth;
    
    bool debugMode;
    bool displaceMode, DelaunaryMode, ParticlesMode;
    bool activeCam;
    
    
    
    //fbo part
    ofFbo fbo;
    ofFbo fbo2;
    ofPixels flip;
    ofPixels copyPixels;
    
    ofRectangle box;
    ofImage copyImg;
    
    Displace myDisplace;
    FaceDelaunay myFaceDelaunay;
    FaceParticles myFacePar;
    


};

