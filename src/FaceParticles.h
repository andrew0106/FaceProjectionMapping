#pragma once

#include "ofMain.h"


using namespace glm;

class FaceParticles {
public:
    
    void setup();
    void update(ofMesh face, vector<vec2>points, vector <vec3>outLinePoints, vector<vec3>noseBasePoints, ofRectangle box);
    void draw();
    void parSet();
    
    vector <glm::vec2> getTriangle(int i);
    float uiAmount,uiDist;
    
    
private:
    vector<vec2> myPoints;
    vector <vec3> myOutLinePoints;
    vector <vec3> myNoseBasePoints;
    
    
    ofImage myGrabberImg;
    ofMesh myFaceMesh;
    ofMesh particles;
    ofRectangle myBox;

    
};
