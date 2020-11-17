#pragma once

#include "ofMain.h"

using namespace glm;

class Displace {
public:
    
    void setup();
    void update(ofPixels grabberImg, ofMesh face);
    void draw();
    
    float noise1D,noise2D,noise3D,amount;
    
    
private:
    vector<vec2> myPoints;
    ofImage myGrabberImg;
    ofMesh myFaceMesh;
    
};
