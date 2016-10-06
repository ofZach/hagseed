#pragma once

#include "ofMain.h"
#include "spring.h"
#include "particle.h"
#include "baseScene.h"

class springScene : public baseScene{
    
public:
    
    
    typedef struct {
        vector < ofRectangle > rects;
        string s;
        float size;
    } textLine;

    
    void reset(){};
    
    void setup();
    void update();
    void draw();
    
    ofTrueTypeFont font;
    
    
    void computeTextline(string s, textLine & res);
    void drawStringOnLine(textLine & res, ofPolyline myLine);
    
    vector  <textLine > textlines;
    
    ofPolyline prevFrame;
    ofPolyline resampleSmoothed;
    
    void trackContour(ofPolyline & line);
    
    vector <particle> particles;
    vector <spring> springs;
    
    ofMesh m;
    
    
    
};