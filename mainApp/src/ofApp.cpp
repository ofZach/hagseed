#include "ofApp.h"
#include "simpleScene.h"
#include "endlessLineScene.h"
#include "displacementScene.h"
#include "springScene.h"
#include "outlineScene.h"
#include "angleScene.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    
    SM.scenes.push_back(new endlessLineScene());
    SM.scenes.push_back(new displacementScene());
    SM.scenes.push_back(new springScene());
    SM.scenes.push_back(new outlineScene());
    SM.scenes.push_back(new angleScene());
    
    for (auto & scene : SM.scenes){
        scene->IM = &IM;
    }
    
    IM.setup();
    SM.setup();
    bDrawDebug = true;

}

//--------------------------------------------------------------
void ofApp::update(){
    
    IM.update();
    SM.update();

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    
    ofBackground(0);
    if (bDrawDebug){
        IM.draw();
    } else {
        SM.draw();
    }
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    if (key == ' '){
        bDrawDebug = !bDrawDebug;
    }
    
    if (key == 'r'){
        SM.reset();
    }
    
    if (key == OF_KEY_RIGHT){
        SM.advanceScene();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

    SM.scenes[SM.currentScene]->mouseDragged(x,y,button);
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    SM.scenes[SM.currentScene]->mousePressed(x,y,button);
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    SM.scenes[SM.currentScene]->mouseReleased(x,y,button);
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
    IM.changeMovie(dragInfo.files[0]);
}
