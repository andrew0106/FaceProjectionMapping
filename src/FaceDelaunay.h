#pragma once

#include "ofMain.h"
#include "ofxDelaunay.h"

using namespace glm;

class FaceDelaunay {
public:
    
    void setup();
    void update(ofPixels grabberImg, ofMesh face, vector<vec2>points);
    void draw();
    
    vector <glm::vec2> getTriangle(int i);
    float noise1D,noise2D,noise3D,amount;
    
    
private:
    vector<vec2> myPoints;
    ofImage myGrabberImg;
    ofMesh myFaceMesh;
    ofxDelaunay delaunay;
    
};
