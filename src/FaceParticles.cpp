#include "FaceParticles.h"

//--------------------------------------------------------------
void FaceParticles::setup(){

    
}

//--------------------------------------------------------------
void FaceParticles::update(ofMesh face, vector<vec2>points, vector <vec3>outLinePoints, vector<vec3>noseBasePoints, ofRectangle box){

    myFaceMesh = face;
    myPoints=points;
    myOutLinePoints=outLinePoints;
    myNoseBasePoints=noseBasePoints;
    myBox=box;
    
}
//--------------------------------------------------------------
void FaceParticles::parSet(){
    particles.clear();
    
//    fbo.begin();
//    ofClear(0);
    ofSeedRandom(30);
    ofPushMatrix();
    ofPushStyle();
    
    ofMesh outline;
    

    
    if(myOutLinePoints.size()>1){
        
        
        
        float Xrange = (distance(myOutLinePoints[10],myOutLinePoints[26]));
        float Yrange = (distance(myOutLinePoints[8],myOutLinePoints[18]));
        
        particles.setMode(OF_PRIMITIVE_LINES);
        
        vec3 TopLeft = myBox.getMin();
        vec3 maxSize = myBox.getMax();
        vec2 cropSize = vec2(maxSize.x-TopLeft.x,maxSize.y-TopLeft.y);
                    
        //base on the uiAmoiunt generate a new mesh point
        for(int i=0 ;i<uiAmount; i++){

            vec2 position = vec2( ofMap(ofNoise(ofRandom(600),ofGetElapsedTimef()*0.06),0,1,TopLeft.x+20,TopLeft.x+cropSize.x-20),
                                  ofMap(ofNoise(ofRandom(600),ofGetElapsedTimef()*0.06),0,1,TopLeft.y,TopLeft.y+cropSize.y));

            particles.addVertex(vec3(position,0));
            particles.addColor(255);
        }
        
        
        // I translate the nosePoint matrix so need to -nosePoint it
        for(int i=0; i<myPoints.size();i++){
            
            float Xmove = (myNoseBasePoints[2].x);
            float Ymove = (myNoseBasePoints[2].y);
            vec2 newPoint = vec2(myPoints[i].x,myPoints[i].y);
            particles.addVertex(vec3(newPoint,0));
             
        }
        
        // count the distance between each meshes point
        for(int i=0; i<particles.getVertices().size();i++){
            auto position = particles.getVertex(i);
            for(int j=0; j<particles.getVertices().size();j++){
                
                auto jDist = particles.getVertex(j);
                auto dist =glm::distance(position, jDist);
        
                if(dist<uiDist){
                particles.addIndex(i);
                particles.addIndex(j);
                }
            }
        }
        
        // draw the meshes points
        for(int i=0; i<particles.getVertices().size();i++){
            ofSetColor(255);
            ofDrawEllipse(particles.getVertex(i), 3,3);
        }
        
        particles.draw();
            
        
    }
    ofPopStyle();
    ofPopMatrix();
}

//--------------------------------------------------------------
void FaceParticles::draw(){

    ofSetLineWidth(3);
    parSet();
    
}
//--------------------------------------------------------------

