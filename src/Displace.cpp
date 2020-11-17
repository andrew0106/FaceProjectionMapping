
#include "Displace.h"


//--------------------------------------------------------------
void Displace::setup(){
    
}

//--------------------------------------------------------------
void Displace::update(ofPixels grabberImg,ofMesh face){
    myGrabberImg.setFromPixels(grabberImg);
    myFaceMesh = face;
    
}

//--------------------------------------------------------------
void Displace::draw(){
    
    ofPushStyle();
    ofMesh finalMesh;
        finalMesh = myFaceMesh;
        
        for(auto& v : myFaceMesh.getVertices()) {
            ofVec2f vp = glm::vec2(v.x,v.y);
            finalMesh.addTexCoord(vp);
        }
    
        
        // modify mesh
        for(auto& v : finalMesh.getVertices()) {
            v += glm::vec3(0,
                           0,
                           ofSignedNoise(v.x*noise1D,v.y*noise2D+v.z*noise2D,ofGetElapsedTimef()*noise3D)*amount);
        }
    
    myGrabberImg.bind();
    finalMesh.drawFaces();
    myGrabberImg.unbind();
    
    ofPopStyle();
    
}


