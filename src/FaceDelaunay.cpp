#include "FaceDelaunay.h"

//--------------------------------------------------------------
void FaceDelaunay::setup(){
    
}

//--------------------------------------------------------------
void FaceDelaunay::update(ofPixels grabberImg,ofMesh face,vector<vec2>points){
    myGrabberImg.setFromPixels(grabberImg);
    myFaceMesh = face;
    myPoints=points;
    
    delaunay.reset();
    
        for(int i=0; i<myPoints.size();i++){
            
            delaunay.addPoint(myPoints[i]);
            }
        delaunay.triangulate();
    
    
}

//--------------------------------------------------------------
void FaceDelaunay::draw(){
    
    ofPushStyle();
    
    if(myPoints.size()>1){
            
           ofSetColor(255,0,0);
           ofFill();
    
           for (int g=0; g<delaunay.getNumTriangles(); g++){      // loop over the triangles
               vector <glm::vec2> pts = getTriangle(g);           // extract the vector with 3 points
               float x = (pts[0].x +pts[1].x +pts[2].x)/3;
               float y = (pts[0].y +pts[1].y +pts[2].y)/3;
               ofColor a = myGrabberImg.getPixels().getColor(x,y);
               ofSetColor(a);
               ofDrawTriangle(pts[0], pts[1], pts[2]);             // use this point to draw a triangle
            }
        }
    ofPopStyle();
}

//--------------------------------------------------------------
vector <glm::vec2> FaceDelaunay::getTriangle(int i){
    int pA = delaunay.triangleMesh.getIndex(i*3);
    int pB = delaunay.triangleMesh.getIndex(i*3+1);
    int pC = delaunay.triangleMesh.getIndex(i*3+2);

    glm::vec2 pointA = delaunay.triangleMesh.getVertex(pA);
    glm::vec2 pointB = delaunay.triangleMesh.getVertex(pB);
    glm::vec2 pointC = delaunay.triangleMesh.getVertex(pC);

    vector <glm::vec2> points;
    points.push_back(pointA);
    points.push_back(pointB);
    points.push_back(pointC);
    return points;
}
