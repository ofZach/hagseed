#pragma once

#include "ofMain.h"
#include "baseScene.h"
#include "angleLengthLine.h"

class angleScene : public baseScene {
    
public:
    
    void setup();
    void update();
    void draw();
    void reset(){};
    
    
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    
    angleLengthLine aline;
    angleLengthLine alineTarget;
    angleLengthLine alineTargetReversed;
    
    ofPolyline templine;
    
    ofTrueTypeFont apercu;
    
    float targetAngle;
    float targetAngleSmooth;
    float targetAngleSmooth2;
    
    float scaleSmooth;
    
    float scaleSmooth2;
    
    
    ofPoint smooth;
    float lockPct;
    
    float angleLock;
    
    
    
    void drawStringOnLine(string myString, ofPolyline myLine);

};