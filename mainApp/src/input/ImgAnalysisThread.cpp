/*
 * ImgAnalysisThread.cpp
 *
 *  Created on: Oct 8, 2014
 *      Author: arturo
 */

#include "ImgAnalysisThread.h"
#include "ofConstants.h"
#include "ctmf.h"

ImgAnalysisThread::ImgAnalysisThread()
:newFrame(true){
	// start the thread as soon as the
	// class is created, it won't use any CPU
	// until we send a new frame to be analyzed
    medianFilterAmount = 3;
	startThread();
    //pixels.allocate(512, 424, OF_PIXELS_GRAY);
}

ImgAnalysisThread::~ImgAnalysisThread(){
	// when the class is destroyed
	// close both channels and wait for
	// the thread to finish
	toAnalyze.close();
	analyzed.close();
	waitForThread(true);
}

void ImgAnalysisThread::analyze(ofPixels & pixels){
	// send the frame to the thread for analyzing
	// this makes a copy but we can't avoid it anyway if
	// we want to update the grabber while analyzing
    // previous frames
	toAnalyze.send(pixels);
}

void ImgAnalysisThread::update(){
	// check if there's a new analyzed frame and upload
	// it to the texture. we use a while loop to drop any
	// extra frame in case the main thread is slower than
	// the analysis
	// tryReceive doesn't reallocate or make any copies
	newFrame = false;
	while(analyzed.tryReceive(pixels)){
		newFrame = true;
	}
//	if(newFrame){
//        if(!texture.isAllocated()){
//            texture.allocate(pixels);
//        }
//		texture.loadData(pixels);
//	}
}

bool ImgAnalysisThread::isFrameNew(){
	return newFrame;
}

ofPixels & ImgAnalysisThread::getPixels(){
	return pixels;
}

ofTexture & ImgAnalysisThread::getTexture(){
	return texture;
}

void ImgAnalysisThread::draw(float x, float y){
    if(texture.isAllocated()){
        texture.draw(x,y);
    }else{
        ofDrawBitmapString("No frames analyzed yet", x+20, y+20);
    }
}

void ImgAnalysisThread::draw(float x, float y, float w, float h){
    if(texture.isAllocated()){
        texture.draw(x,y,w,h);
    }else{
        ofDrawBitmapString("No frames analyzed yet", x+20, y+20);
    }
}

void ImgAnalysisThread::threadedFunction(){
    // wait until there's a new frame
    // this blocks the thread, so it doesn't use
    // the CPU at all, until a frame arrives.
    // also receive doesn't allocate or make any copies
    ofPixels returnPixels;
    returnPixels.allocate(512, 424, OF_PIXELS_GRAY);
    ofPixels pixels;
    while(toAnalyze.receive(pixels)){
        // we have a new frame, process it, the analysis
        // here is just a thresholding for the sake of
        // simplicity

        
        milton::ctmf(pixels.getData(), returnPixels.getData(),
             512, 424, 512, 512,medianFilterAmount, 1);
//
        // once processed send the result back to the
        // main thread. in c++11 we can move it to
        // avoid a copy
#if __cplusplus>=201103
        analyzed.send(std::move(returnPixels));
#else
        analyzed.send(returnPixels);
#endif
	}
}

