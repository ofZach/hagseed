

#include "displacementScene.h"

//---------------------------------------------------------------
void displacementScene::setup(){
    
    
    offsetAmount = 0;
    offsetTarget = 90;
    
    
    spot.load("displace/spot.png");
    
    std::ifstream t(ofToDataPath("displace/lorem.txt").c_str());
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    
    vector < string > strSplit = ofSplitString(str, "\n");
    
    //std::reverse(, <#_BidirectionalIterator __last#>)
    
    for (int i=0; i< strSplit.size(); i++) {
        ofxParagraph* p = new ofxParagraph(strSplit[i]);
        p->setColor(ofColor::fromHex(0x555555));
        p->drawBorder(ofColor::fromHex(0x777777));
        p->setAlignment(ofxParagraph::ALIGN_LEFT);
        paragraphs.push_back(p);
    }
    
    // change these to whatever you want //
    int pWidth = 1000;
    int pFontSize = 14;
    float pPadding = pWidth*.0;
    
    // check for high resolution display //
    if (true)
    {
        pWidth*=2;
        pFontSize*=2;
    }
    
    // load our fonts and layout our paragraphs //
    //label = ofxSmartFont::add("raleway/Raleway-Thin.ttf", pFontSize*2, "raleway-thin");
    
    ofxSmartFont::add("displace/raleway/Raleway-Medium.ttf", pFontSize, "raleway-medium");
    ofxSmartFont::get("raleway-medium")->ttf.setLineHeight(14);
    for (int i=0; i<paragraphs.size(); i++){
        paragraphs[i]->setFont(ofxSmartFont::get("raleway-medium"));
        
    }
    //  paragraphs[1]->setFont(ofxSmartFont::add("raleway/Raleway-Italic.ttf", pFontSize, "raleway-italic"));
    //  paragraphs[2]->setFont(ofxSmartFont::add("raleway/Raleway-SemiBoldItalic.ttf", pFontSize, "raleway-semi-bi"));
    //  ofxSmartFont::list();
    
    int pLeading = pFontSize*.15;
    int tWidth = paragraphs.size()*pWidth + (paragraphs.size()-1)*pPadding;
    
    int x = (ofGetWidth() - tWidth)/2;
    int height = 0;
    for (int i=0; i<paragraphs.size(); i++){
        paragraphs[i]->setWidth(pWidth);
        paragraphs[i]->setLeading(pLeading*0.0);
        
        paragraphs[i]->setSpacing(pFontSize*.6);
        paragraphs[i]->setBorderPadding(0);
        paragraphs[i]->setPosition(100, height);
        height += paragraphs[i]->getHeight();
    }
    
    
    
    for (int i = 0; i < 255; i++){
        string ssTemp = "";
        ssTemp += (char)i;
        ofRectangle r = ofxSmartFont::get("raleway-medium")->ttf.getStringBoundingBox(ssTemp, 0, 0);
        ofPoint pt = r.getPosition() + ofPoint(0, r.getHeight());
        
        //cout << "offset " <<  ssTemp << " " << pt << endl;
        offsets.push_back(pt);
    }
    
    
    
    
    //  draw the paragraphs //
    
    ofxSmartFont::get("raleway-medium")->ttf.setLineHeight(5);
    
    height = 100;
    for (int i=0; i<paragraphs.size(); i++){
        paragraphs[i]->setWidth(1100);
        paragraphs[i]->setPosition(100, 0);
        paragraphs[i]->setLeading(pLeading*0.0);
        height += paragraphs[i]->getHeight();
    }
    
    //  make some fucking lines.
    float xStart = 100;
    float xEnd = 100 + 1100;
    
    
    for (int z=0; z<paragraphs.size(); z++){
        
        
        line tempLine;
        lines.push_back(tempLine);
        
        for (int i=0; i< paragraphs[z]->mLines.size(); i++) {
            line tempLine;
            for (int j=0; j< paragraphs[z]->mLines[i].size(); j++) {
                
                //                ofPushStyle();
                //                ofNoFill();
                //                ofSetHexColor(0xff0099);
                //
                
                float x = paragraphs[z]->x + paragraphs[z]->mLines[i][j]->rect.x;
                float y = paragraphs[z]->y + paragraphs[z]->mLines[i][j]->rect.y;
                
                ofMesh m = ofxSmartFont::get("raleway-medium")->ttf.getStringMesh(paragraphs[z]->mLines[i][j]->text, x, y);
                
                string ss = paragraphs[z]->mLines[i][j]->text;
                
                //m.drawWireframe();
                
                for (int l = 0; l < ss.size(); l++){
                    string ssTemp = "";
                    ssTemp += ss[l];
                    
                    ofPoint pt2 = m.getVertices()[l * 4 + 2] ; //- offsets[ (int)ss[l] ];
                    ofPoint pt = m.getVertices()[l * 4 + 3] ; //- offsets[ (int)ss[l] ];
                    float pct = ofMap(pt.x, xStart, xEnd, 0, 1);
                    tempLine.pcts.push_back(pct);
                    tempLine.chars.push_back(ss[l]);
                    tempLine.pctWidths.push_back(ofMap(fabs(pt2.x-pt.x), xStart, xEnd, 0, 1));
                    
                    
                    //cout << "pct " << pct << endl;
                    
                    //ofxSmartFont::get("raleway-medium")->ttf.drawString(ssTemp, pt.x, pt.y);
                }
                
                
                
                //ofPopStyle();
                
            }
            lines.push_back(tempLine);
            
        }
        
        
    }
    
    
    img.allocate(512, 424, OF_IMAGE_GRAYSCALE);

}


//---------------------------------------------------------------
void displacementScene::update(){
    
    
    
    offsetAmount = 0.98 * offsetAmount + 0.02 * offsetTarget;
    
    
    cv::Mat m = ofxCv::toCv(img);
    m = cvScalar(0,0,0);
    
    if (IM->finder.size() > 0){
        
        ofPolyline line = IM->finder.getPolyline(0);
        
        int count =  line.getVertices().size();
        cv::Point * linePts[1];
        linePts[0] = new cv::Point[count];
        
        for (int i = 0; i < count; i++){
            linePts[0][i] = cv::Point(line[i].x,line[i].y);
        }
        
        const cv::Point* ppt[1] = { linePts[0] };
        int npt[] = { count };
        
       
        fillPoly( m,
                 ppt,
                 npt,
                 1,
                 cvScalar( 255,255,255),
                 0 );
        
        
    }
    
    
    ofxCv::erode(img);
    ofxCv::erode(img);
    ofxCv::erode(img);
    ofxCv::erode(img);
    ofxCv::erode(img);
//    ofxCv::dilate(img);
//    ofxCv::dilate(img);
//    ofxCv::dilate(img);
    
    for (int i = 0; i < 14; i++){
        ofxCv::blur(img, 3);
    }
    
    img.update();
    
}



typedef struct {
    
    ofPoint pt;
    ofMatrix4x4 rot;
    float scale;
    char letter;
    ofPoint offset;
    int originalIndex;
} letterInSpace;

vector < letterInSpace > letterInSpaces;

bool bSort(letterInSpace & a, letterInSpace & b){
    return a.pt.z < b.pt.z;
}


float getBrightness(ofPoint input, ofRectangle inputRect, ofRectangle outputRect, ofImage & pixels){
    
    ofRectangle imgRect(0,0,pixels.getWidth()-1, pixels.getHeight()-1);
    
    // do the remap;
    
    float newX = ofMap(input.x, inputRect.x, inputRect.x + inputRect.width,
                       outputRect.x, outputRect.x + outputRect.width);
    
    float newY = ofMap(input.y, inputRect.y, inputRect.y + inputRect.height,
                       outputRect.y, outputRect.y + outputRect.height);
    
    if (imgRect.inside(newX, newY)){
        
        ofColor c = pixels.getColor(newX, newY);
        return c.r / 255.0;
        
    } else {
        return 0;
    }
}



//---------------------------------------------------------------
void displacementScene::draw(){
    
    
    //img.draw(0,0);
    //return;
    
    
    //cout << img.getImageType() << " " << OF_IMAGE_COLOR;
    
    vector < ofPolyline > linesTo;
    
    cout << ofGetMouseX() << " " << ofGetMouseY() << endl;
    for (int i = 0; i < lines.size(); i++){
        ofPolyline myLine;
        myLine.addVertex(ofPoint(100 -600 + 510, 30 + i * 50 - 1000 + 30));
        myLine.addVertex(ofPoint(1100 - 600 + 510, 30 + i * 50 -1000 + 30));
        myLine = myLine.getResampledByCount(100);
        linesTo.push_back(myLine);
    }
    
    
    float ff = ofGetElapsedTimef() * 0.4;
    
    
    //float pct = sin(ff) * 0.5 + 0.5;
    
    
    ofRectangle r;
    for (int i = 0; i < linesTo.size(); i++){
        for (int j = 0; j < linesTo[i].size(); j++){
            if (i == 0 && j == 0){
                r.setPosition(linesTo[i][j]);
            } else {
                r.growToInclude(linesTo[i][j]);
            }
        }
    }
    ofNoFill();
    //ofDrawRectangle(r) ;//, <#float y1#>, <#float w#>, <#float h#>)
    ofRectangle r2 = r;
    r2.scaleTo( ofRectangle(0,0,img.getWidth(), img.getHeight()));
    r2.scaleFromCenter(0.8);
    //ofDrawRectangle(r2);
    
    // cam.begin();
    
    
    for (int i = 0; i < linesTo.size(); i++){
        for (int j = 0; j < linesTo[i].size(); j++){
            
            
            
            ofPoint pt =  linesTo[i][j];
            float bri =  getBrightness(pt,r, r2, img);
            
            
            float amt = 100;
            float ne =  getBrightness(linesTo[i][j] + ofPoint(-amt, -amt), r, r2, img);
            float n_ =   getBrightness(linesTo[i][j] + ofPoint( 0.0, -amt), r, r2, img);
            float nw =   getBrightness(linesTo[i][j] + ofPoint( amt, -amt), r, r2, img);
            float w_ =   getBrightness(linesTo[i][j] + ofPoint( amt,  0.0), r, r2, img);
            float sw =   getBrightness(linesTo[i][j] + ofPoint( amt,  amt), r, r2, img);
            float s_ =   getBrightness(linesTo[i][j] + ofPoint( 0.0,  amt), r, r2, img);
            float se =   getBrightness(linesTo[i][j] + ofPoint(-amt,  amt), r, r2, img);
            float e_ =   getBrightness(linesTo[i][j] + ofPoint(-amt,  0.0), r, r2, img);
            
            float up = (ne + n_ + nw) - (se + s_ + sw);
            float right = (sw + w_ + nw) - (ne + e_ + se);
            
            
            //ofPoint diff = linesTo[i][j] - ofPoint(mouseX, mouseY, -30);
            //float dist = ofDist(linesTo[i][j].x, linesTo[i][j].y, mouseX*3, mouseY*3);
            //float strength = ofMap(dist, 0, 1000, 1.0, 0, true);
            
            // cout << mouseY << endl;
            linesTo[i][j] += (ofPoint(0, -bri,0) * offsetAmount); //+ ofPoint(-right,up, 0) * mouseX;
            //diff.getNormalized() * strength * 400.0;   //ofPoint(offsetx, offsety, offsetz) * ((sin(ff*0.4)*0.5 + 0.5) * 5000) * pct;
        }
        
        linesTo[i] = linesTo[i].getSmoothed(3);
        //linesTo[i].draw();
    }
    
    
    for (int z = 0; z < 3; z++){
        for (int i = 1; i < linesTo.size()-1; i++){
            for (int j = 0; j < linesTo[i].size(); j++){
                
                
                linesTo[i][j] = 0.9 * linesTo[i][j] + 0.05 * linesTo[i-1][j] + 0.05 * linesTo[i+1][j];
            }
            
            //linesTo[i] = linesTo[i].getSmoothed(3);
            //linesTo[i].draw();
        }
        
        for (int i = 0; i < linesTo.size(); i++){
            for (int j = 1; j < linesTo[i].size()-1; j++){
                
                
                linesTo[i][j] = 0.9 * linesTo[i][j] + 0.05 * linesTo[i][j+1] + 0.05 * linesTo[i][j-1];
            }
            
            //linesTo[i] = linesTo[i].getSmoothed(3);
            //linesTo[i].draw();
        }
        
    }
    
    
    
    // ofEnableDepthTest();
    
    letterInSpaces.clear();
    
    for (int i = 0; i < lines.size(); i++){
        
        float scale = linesTo[i].getPerimeter() / 1200.0;
        
        for (int j = 0; j < lines[i].pcts.size(); j++){
            
            ofPoint pt = linesTo[i].getPointAtPercent(lines[i].pcts[j]);
            ofPoint pt2 = linesTo[i].getPointAtPercent(lines[i].pcts[j]+lines[i].pctWidths[j]*0.2);
            //ofPoint pt( ofMap(lines[i].pcts[j], 0, 1, 100, 1300), 30 + i * 50);
            //cout << pt << " " << pt2 << endl;
            char c = lines[i].chars[j];
            string ssTemp = "";
            ssTemp += c;
            ofPoint offset = offsets[(int)c];
            ofPoint diff =pt2 - pt;
            //cout << diff.length() << endl;
            
            float angle = atan2(diff.y, diff.x) + PI;
            
            //ofPushMatrix();
            
            //ofQuaternion q;
            
            ofPoint diffA = ofPoint(1,0);
            ofPoint diffB = (pt - pt2).getNormalized();
            
            float dotProd = diffB.dot(ofPoint(0,0,1));
            
            // if (fabs(dotProd) > 0.9) continue;
            //cout << dotProd << endl;
            
            //q.makeRotate(diffA, diffB);
            
            ofMatrix4x4 m;
            m.makeRotationMatrix(diffA, diffB);
            
            letterInSpace lis;
            lis.letter = c;
            lis.scale = scale;
            lis.pt = pt;
            lis.rot = m;
            
            lis.originalIndex = letterInSpaces.size();
            //ofTranslate(pt.x, pt.y, pt.z);
            
            
            //ofMultMatrix(m);
            
            //ofRotateZ(angle * RAD_TO_DEG);
            //ofScale(scale, scale, scale);
            offset = -offset * ofPoint(1,0) * scale;
            
            lis.offset = offset;
            letterInSpaces.push_back(lis);
            //ofxSmartFont::get("raleway-medium")->ttf.drawString(ssTemp, offset.x, offset.y);
            //ofPopMatrix();
        }
        
    }
    
    
    ofSort(letterInSpaces, bSort);
    
    
    //cout << mouseX << " " << mouseY << endl;
    ofTranslate(455, 679);
    ofScale(0.7, 0.7);
    
    
    for (int i = 0; i < letterInSpaces.size(); i++){
        
        ofPoint pt = letterInSpaces[i].pt;
        
        ofPushMatrix();
        ofTranslate(pt.x, pt.y, pt.z);
        
        if (letterInSpaces[i].originalIndex % 5 == 0){
            ofPushMatrix();
            ofTranslate(0,0, 5);
            ofSetColor(0,0,0, 20);
            //ofScale(2,2,1);
            spot.draw(-50 + 20, -30+-50,200,200);
            
            ofPopMatrix();
        }
        ofSetColor(255);
        ofMultMatrix(letterInSpaces[i].rot);
        ofScale(letterInSpaces[i].scale, letterInSpaces[i].scale, letterInSpaces[i].scale);
        
        string ssTemp = "";
        ssTemp += letterInSpaces[i].letter;
        ofPoint offset = letterInSpaces[i].offset*letterInSpaces[i].scale;
        
        ofxSmartFont::get("raleway-medium")->ttf.drawString(ssTemp, offset.x, offset.y);
        ofPopMatrix();
    }
    
    
    ofSetColor(0,0,0);
    
    //    for (int i = 0; i < linesTo.size()-1; i++){
    //        ofMesh m;
    //        m.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    //        for (int j = 0; j < linesTo[i].size()-1; j++){
    //            m.addVertex(linesTo[i][j] - ofPoint(0,0,mouseY*0.1));
    //            m.addVertex(linesTo[i+1][j] - ofPoint(0,0, mouseY*0.1));
    //
    //        }
    //        ofSetColor(0);
    //        m.draw();
    //        ofSetColor(255);
    //        //m.drawWireframe();
    //
    //    }
    
    ofSetColor(255);
    
    

}