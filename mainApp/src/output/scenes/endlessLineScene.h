#pragma once

#include "ofMain.h"
#include "baseScene.h"



class endlessLineScene : public baseScene {
    
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
    
    
    
    void setup();
    void update();
    void draw();
    void reset();
    
    ofPoint getMixedPt(int index, float pct, ofPolyline & myLine);
    
    ofEasyCam cam;
    ofPolyline line;
    ofPoint pt;
    double time;
    float lastFrameTime;
    ofPoint smooth;
    ofTrueTypeFont font;
    vector < ofMesh > meshes;
    vector < ofRectangle > rects;
    string ss;
    vector < letterRect > letterRects;
    int counter = 0;
    
};