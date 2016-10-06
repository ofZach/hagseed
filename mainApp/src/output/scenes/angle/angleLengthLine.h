#pragma once

#include "ofMain.h"

class angleLengthLine {

public: 
    ofPoint startPoint;
    
    vector < float > angles;
    vector < float > angleDiffs;
    vector < float > distances;
    void clear(){
        angles.clear();
        angleDiffs.clear();
        distances.clear();
    }
    float startTime;
    
    void reverse(){
        ofPolyline temp = convertBack();
        std::reverse(temp.begin(), temp.end());
        convertFrom(temp);
    }
		
    void convertFrom(ofPolyline & line){
        
        
        if (line.getVertices().size() > 2){
            
            angles.clear();
            distances.clear();
            startPoint = line.getVertices()[0];
            angleDiffs.clear();
            
            for (int i = 0; i < line.getVertices().size()-1; i++){
                
                ofPoint diff = line.getVertices()[i+1] - line.getVertices()[i];
                
                float angle = atan2(diff.y, diff.x);
                
                float dist = diff.length();
                angles.push_back(angle);
                distances.push_back(dist);
                
                
                if (i == 0){
                    angleDiffs.push_back(0);
                } else {
                    float diffAngle = angles[i] - angles[i-1];
                    if (diffAngle < -PI) diffAngle += TWO_PI;
                    if (diffAngle > PI) diffAngle -= TWO_PI;
                    
                    angleDiffs.push_back(diffAngle);
                }
            }
            
        }
        
        //startTime = ofGetElapsedTimef();
    }
    
    ofPolyline convertBack(){
        
        
        
        ofPolyline temp;
        
        if (angles.size() < 1) return temp;
        
        
        ofPoint pt = startPoint;
        
        temp.addVertex(pt.x, pt.y);
        
        
        float angle = 0;
        
        for (int i = 0; i < angles.size(); i++){
            
            if (i == 0){
               angle = angles[0];
            } else {
                angle += angleDiffs[i];
            }
    
            pt.x += cos(angle) * 6.0;
            pt.y += sin(angle) * 6.0;
            
            temp.addVertex(pt.x, pt.y);
        }
        
        //cout << temp[0] << endl;
        
        return temp;
        
        //ofEndShape();
        
    }
    
//    void draw(){
//        
//        ofNoFill();
//        ofBeginShape();
//        
//        ofPoint pt = startPoint;
//        
//        ofVertex(pt.x, pt.y);
//        
//    
//        float angle = 0; 
//        
//        for (int i = 0; i < angles.size(); i++){
//            
//            if (i == 0){
//                angle = angles[0];
//            } else {
//                
//                angle += angleDiffs[i] * cos( (ofGetElapsedTimef() - startTime) * 0.5);
//            }
//    
//            pt.x += cos(angle) * distances[i];
//            pt.y += sin(angle) * distances[i];
//            
//            ofVertex(pt.x, pt.y);
//        }
//        
//        ofEndShape();
//    }
    
};
