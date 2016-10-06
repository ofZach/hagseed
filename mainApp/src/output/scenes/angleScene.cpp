

#include "angleScene.h"
#include "Miniball.hpp"

typedef double mytype;             // coordinate type
typedef std::list<std::vector<mytype> >::const_iterator PointIterator;
typedef std::vector<mytype>::const_iterator CoordIterator;


// much fucking better
ofPoint getPointAtLength(float len, ofPolyline & line){
    
    // easy peasy
    float length = 0;
    for (int i = 0; i < line.size()-1; i++){
        float segLength = (line[i+1] - line[i]).length();
        
        if (len >= length && len < (length + segLength)){
            float pct = (len - length) /  segLength;
            return  (pct * line[i+1] + (1-pct) * line[i]);
        }
        
        length+= segLength;
    }
    
    if (len < 0) return line[0];
    if (len >= line.getPerimeter()){
        return line[line.size()-1];
    }
    
}




//---------------------------------------------------------------
void angleScene::setup(){
    
    scaleSmooth2 = 1.0;
    
    apercu.load("fonts/ApercuBold.otf", 30);//, bool _makeContours, float _simplifyAmt, int _dpi
    
    targetAngleSmooth = 0;
    targetAngle = 0;
    lockPct = 0;
    scaleSmooth = 1.0;
    
}


void angleScene::drawStringOnLine(string myString, ofPolyline myLine){
    
    
    
    //font.setKerning(true);
    
    string s = myString;
    
    float x = 0;
    float y = 0;
    
    vector < ofRectangle > rects;
    
    for (auto ss : s){
        
        string sss = "";
        sss += ss;
        
        ofRectangle r = apercu.getStringBoundingBox(sss, x, y);
        //cout << x << " " << r.x << endl;
        if (ss == ' '){
            r.width = 10;
        }
        r.x = x;
        
        rects.push_back(r);
        float dx = x - r.x;
        ofNoFill();
        //ofRect(r);
        ofFill();
        x = (r.x + r.width);
    }
    
    //    ofNoFill();
    //    for (int i = 0; i < rects.size(); i++){
    //        ofRectangle r = rects[i];
    //        r.position += ofPoint(50,50);
    //        ofRect(r);
    //    }
    //
    //    ofLine(0,50,ofGetWidth(), 50);
    
    ofPolyline tempLine = myLine;
    
    float xx = tempLine.getPerimeter()*1.0;
    
    //cout << rects.size() << endl;
    //cout << rects.size() << " " << sss.size() << endl;
    
    
    //ofTranslate(40,0);
    for (int i= 0; i < rects.size(); i++){
        
        if (rects[i].x < xx){
            char ss = s[i];
            string sss = "";
            sss += ss;
            ofPoint a = getPointAtLength(rects[i].x, tempLine);
            ofPoint b = getPointAtLength(rects[i].x + rects[i].width, tempLine); //tempLine.getPointAtLength(rects[i].x + rects[i].width);
            ofPoint diff = b-a;
            
            //cout << a << endl;
            
            //ofCircle(a, 10);
            float angle = atan2(diff.y, diff.x);
            ofPushMatrix();
            ofTranslate(a.x, a.y, 0);
            ofRotate(angle * RAD_TO_DEG, 0,0,1);
            //font.setSize(10);
            ofSetColor(0);
            ofPushMatrix();
            ofTranslate(3, 3);
            apercu.drawString(sss, 0,0);
            ofPopMatrix();
            ofSetColor(255);
            apercu.drawString(sss, 0,0);
            
            //ofCircle(0,0,10);
            //ofRect(0,0,20,5);
            ofPopMatrix();
        }
        
        //ofLine(a,b);
    }
}


//---------------------------------------------------------------
void angleScene::update(){
    
    float time = ofGetElapsedTimef();
    
    for (int i = 0; i < aline.angleDiffs.size(); i++){
        aline.angleDiffs[i] = 0.99 * aline.angleDiffs[i] + 0.01 * alineTarget.angleDiffs[i] ;
    }
    
    // aline.reverse();
    
    
    //    for (int i = 0; i < aline.angleDiffs.size(); i++){
    //        aline.angleDiffs[i] = 0.99 * aline.angleDiffs[i] + 0.01 * alineTargetReversed.angleDiffs[i];
    //    }
    //
    //    aline.reverse();
    //alineTarget.reverse();
    
    
    //    float diff = targetAngle - targetAngleSmooth;
    //    while (diff < -PI){
    //        diff += TWO_PI;
    //
    //    }
    //    while (diff > PI){
    //        diff -= TWO_PI;
    //    }
    
    ofPolyline temp = aline.convertBack();
    
    if (temp.size() > 0){
        ofPoint diff2 = temp[0] - temp[temp.size()-1];
        float angle = -atan2(diff2.y, diff2.x);
        float angleDiff = angleLock - targetAngleSmooth2;
        
        while (angleDiff > PI) angleDiff -= TWO_PI;
        while (angleDiff < -PI) angleDiff += TWO_PI;
        
        targetAngleSmooth = angle;
        targetAngleSmooth2 += 0.1*angleDiff;
    }

}


//---------------------------------------------------------------
void angleScene::draw(){
    
    
    templine.draw();
    
    lockPct = 0.98f * lockPct + 0.02 * 1.0;
    
    
    ofPolyline temp = aline.convertBack();
    
    
    if (temp.size() == 0) return;
    
    ofPoint midPt;
    for (auto a : temp){
        midPt += a;
    }
    midPt /= (float) temp.size();
    smooth = 0.5 * smooth + 0.5 * midPt;
    
    
    
    
    
    
    //cout << "Radius " << radius << endl;
    
    float scale = 2000.0 / temp.getPerimeter();
    scaleSmooth = 0.9 * scaleSmooth + 0.1 * scale;
    
    for (auto & a : temp){
        a-= smooth;
        a *= scaleSmooth;
        //a.rotate(180 + angleDiff * lockPct * RAD_TO_DEG, ofPoint(0,0,1));
    }
    
    
    //----------------- circle mf;
    const int       d = 2;            // dimension
    std::list<std::vector<mytype> > lp;
    for (int i = 0; i < temp.size(); i++){
        std::vector<mytype> c_vector {temp[i].x, temp[i].y};
        lp.push_back(c_vector);
    }
    typedef Miniball::
    Miniball <Miniball::CoordAccessor<PointIterator, CoordIterator> >
    MB;
    MB mb (d, lp.begin(), lp.end());
    ofPoint center(mb.center()[0], mb.center()[1]);
    float radius =  sqrt( mb.squared_radius());
    
    float scalar = 300 / radius;
    
    scaleSmooth2 = 0.99f * scaleSmooth2 + 0.01 * scalar;
    
    string s = "Lorem ipsum dolor sit amet, ad tantas dissentiet eam, docendi torquatos omittantur has eu, affert debitis per ex. Fugit gloriatur concludaturque ad ius. Usu errem necessitatibus in, et pro vidit rebum. Cu etiam suscipiantur sea.  Eam id omittam sadipscing. At ridens eleifend mel, no soleat ridens putant nec. Ei cibo tincidunt conceptam nam. Ea per legimus moderatius. Ius saperet corrumpit expetendis te, purto deserunt nec ea, ignota officiis mnesarchum ne mea. Te est homero fastidii splendide. Cum eu omnis novum timeam, rebum graecis eu eos. Porro iudico quidam in pro, ut integre dolorum laoreet qui. Vis mundi ubique petentium ut. Cum eu sint denique, eu elitr euismod invenire vel. Eam et audiam offendit, albucius convenire ex est, ei nam feugait deleniti. Duo ut persius feugait lucilius. No odio alia gubergren pri. Et modo interesset ius, antiopam mnesarchum mei ad. Qui ut tritani oblique detracto, mei odio consequat scriptorem eu. Has an solet vulputate, graeco omittam suscipit eu eos, ut usu doctus facilis. Consul vivendum atomorum mei at, errem fabulas hendrerit ea sea, nam illum timeam assentior et. Sint aliquid et vis, eam ne populo semper verterem, mel an viderer rationibus inciderint. Te nostro indoctum sea, omnis equidem assueverit no vim. Pri ullum dictas possim ea, usu iriure maiestatis disputando ea. Nec an magna ipsum epicuri, quis vidit paulo nec ei. Pri oporteat inimicus in, no sed nominati voluptatum.";
    
    
    
    //ofTranslate(-smooth);
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    ofRotate((targetAngleSmooth + targetAngleSmooth2)* RAD_TO_DEG, 0,0,1);
    ofScale(scaleSmooth2, scaleSmooth2);
    
    
    //temp.draw();
    
    temp = temp.getSmoothed(3);
    drawStringOnLine(s, temp);

}



//--------------------------------------------------------------
void angleScene::mouseDragged(int x, int y, int button){
    
    cout << "drag" << endl;
    
    templine.addVertex(ofPoint(x,y));
}

//--------------------------------------------------------------
void angleScene::mousePressed(int x, int y, int button){
    
    
    cout << "press" << endl;
    
    templine.clear();
}

//--------------------------------------------------------------
void angleScene::mouseReleased(int x, int y, int button){
    
    cout << "released" << endl;
    
    
    ofPolyline templine2 = templine.getResampledByCount(205);
    while(templine2.size() > 200){
        templine2.getVertices().pop_back();
    }
    
    if (aline.angleDiffs.size() == 0){
        aline.convertFrom(templine2);
    }
    
    alineTarget.convertFrom(templine2);
    
    
    
    std::reverse(templine2.begin(), templine2.end());
    
    alineTargetReversed.convertFrom(templine2);
    
    
    lockPct = 0;
    
    
    ofPolyline temp = alineTarget.convertBack();
    ofPoint diff2 = temp[0] - temp[temp.size()-1];
    angleLock = atan2(diff2.y, diff2.x);
    
}