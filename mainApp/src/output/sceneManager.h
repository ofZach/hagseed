#pragma once

#include "ofMain.h"
#include "baseScene.h"

class sceneManager {
    
public:
    
    void setup();
    void update();
    void draw();
    void reset();
    
    void advanceScene();
    
    vector < baseScene * > scenes;
    int currentScene;
    
};