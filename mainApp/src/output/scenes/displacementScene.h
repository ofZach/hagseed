#pragma once

#include "ofMain.h"
#include "baseScene.h"
#include "ofxParagraph.h"
#include "ofxCv.h"

class displacementScene : public baseScene {
    
public:
    
    
    typedef struct {
        
        vector < float > pcts;
        vector < float > pctWidths;
        vector < char > chars;
        float origLength;
        
    } line;
    
    
    void setup();
    void update();
    void draw();
    
    void reset(){};
    
    
    ofImage img;
    
    ofImage prevImg;;
    
    vector<ofxParagraph*> paragraphs;
    ofImage spot;
    vector < line > lines;
    vector < ofPoint > offsets;
    
    ofVideoPlayer player1;
    ofVideoPlayer player2;
    
    ofImage grayscale;
    ofImage grayscale2;
    
    float offsetAmount;
    float offsetTarget;
    
    ofxCv::ContourFinder finder;
    
    float diffSmooth;
    int goodDiff;
    
};