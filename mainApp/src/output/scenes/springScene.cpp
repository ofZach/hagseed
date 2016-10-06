

#include "springScene.h"

//---------------------------------------------------------------
void springScene::setup(){
    
    font.load("fonts/Charter Regular.ttf", 20); //1.0, 1024, true, 8, 1.0);
    
    
    int count = 0;
    for (int i = 0; i < 100; i++){
        for (int j = 0; j < 15; j++){
            particle temp;
            temp.setInitialCondition(600 + ofRandom(-30,30), 600 + ofRandom(-30,30), 0, 0);
            temp.type = j;
            particles.push_back(temp);
            count++;
        }
    }

    count = 0;
    for (int i = 0; i < 100; i++){
        for (int j = 0; j < 15; j++){
            
            if (j != 0){
                
                spring tempSpring;
                springs.push_back(tempSpring);
                
                springs.back().particleA = &(particles[count-1]);
                springs.back().particleB = &(particles[count]);
                springs.back().distance = 2;
                springs.back().springiness = 0.4;
                
            }
            count++;
        }
    }
    
}


void springScene::trackContour(ofPolyline & line){
    
    
    ofPolyline resampled = line.getResampledByCount( 180);
    
    while(resampled.size() < 180){
        resampled.getVertices().push_back(resampled[resampled.size()-1]);
    }
    
    //resampled.draw();
    
    
    
    if (prevFrame.size() > 0){
        
        
        int smallestStart = -1;
        float smallestAvgLen = 10000000;
        
        for (int i = 0; i < 180; i++){
            
            float avgLen = 0;
            for (int j = 0; j < 180; j++){
                avgLen += (resampled[ (j + i ) % 180] - prevFrame[j]).length() / 180.0;
            }
            if (avgLen < smallestAvgLen){
                smallestAvgLen = avgLen;
                smallestStart = i;
            }
            
        }
        
        ofPolyline temp;
        for (int i = 0; i < 180; i++){
            temp.addVertex( resampled[ (i + smallestStart) % 180]);
        }
        resampled = temp;
        
        //cout << smallestStart << endl;
        
        
    }
    
    ofPolyline tempT = resampled.getResampledByCount(100);
    
    while(tempT.size() < 100){
        tempT.getVertices().push_back(tempT[tempT.size()-1]);
    }
    // cout << tempT.size() << " " << resampleSmoothed.size() << endl;
    
    if (resampleSmoothed.size() == 0){
        resampleSmoothed = tempT;
    } else {
        for (int i = 0; i < 100; i++){
            resampleSmoothed[i] = 0.75f * resampleSmoothed[i] + 0.25f * tempT[i];
        }
    }
    
    for (auto p : resampleSmoothed.getVertices()){
        
        // ofCircle(p, 5);
        
        unsigned int nearestIndex = 0;
        resampled.getClosestPoint(p, &nearestIndex);
        // ofLine(p, p + 50 * resampled.getTangentAtIndex(nearestIndex).rotate(90, ofPoint(0,0,1)));
        
        
        
    }
    
    
    prevFrame = resampled;
    
    
    
}





//---------------------------------------------------------------
void springScene::update(){
    
    if (IM->finder.size() > 0){
        ofPolyline line = IM->finder.getPolyline(0);
        ofPoint center = line.getCentroid2D();
        line = line.getSmoothed(3);
        int count =  line.getVertices().size();
        cv::Point * linePts[1];
        linePts[0] = new cv::Point[count];
        ofPolyline newLine;
        for (int i = 0; i < count; i++){
            linePts[0][i] = cv::Point(line[i].x*3,line[i].y*3);
            newLine.addVertex(ofPoint(line[i].x*3,line[i].y*3));
        }
        trackContour(newLine);
    }

    ofPoint centroid = resampleSmoothed.getCentroid2D();


    for (int i = 0; i < particles.size(); i++){
        particles[i].resetForce();
    }

    float time = ofGetElapsedTimef();
    for (int i = 0; i < particles.size(); i++){
        
        particle p;
        p.pos = centroid;
        particles[i].addRepulsionForce(centroid.x, centroid.y, 300,  3);
        
        for (int j = 0; j < i; j++){
            if (particles[j].type != particles[i].type){
                particles[i].addRepulsionForce(particles[j], ofClamp(ofGetMouseX(), 5, 200), 2.4);
            } else {
                particles[i].addRepulsionForce(particles[j], ofClamp(ofGetMouseY(), 5, 200), 1.4);
                
            }
        }
        
        particles[i].addForce(0, sin(time));
    }

    for (int i = 0; i < springs.size(); i++){
        springs[i].update();
    }


    if (resampleSmoothed.size() == 100){
        for (int i = 0; i < particles.size(); i++){
            if (i % 15 == 0){
                particles[i].bFixed = true;
                particles[i].pos = resampleSmoothed[ i / 15];
            }
            
            if (i % 15 == 14){
                particles[i].bFixed = true;
                particles[i].pos = resampleSmoothed[ (i / 15 + 2) % resampleSmoothed.size()];
            }
            
            
        }
    }


    for (int i = 0; i < particles.size(); i++){
        //     particles[i].bounceOffWalls();
        particles[i].addDampingForce();
        particles[i].update();
    }

}


void springScene::computeTextline(string s, textLine & res){
    
    
    res.s = s;
    
    //font.setKerning(true);
    
    res.size = ofRandom(0.8, 2.0);
    
    //string s = myString;
    
    float x = 0;
    float y = 0;
    
    
    for (auto ss : s){
        
        string sss = "";
        sss += ss;
        
        ofRectangle r = font.getStringBoundingBox(sss, x, y);
        r.width *= res.size;
        r.height *= res.size;
        
        if (ss == ' '){
            r.width = res.size*0.5;
        }
        res.rects.push_back(r);
        float dx = x - r.x;
        ofNoFill();
        //ofRect(r);
        ofFill();
        x += r.width;
    }
}


void springScene::drawStringOnLine(textLine & myString, ofPolyline myLine){
    
    
    ofPolyline tempLine = myLine;
    
    float xx = tempLine.getPerimeter();
    
    for (int i= 0; i < myString.rects.size(); i++){
        
        if (myString.rects[i].x < xx){
            char ss = myString.s[i];
            string sss = "";
            sss += ss;
            ofPoint a = tempLine.getPointAtLength(myString.rects[i].x);
            ofPoint b = tempLine.getPointAtLength(myString.rects[i].x + myString.rects[i].width);
            ofPoint diff = b-a;
            
            //cout << a << endl;
            
            //ofCircle(a, 10);
            float angle = atan2(diff.y, diff.x);
            
            
            ofMatrix4x4 mat;
            mat.translate(a.x, a.y, 0);
            
            ofMatrix4x4 mat2;
            mat2.rotateRad(-angle, 0, 0, 1);
            
            ofMatrix4x4 scale;
            scale.scale(myString.size, myString.size, 1.0);
            //ofPushMatrix();
            //ofTranslate(a.x, a.y, 0);
            //ofRotate(angle * RAD_TO_DEG, 0,0,1);
            
            //font.setSize(myString.size);
            ofMesh mesh = font.getStringMesh(sss, 0,0);
            
            
            
//            
//            m.addVertex(ofVec3f(xmin,ymin));
//            m.addVertex(ofVec3f(xmax,ymin));
//            m.addVertex(ofVec3f(xmax,ymax));
//            m.addVertex(ofVec3f(xmin,ymax));
//            
//            m.addTexCoord(ofVec2f(t1,v1));
//            m.addTexCoord(ofVec2f(t2,v1));
//            m.addTexCoord(ofVec2f(t2,v2));
//            m.addTexCoord(ofVec2f(t1,v2));
//            
//            m.addIndex(firstIndex);
//            m.addIndex(firstIndex+1);
//            m.addIndex(firstIndex+2);
//            m.addIndex(firstIndex+2);
//            m.addIndex(firstIndex+3);
//            m.addIndex(firstIndex);

            //cout << mesh.getVertices().size() << endl;
            
            int firstIndex = m.getVertices().size();
            for (auto & a : mesh.getVertices()){
                a =  mat2 * (scale*a) * mat;
                //m.addVertex(a);
            }
            for (int j = 0; j < 4; j++){
                m.addVertex(mesh.getVertices()[j]);
                m.addTexCoord(mesh.getTexCoords()[j]);
            }
            
            m.addIndex(firstIndex);
            m.addIndex(firstIndex+1);
            m.addIndex(firstIndex+2);
            m.addIndex(firstIndex+2);
            m.addIndex(firstIndex+3);
            m.addIndex(firstIndex);
            
            
            
            //mesh.draw();
            //ofCircle(0,0,10);
            //ofRect(0,0,20,5);
            //ofPopMatrix();
        }
        
        //ofLine(a,b);
    }
}



//---------------------------------------------------------------
void springScene::draw(){
    
    string s = "Prospero is not crazy. Ariel exists. People other than Prospero see him and hear him. The enchantments are real. Hold onto that. Trust the play. Prospero is not crazy. Ariel exists. People other than Prospero see him and hear him. The enchantments are real. Hold onto that. Trust the play. Prospero is not crazy. Ariel exists. People other than Prospero see him and hear him. The enchantments are real. Hold onto that. Trust the play. Prospero is not crazy. Ariel exists. People other than Prospero see him and hear him. The enchantments are real. Hold onto that. Trust the play. Prospero is not crazy. Ariel exists. People other than Prospero see him and hear him. The enchantments are real. Hold onto that. Trust the play. ";
    
    vector < string > sss = ofSplitString(s, " ");
    
    m.clear();
    m.setMode(OF_PRIMITIVE_TRIANGLES);
    
    ofPolyline temp;
    
    if (textlines.size() == 0){
        
        for (int i = 0;  i < 100; i++){
            
            ofSeedRandom(i * 10000);
            string newS = "";
            ofRandomize(sss);
            for (int j = 0; j < 40; j++){
                newS += sss[j];
                newS += " ";
            }
            
            textLine temp;
            computeTextline(newS, temp);
            textlines.push_back(temp);
        }
    }
    
    ofPushMatrix();
    ofTranslate(230,-10);
    int counter = 0;
    //font.getFontTexture().bind();
    
    for (int i = 0; i < particles.size(); i++){
        
        temp.addVertex(particles[i].pos);
        if (i % 15 == 14 && i != 0){
            
            
            temp = temp.getResampledByCount(70);
            temp  = temp.getSmoothed(11);
            
            drawStringOnLine(textlines[counter], temp);
            temp.clear();
            counter++;
        }
    }
    
    
    bool blendEnabled = glIsEnabled(GL_BLEND);
    GLint blend_src, blend_dst;
    glGetIntegerv( GL_BLEND_SRC, &blend_src );
    glGetIntegerv( GL_BLEND_DST, &blend_dst );
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    font.getFontTexture().bind();
    m.draw();
    font.getFontTexture().unbind();
    
    if(!blendEnabled){
        glDisable(GL_BLEND);
    }
    glBlendFunc(blend_src, blend_dst);

    
    
    
    ofPopMatrix();
    
    
}