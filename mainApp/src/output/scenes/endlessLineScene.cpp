

#include "endlessLineScene.h"




//---------------------------------------------------------------
void endlessLineScene::setup(){
    
    
    pt.set(0,0,0);
    time = 0;
    lastFrameTime = 0;
    
    
    font.load("fonts/DroidSerif-Bold.ttf", 34);
    
    
    ss = "There's work to be done, there are plots to be plotted, there are scams to be scammed, there are villains to be misled!               Tip of the tongue, top of the teeth.      Testing the tempestuous teapot.      She sells seashells by the seashore.      There.      Not a syllable fluffed.       He can still do it.      He'll pull it off, despite all obstacles.      Charm the pants off them at first, not that he'd relish the resulting sight.      Wow them with wonder, as he says to his actors.      Let's make magic!       And let's shove it down the throat of that devious, twisted bastard, Tony.           ";
    
    ofMesh m = font.getStringMesh(ss, 0, 0);
    
    
    for (int i = 0; i < m.getVertices().size(); i+=4){
        //ofCircle(m.getVertices()[i], 3);
        ofRectangle r;
        r.setPosition(m.getVertices()[i]);
        r.growToInclude(m.getVertices()[i+1]);
        r.growToInclude(m.getVertices()[i+2]);
        r.growToInclude(m.getVertices()[i+3]);
        rects.push_back(r);
    }
    
    
    for (int z = 0; z < 256; z++){
        string s = "";
        s += z;
        ofMesh mTemp = font.getStringMesh(s, 0, 0);
        ofPoint midPt;
        for (int i = 0; i < mTemp.getVertices().size(); i++){
            midPt += mTemp.getVertices()[i];
        }
        midPt /= (float)mTemp.getVertices().size();
        for (int i = 0; i < mTemp.getVertices().size(); i++){
            mTemp.getVertices()[i] -= midPt;
        }
        meshes.push_back(mTemp);
    }

    

}

ofPoint endlessLineScene::getMixedPt(int index, float pct, ofPolyline & myLine){
    
    if (index < 0) return myLine[0];
    if (index >= myLine.size()-1) return myLine[myLine.size()-1];
    
    ofPoint a = myLine[index];
    ofPoint b = myLine[index+1];
    return a + (b-a)*pct;
}

//---------------------------------------------------------------
void endlessLineScene::update(){
    
    //cout << ofGetMouseX() << endl;
    
    for (int i = 0; i < 5; i++){
        //float currentTime = ofGetElapsedTimef();
        time += (counter/60.0) * ofGetMouseX() * 0.000003;
        counter ++;
        //lastFrameTime = currentTime;
        
        //ofPoint scaledPt = pt * 0.1;
        
        float xAdd = sin(pt.y*0.01 + time);
        float yAdd = sin(pt.z*0.01 + 10000 + time);
        float zAdd = sin(pt.x*0.01 +  -10000 + time);
        
        pt += ofPoint(xAdd, yAdd, zAdd) * 1.8;
        line.addVertex(pt);
    }
    
    //    if (line.size() > 10){
    //    ofPolyline smoothed = line.getSmoothed(3.0);
    //    for (int i = 0; i < line.size(); i++){
    //        float pct = ofMap(i, 0, line.size()-1, 1,0, true);
    //        line[i] = (1-pct) * line[i] + (pct) * smoothed[i];
    //    }
    //    }
    
    // line = line.getSmoothed(3);
    
    //cout << line.getPerimeter() << endl;
    
    float totalWidth = rects[rects.size() -1].x + rects[rects.size() -1].width;
    float distance = fmod(line.getPerimeter(), totalWidth);
    int amount = (int)(line.getPerimeter() / totalWidth);
    //cout << amount <<  " " << distance << endl;
    
    // find the closest letter to the end
    int lastRect = 0;
    for (int i = 0; i < rects.size(); i++){
        if ((rects[i].x + rects[i].width) < distance){
            lastRect = i;
        }
    }
    // let's add all the letters we've found from the last time.
    
    int weCanGoTo = (amount * rects.size() + lastRect);
    int weStartFrom = letterRects.size();
    for (int i = weStartFrom; i < weCanGoTo; i++){
        // now, let's figure out WHERE this letter rect is in terms of distance
        int whichPass = (int)(i / rects.size() );
        int which     = (int)(i % rects.size() );
        float midPt = whichPass * totalWidth + rects[which].getCenter().x;
        float startPt = whichPass * totalWidth + rects[which].x;
        float endPt = whichPass * totalWidth + rects[which].x + rects[which].width;
        
        
        // now, a linear search to figure out where this puts us:
        
        int goodIndex = -1;
        float goodPct = 0;
        float totalLength = 0;
        
        int goodStartIndex = -1;
        float goodStartPct = 0;
        
        int goodEndIndex = -1;
        float goodEndPct = 0;
        
        for (int j = 0; j < line.size()-1; j++){
            
            float segmentLength = (line[j] - line[j+1]).length();
            
            if (midPt >= totalLength && midPt < (totalLength + segmentLength)){
                goodIndex = j;
                goodPct = (midPt - totalLength) / (segmentLength);
            }
            
            if (startPt >= totalLength && startPt < (totalLength + segmentLength)){
                goodStartIndex = j;
                goodStartPct = (startPt - totalLength) / (segmentLength);
            }
            
            if (endPt >= totalLength && endPt < (totalLength + segmentLength)){
                goodEndIndex = j;
                goodEndPct = (endPt - totalLength) / (segmentLength);
            }
            totalLength += segmentLength;
        }
        letterRect lr;
        lr.midIndex = goodIndex;
        lr.midPct = goodPct;
        
        lr.startIndex = goodStartIndex;
        lr.startPct = goodStartPct;
        
        lr.endIndex = goodEndIndex;
        lr.endPct = goodEndPct;
        
        ofPoint start = getMixedPt(goodStartIndex, goodStartPct, line);
        ofPoint end = getMixedPt(goodEndIndex, goodEndPct, line);
        
        float origDist = (end-start).length();
        lr.origDistance = origDist;
        lr.s = "";
        lr.s += ss[which];
        lr.origOffset = rects[which].getCenter().y;
        letterRects.push_back(lr);
        
        //cout << i << " " << goodIndex << endl;
    }
    
    //cout << "lastRect = " << lastRect << " " << (amount * rects.size() + lastRect) <<  endl;
    
}


//---------------------------------------------------------------
void endlessLineScene::draw(){
    
    smooth = 0.95 * smooth + 0.05 * line[line.size()-1];
    
    
    //cam.rotate(1, ofPoint(0,1,0));
    
    ofPolyline pp;
    
    
    //cam.pan(mouseX);
    
    ofMatrix4x4 mat;
    mat.makeRotationMatrix(180 * sin(ofGetElapsedTimef()*0.4), ofPoint(0, 1, 0));
    cam.begin();
    
    
    ofPolyline temp;
    temp = line;
    
    for (auto & a: temp){
        a -= smooth;
        a = a * mat;
    }
    ofPushMatrix();
    //ofMultMatrix(mat);
    //ofRotate(sin(180 + ofGetElapsedTimef()) * 20, 0, 1, 0);
    // temp.draw();
    
    for (auto & a: temp){
        ofPoint ppTemp = cam.worldToScreen(a);
        ppTemp.z = 0;;
        pp.addVertex(ppTemp);
        
    }
    
    ofPopMatrix();
    
    //ofTranslate(-smooth.x, -smooth.y);
    //line.draw();
    cam.end();
    
    
    
    font.getFontTexture().bind();
    
    // float angle = 0;
    float angleSmooth = 0;
    float scaleSmooth = 0;
    bool bStart = true;
    
    int start = MAX(0, letterRects.size()-400);
    for (int i = 0; i < letterRects.size(); i++){
        
        if (i < start){
            continue;
        }
        
        float pct = ofMap(i, start, letterRects.size(), 0, 1);
        ofPoint start = getMixedPt(letterRects[i].startIndex, letterRects[i].startPct, pp);
        ofPoint mid = getMixedPt(letterRects[i].midIndex, letterRects[i].midPct, pp);
        ofPoint end = getMixedPt(letterRects[i].endIndex, letterRects[i].endPct, pp);
        
        int which = letterRects[i].s[0];
        //cout << start << " " << mid << " " << end << endl;
        //ofCircle(mid, 3);
        //ofLine(start, end);
        ofPoint diff = end - start;
        float angle = atan2(diff.y, diff.x);
        
        
        ofPoint offset = (end - start).getNormalized().rotate(90, ofPoint(0,0,1));
        
        float length = letterRects[i].origOffset;
        
        float ww =  (meshes[which].getVertices()[1].x - meshes[which].getVertices()[0].x);
        //ofCircle(me, 5);
        float lenNow = (end - start).length();
        float scale = lenNow / ww; //letterRects[i].origDistance;
        scale *= 0.9; //mouseY * 0.1;
        
        if (bStart == true){
            angleSmooth = angle;
            scaleSmooth = scale;
            bStart = false;
        } else {
            float diff = angle - angleSmooth;
            while (diff < -PI) diff += TWO_PI;
            while (diff > PI) diff -= TWO_PI;
            angleSmooth += 0.8 * diff;
            if (letterRects[i].s != " "){
                scaleSmooth = 0.1 * scale + 0.9 * scaleSmooth;
            }
        }
        
        ofPoint me = mid + length * offset * scale;
        
        ofSetColor(255,255,255,255*pct);
        
        if (scale < 4.0){
            ofPushMatrix();
            ofTranslate(me);
            ofScale(scale, scale);
            ofRotateZ(angleSmooth * RAD_TO_DEG);
            //ofRect(0,0,10,10);
            meshes[which].draw();
            ofPopMatrix();
        }
        
    }
    
    font.getFontTexture().unbind();
    
    
    //ofTranslate(0,5);
    
    //ofEnableBlendMode(OF_BLENDMODE_ADD);
    //    for (int i = 1; i < pp.size()-1; i++){
    //
    //        ofPoint a = pp[i-1];
    //        ofPoint b = pp[i+1];
    //        ofPoint diff =( b- a).rotate(90, ofPoint(0,0,1));
    //        diff = diff.getNormalized();
    //
    //        float angle = atan2(diff.y, diff.x);
    //        ofColor c;
    //        c.setHsb(ofMap(angle, -PI, PI, 0, 255), 255, 255);
    //        ofSetColor(c);
    //        ofLine(pp[i] - diff*mouseY, pp[i] + diff * mouseY);
    //        
    //        //pp.draw();
    //    }
    
}

void endlessLineScene::reset(){
    line.clear();
    pt.set(0,0,0);
    letterRects.clear();
    
}