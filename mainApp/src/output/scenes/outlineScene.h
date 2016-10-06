#pragma once

#include "ofMain.h"
#include "baseScene.h"


typedef struct {
    ofPolyline line;
    int level;
    float pct;
    float distToCenter;
} polyLevel;

class outlineScene : public baseScene {
    
public:
    
    
  
    
    typedef struct {
        string s;
        ofRectangle rect;
        int midIndex;
        float midPct;
        int startIndex;
        float startPct;
        int endIndex;
        float endPct;
        float origDistance;
        float origOffset;
    } letterRect;
    
    ofPoint centerSmooth;
    
    void setup();
    void update();
    void draw();
    void reset(){};
    
    ofImage         person;
    vector < polyLevel > levels;

    ofTrueTypeFont font;
    vector < ofMesh > meshes;
    vector < ofRectangle > rects;
    string ss;
    vector < letterRect > letterRects;
    
    float sizeTarget;
    float sizeAmount;
    
};