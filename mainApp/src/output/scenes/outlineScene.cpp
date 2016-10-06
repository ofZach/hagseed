

#include "outlineScene.h"
#include "ofxCv.h"


using namespace ofxCv;
using namespace cv;

#define OUT_W (1024*0.8)
#define OUT_H (1024*1.1)



bool bSortLevel ( polyLevel & a, polyLevel & b){
    return a.level < b.level;
}


ofPoint getMixedPt(int index, float pct, ofPolyline & myLine){
    ofPoint a = myLine[index];
    ofPoint b = myLine[index+1];
    return a + (b-a)*pct;
}

//---------------------------------------------------------------
void outlineScene::setup(){
    
    person.allocate(1024,1024, OF_IMAGE_GRAYSCALE);
    
    
    sizeAmount = 0;
    sizeTarget = 1.0;
    
    
    
    centerSmooth.set(300,300);
    
    //------
    
    font.load("fonts/Charter Regular.ttf", 15);
    
    
    ss = "He required a focus, a purpose. He gave this much thought while sitting in his deck chair. Eventually he concluded that there were two things thing left for him two projects that could still hold satisfaction. After a time he began to see more clearly what they were. First, he needed to get his Tempest back. He had to stage it, somehow, somewhere. His reasons were beyond theatri- cal; they had nothing to do with his reputation, his career Ð none of that. Quite simply, his Miranda must be released from her glass co n; she must be given a life. But how to do it, where to  nd the actors? Actors did not grow on trees, numerous though the trees were around his hovel. Second, he wanted revenge. He longed for it. He day- dreamed about it. Tony and Sal must su er. His present woeful situation was their doing, or a lot of it was. TheyÕd treated him shabbily. But what form could such revenge possibly take? Those were the two things he wanted. He wanted them more each day. But he didnÕt know how to go about getting them.";
    
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


//---------------------------------------------------------------
void outlineScene::update(){
    
    
    if (IM->finder.size() > 0){
        
        int count =  IM->finder.getPolyline(0).size();
        
        if (count < 20) return;
        
        cv::Point * linePts[1];
        linePts[0] = new cv::Point[count];
        
        ofPolyline tempLine = IM->finder.getPolyline(0);
        ofPoint center = tempLine.getCentroid2D();
        centerSmooth = 0.95 * centerSmooth + 0.05 * center;
        for (auto & a : tempLine){
            a -= centerSmooth;
        }
        
        
        for (int i = 0; i < count; i++){
            linePts[0][i] = cv::Point(tempLine[i].x*0.5+ 512,tempLine[i].y*0.5+ 512);
        }
        
        //ofPoint midPt = tempLine.getCentroid2D() + ofPoint(300,100);
        
        const cv::Point* ppt[1] = { linePts[0] };
        int npt[] = { count };
        
        cv::Mat m = ofxCv::toCv(person);
        
        m = cvScalar(255,255,255);
        fillPoly( m,
                 ppt,
                 npt,
                 1,
                 cvScalar( 0,0,0),
                 0 );
        
        delete [] linePts[0];
        
        cv::Mat dist;
        cv::distanceTransform(m, dist, CV_DIST_L2, 5);
        //  cv::normalize(dist, dist, 0, 1.0, cv::NORM_MINMAX);
        
        float* matData = (float*)dist.data;
        float ff = ofGetElapsedTimef();
        unsigned char * pixels = person.getPixels().getData();
        
        
        float f = 30 * 0.01;
        
        for (int i = 0; i < 1024*1024; i++){
            
            if ( sin(matData[i] * 100 * 0.0033) > 0 && matData[i] < 240){
                pixels[i] = 255;
            } else {
                pixels[i] = 0;
            }
        }
        
        
        person.update();
        
        
        //cv::Mat m = ofxCv::toCv(person);
        
        vector<vector<cv::Point> > contours;
        vector<Vec4i> hierarchy;
        findContours(m, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
        
        //  cout << contours.size() << endl;
        
        
        levels.clear();
        ofEnableAlphaBlending();
        if (contours.size() < 100){
            
            //        if (z == 0) ofSetColor(255,255,0);
            //        if (z == 1) ofSetColor(0,255,255);
            //        if (z == 2) ofSetColor(255,0,255);
            //
            
            //    tempLine[z].draw();
            
            
            
            
            for (size_t i = 0; i < contours.size(); i++){
                
                int levelDeep = 0;
                int parent = hierarchy[i][3];
                if (parent < 0){
                    levelDeep = 0;
                } else {
                    while (parent >= 0){
                        parent = hierarchy[parent][3];
                        levelDeep++;
                    }
                }
                ofPolyline line;
                
                
                for (int j = 0; j < contours[i].size(); j++){
                    //ofCircle (contours[i][j].x, contours[i][j].y, 2);
                    line.addVertex(contours[i][j].x*2, contours[i][j].y*2);
                }
                line.setClosed(true);
                
                //            if (levelDeep == 0){
                //                line = line.getSmoothed(11);
                //            }
                //line = line.getSmoothed(11);
                //line = line.getResampledBySpacing(3);
                //line = line.getSmoothed(11);
                line.flagHasChanged();
                
                // ofSetColor(255);
                //line.draw();
                
                
                polyLevel p;
                p.level = levelDeep;
                p.line = line;
                //p.distToCenter = (line[0]-midPt).length();
                levels.push_back(p);
                
                
            }
            
            float pctAmount = 30;
            ofSort(levels, bSortLevel);
            //levels[levels.size()-1].line = levels[levels.size()-1].line .getSmoothed(13);
        }
        
    }
    
}

//
//void outlineScene::drawStringOnLine(string myString, ofPolyline myLine){
//    
//    
//    
//    //font.setKerning(true);
//    
//    string s = myString;
//    
//    float x = 0;
//    float y = 0;
//    
//    vector < ofRectangle > rects;
//    
//    for (auto ss : s){
//        
//        string sss = "";
//        sss += ss;
//
//        ofRectangle r = font.getStringBoundingBox(sss, x, y);
//        
//        if (ss == ' '){
//            r.width = font.getStringBoundingBox("X", 0, 0).width*1.4;
//        }
//        rects.push_back(r);
//        float dx = x - r.x;
//        ofNoFill();
//        //ofRect(r);
//        ofFill();
//        x += r.width;
//    }
//    
//    //    ofNoFill();
//    //    for (int i = 0; i < rects.size(); i++){
//    //        ofRectangle r = rects[i];
//    //        r.position += ofPoint(50,50);
//    //        ofRect(r);
//    //    }
//    //
//    //    ofLine(0,50,ofGetWidth(), 50);
//    
//    ofPolyline tempLine = myLine;
//    
//    float xx = tempLine.getPerimeter();
//    
//    //cout << rects.size() << endl;
//    
//    //ofTranslate(40,0);
//    for (int i= 0; i < rects.size(); i++){
//        
//        if (rects[i].x < xx*0.5){
//            char ss = s[i];
//            string sss = "";
//            sss += ss;
//            ofPoint a = tempLine.getPointAtLength(rects[i].x);
//            ofPoint b = tempLine.getPointAtLength(rects[i].x + rects[i].width);
//            ofPoint diff = b-a;
//            
//            //cout << a << endl;
//            
//            //ofCircle(a, 10);
//            float angle = atan2(diff.y, diff.x);
//            
//            ofMatrix4x4 mat;
//            mat.translate(a.x, a.y, 0);
//            
//            ofMatrix4x4 mat2;
//            mat2.rotateRad(-angle, 0, 0, 1);
//            
// 
//            //font.setSize(myString.size);
//            ofMesh mesh = font.getStringMesh(sss, 0,0);
//            
//
//            
//            int firstIndex = m.getVertices().size();
//            for (auto & a : mesh.getVertices()){
//                a =  mat2 * (a) * mat;
//                //m.addVertex(a);
//            }
//            for (int j = 0; j < 4; j++){
//                m.addVertex(mesh.getVertices()[j]);
//                m.addTexCoord(mesh.getTexCoords()[j]);
//            }
//            
//            m.addIndex(firstIndex);
//            m.addIndex(firstIndex+1);
//            m.addIndex(firstIndex+2);
//            m.addIndex(firstIndex+2);
//            m.addIndex(firstIndex+3);
//            m.addIndex(firstIndex);
//            
//
//        }
//        
//        //ofLine(a,b);
//    }
//}
//


//---------------------------------------------------------------
void outlineScene::draw(){
    
    
   // cout << ofGetMouseX() << " ? " << ofGetMouseY() << endl;
    
   ofScale(1.6, 1.6);
    ofTranslate(52, -174);
    ofBackground(0);
    sizeAmount = 0.9f * sizeAmount + 0.1 * sizeTarget;
    
    
    ofBackground(0);
    
    ofTranslate(-512,-512);
    
    //
    //    //ofScale(0.25, 0.25);
    //    grayscale2.draw(0,0);
    //
    //    //ofTranslate(400,0);
    //    if (finder.size() > 0){
    //        ofPolyline line = finder.getPolyline(0);
    //        line = line.getResampledByCount(100);
    //
    //        line.draw();
    //    }
    //
    //    finder.draw();
    
    
    // cam.begin();
    
    float stripeAmount = ofMap(sin(ofGetElapsedTimef()), -1, 1, 1, 6);
    float minm = 0;
    float maxm = 1;
    if (levels.size() > 0) maxm = levels[levels.size()-1].level;
    
    ofColor cc;
    float timeffff = ofGetElapsedTimef();
    for (int i = 0; i < levels.size(); i++){
        float pct = ofMap(levels[i].level, minm, maxm, 1, 0);
        if ( levels[i].line.size () < 3) continue;
        
        //        ofSetColor(255);
        //        ofNoFill();
        //        ofBeginShape();
        //
        //        for (auto & pt : levels[i].line){
        //            ofVertex(pt.x, pt.y, 0);
        //        }
        //        ofEndShape();
        //
        //        ofDrawBitmapString(ofToString(levels[i].level),levels[i].line[0]);
        // m.draw();
    }
    
    
    ofSeedRandom(0);
    
    for (int zzz = 0; zzz < levels.size(); zzz++){
        ofPolyline line = levels[zzz].line;
        if (levels[zzz].level % 2 == 0){
            std::reverse(line.getVertices().begin(),
                         line.getVertices().end());
        }
        
        float randomPct = ofRandom(0,0.1);
        while (randomPct < 1) randomPct += 1;
        
        ofPolyline temp;
        
        float sizeMe = sizeAmount; //1.0; //sin(ofGetElapsedTimef() + ofRandom(0, TWO_PI)) * 0.5 + 0.5;
        for (int i = 0; i < line.size() * sizeMe; i++){
            temp.addVertex(line[ (int)((line.size() * randomPct) + i) % line.size()]);
        }
        line = temp;
        
        
        
        
        
        letterRects.clear();
        
        
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
        }
        
        
        //--------- draw
        
        
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
            ofPoint start = getMixedPt(letterRects[i].startIndex, letterRects[i].startPct, line);
            ofPoint mid = getMixedPt(letterRects[i].midIndex, letterRects[i].midPct, line);
            ofPoint end = getMixedPt(letterRects[i].endIndex, letterRects[i].endPct, line);
            
            int which = letterRects[i].s[0];
            if (which < 0) continue;
            
            //cout << which  << endl;
            
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
            
            ofSetColor(255,255,255,255);
            
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
    }
    

    
    //cout << ofGetFrameRate() << endl;
    
}