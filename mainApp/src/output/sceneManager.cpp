

#include "sceneManager.h"

//---------------------------------------------------------------
void sceneManager::setup(){
    
    for (int i = 0; i < scenes.size(); i++){
        scenes[i]->setup();
    }
    
    currentScene = 0;
    
}


//---------------------------------------------------------------
void sceneManager::update(){
    
    scenes[currentScene]->update();

}


//---------------------------------------------------------------
void sceneManager::draw(){
    
    scenes[currentScene]->draw();
    
}

//---------------------------------------------------------------
void sceneManager::reset(){
    
    scenes[currentScene]->reset();
    
}

void sceneManager::advanceScene(){
    currentScene ++;
    currentScene %= scenes.size();
}