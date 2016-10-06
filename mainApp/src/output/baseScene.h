#pragma once

#include "ofMain.h"
#include "inputManager.h"

class baseScene {
    
public:
    
    virtual void setup();
    virtual void update();
    virtual void draw();
    virtual void reset();
    
    
    virtual void mouseDragged(int x, int y, int button){};
    virtual void mousePressed(int x, int y, int button){};
    virtual void mouseReleased(int x, int y, int button){};
    
    inputManager * IM;
    
};