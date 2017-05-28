#include "ofApp.h"

using namespace ofxCv;

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofSetFrameRate(30);
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
    ofBackground(0);
    
    Width = ofGetWidth();
    Height = ofGetHeight();
    WindowWidth = ofGetWindowWidth();
    WindowHeight = ofGetWindowHeight();
    
    // set our port
    receiver.setup(port);
    
    // muse values
    bTouchingForehead = 0;
    signalQuality = 0;
    alpha = 0.0;
    beta = 0.0;
    gamma = 0.0;
    delta = 0.0;
    theta = 0.0;
    attention = 0.0;
    meditation = 0.0;
    
    // initialize camera
    cam.initGrabber(640, 480);
    //    camWidth = 640;
    //    camHeight = 480;
    cam.setVerbose(true);
    //    cam.initGrabber(camWidth, camHeight * 3);
    //    mirrorImage = new unsigned char(camWidth * camHeight * 3);
    //    mirror.allocate(camWidth, camHeight, GL_RGB);
    
    tracker.setup();
    tracker.setRescale(.5);
    
    int num = 7000;
    p.assign(num, Particle());
    currentMode = PARTICLE_MODE_NEAREST_POINTS;
    resetParticles();
    
    centerOfFace.set(0,0,0);
    centerOfRightEye.set(0,0,0);
    centerOfLeftEye.set(0,0,0);
    centerOfJaw.set(0,0,0);
    
    transAtt = 0;
    transMed = 0;
    //-----------
    
}


//--------------------------------------------------------------
void ofApp::resetParticles(){
    
    //these are the attraction points used in the forth 
    attractPoints.clear();
    for(int i = 0; i < 4; i++){
        attractPoints.push_back( ofPoint(0,0) );
    }
    
    attractPointsWithMovement = attractPoints;
    
    for(vector<Particle>::iterator it = p.begin(); it != p.end(); it++ ){
        it -> setMode(currentMode);
        it -> setAttractPoints(&attractPointsWithMovement);;
        it -> reset();
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    
    while (receiver.hasWaitingMessages()) {
    
        receiver.getNextMessage(&m);
        
        if (m.getAddress() == "/muse/elements/touching_forehead") {
            bTouchingForehead = m.getArgAsInt32(0);
            //cout << "Touching Forehead: " << bTouchingForehead << endl;
        }
        if (m.getAddress() == "/muse/elements/horseshoe") {
            signalQuality = m.getArgAsFloat(1);
        }
        
        if (m.getAddress() == "/muse/elements/alpha_absolute") {
            alpha = m.getArgAsFloat(0);
            //cout << "Alpha: " << alpha << endl;
        }
        
        if (m.getAddress() == "/muse/elements/beta_absolute") {
            beta = m.getArgAsFloat(0);
            //cout << "Beta: " << beta << endl;
        }
        
        if (m.getAddress() == "/muse/elements/gamma_absolute") {
            gamma = m.getArgAsFloat(0);
            //cout << "Gamma: " << gamma << endl;
        }
        
        if (m.getAddress() == "/muse/elements/delta_absolute") {
            delta = m.getArgAsFloat(0);
            //cout << "Delta: " << delta << endl;
        }

        if (m.getAddress() == "/muse/elements/theta_absolute") {
            theta = m.getArgAsFloat(0);
            //cout << "Theta: " << theta << endl;
        }
        
        alphaRelative = (pow(10, alpha) / (pow(10, alpha) + pow(10, beta) + pow(10, gamma) + pow(10, delta) + pow(10, theta)));
        gammaRelative = (pow(10, gamma) / (pow(10, alpha) + pow(10, beta) + pow(10, gamma) + pow(10, delta) + pow(10, theta)));
        cout << "alpha relative: " << alphaRelative << endl;
        cout << "gamma relative: " << gammaRelative << endl;
        
        meditation = ofMap(alphaRelative, 0.3, 0.0, 0, 100);
        attention = ofMap(gammaRelative, 0.2, 0.0, 0, 100);
    }
    
    cam.update();
    if(cam.isFrameNew()) {
        tracker.update(toCv(cam));
        ofTexture _text;
        _text = cam.getTexture();
        _text.readToPixels(pixels);
        
    }
    
    
    for(vector<Particle>::iterator it=p.begin(); it!=p.end(); it++){
        it -> setMode(currentMode);
        it -> update(centerOfFace, attractPoints, transAtt, transMed);
        it -> color.set( pixels.getColor( ofMap(it -> pos.x, 0, WindowWidth, 0, 640), ofMap(it -> pos.y, 0, WindowHeight, 0, 480) ));
        
    }
    
    // Attention values = particle size/resolution
    if(attention >= 0 && attention <= 10){
        transAtt = 4.2;
    }
    if(attention >=11 && attention <= 20){
        transAtt = 3.8;
    }
    if(attention >=21 && attention <= 30){
        transAtt = 3.4;
    }
    if(attention >=31 && attention <= 40){
        transAtt = 2.8;
    }
    if(attention >=41 && attention <= 50){
        transAtt = 2.4;
    }
    if(attention >= 51 && attention <= 60){
        transAtt = 2.1;
    }
    if(attention >= 61 && attention <=70){
        transAtt = 1.7;
    }
    if(attention >= 71 && attention <=80){
        transAtt = 1.4;
    }
    if(attention >= 81 && attention <= 90){
        transAtt = 1.1;
    }
    if(attention >=91 && attention <= 100){
        transAtt = 0.7;
    }
    
    // Meditation values = proximity to facial features
    if(meditation >= 0 && meditation <= 10){
        transMed = ofRandom(0.90,0.98);
        particles.frcVar = 5;
        for(vector<Particle>::iterator it = p.begin(); it != p.end(); it++ ){
            it -> chaos(1.5);
        }
    }
    if(meditation >=11 && meditation <= 20){
        transMed = ofRandom(0.91,0.98);
        particles.frcVar = 4;
        for(vector<Particle>::iterator it = p.begin(); it != p.end(); it++ ){
            it -> chaos(1.25);
        }
    }
    if(meditation >=21 && meditation <= 30){
        transMed = ofRandom(0.92,0.98);
        particles.frcVar = 3;
        for(vector<Particle>::iterator it = p.begin(); it != p.end(); it++ ){
            it -> chaos(1.0);
        }
    }
    if(meditation >=31 && meditation <= 40){
        transMed = ofRandom(0.93,0.98);
        particles.frcVar = 2;
        for(vector<Particle>::iterator it = p.begin(); it != p.end(); it++ ){
            it -> chaos(0.8);
        }
    }
    if(meditation >=41 && meditation <= 50){
        transMed = ofRandom(0.94,0.98);
        particles.frcVar = 1.7;
        for(vector<Particle>::iterator it = p.begin(); it != p.end(); it++ ){
            it -> chaos(0.6);
        }
    }
    if(meditation >= 51 && meditation <= 60){
        transMed = ofRandom(0.97,0.98);
        particles.frcVar = 1.4;
        for(vector<Particle>::iterator it = p.begin(); it != p.end(); it++ ){
            it -> chaos(0.4);
        }
    }
    if(meditation >= 61 && meditation <= 70){
        transMed = ofRandom(0.97,0.98);
        particles.frcVar = 1.1;
        for(vector<Particle>::iterator it = p.begin(); it != p.end(); it++ ){
            it -> chaos(0.3);
        }
    }
    if(meditation >= 71 && meditation <= 80){
        transMed = ofRandom(0.97,0.98);
        particles.frcVar = 0.8;
        for(vector<Particle>::iterator it = p.begin(); it != p.end(); it++ ){
            it -> chaos(0.2);
        }
    }
    if(meditation >= 81 && meditation <= 90){
        transMed = ofRandom(0.97,0.98);
        particles.frcVar = 0.7;
        for(vector<Particle>::iterator it = p.begin(); it != p.end(); it++ ){
            it -> chaos(0.18);
        }
    }
    if(meditation >= 91 && meditation <= 100){
        transMed = ofRandom(0.97,0.98);
        particles.frcVar = 0.4;
        for(vector<Particle>::iterator it = p.begin(); it != p.end(); it++ ){
            it -> chaos(0.15);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255);
    
    //	ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);
    
    leftEye = tracker.getImageFeature(ofxFaceTracker::LEFT_EYE);
    rightEye = tracker.getImageFeature(ofxFaceTracker::RIGHT_EYE);
    faceOutline = tracker.getImageFeature(ofxFaceTracker::FACE_OUTLINE);
    jaw = tracker.getImageFeature(ofxFaceTracker::JAW);
    
    
    // mapping image to screen dimension
    if(faceOutline.size()>0){
        centerOfFace.set(faceOutline.getCentroid2D() );
        centerOfRightEye.set(rightEye.getCentroid2D() );
        centerOfLeftEye.set(leftEye.getCentroid2D() );
        centerOfJaw.set(jaw.getCentroid2D() );
        attractPoints[0].x = ofMap(centerOfFace.x, 0, 640, 0, WindowWidth );
        attractPoints[0].y = ofMap(centerOfFace.y, 0, 480, 0, WindowHeight );
        attractPoints[1].x = ofMap(centerOfRightEye.x, 0, 640, 0, WindowWidth );
        attractPoints[1].y = ofMap(centerOfRightEye.y, 0, 480, 0, WindowHeight );
        attractPoints[2].x = ofMap(centerOfLeftEye.x, 0, 640, 0, WindowWidth );
        attractPoints[2].y = ofMap(centerOfLeftEye.y, 0, 480, 0, WindowHeight );
        attractPoints[3].x = ofMap(centerOfJaw.x, 0, 640, 0, WindowWidth );
        attractPoints[3].y = ofMap(centerOfJaw.y, 0, 480, 0, WindowHeight );
    }
    
    for(vector<Particle>::iterator it=p.begin(); it!=p.end(); it++){
        it -> draw();
    }
    
    if(viewMode > 0){
        //draw fps
        ofSetColor(200);
        ofDrawBitmapString("fps: "+ofToString(ofGetFrameRate()), 20, 20);
        
        //draw face tracking
        ofSetLineWidth(2);
        ofSetColor(ofColor::red);
        leftEye.draw();
        ofSetColor(ofColor::green);
        rightEye.draw();
        ofSetColor(ofColor::blue);
        faceOutline.draw();
        
        ofSetLineWidth(1);
        ofSetColor(255);
        tracker.draw();
    }
    
    if(viewMode > 1){
        ofSetColor(255, 50, 150);
        ofDrawRectangle(Width-35, 0, 35, meditation*5);
        ofSetColor(50, 255, 150);
        ofDrawRectangle(Width-70, 0, 35, attention*5);
        ofSetColor(255);
        ofDrawBitmapString("M:"+ofToString(meditation), Width-35, 15);
        ofDrawBitmapString("A:"+ofToString(attention), Width-70, 15);
    
        if (sig > 0) {
            ofSetColor(255, 0, 30);
            ofDrawCircle(ofGetWidth()-200, 30, 30);
        }
    }
    
    if(viewMode > 2){
        cam.draw(50, ofGetHeight()-600);
    }
    
    // Indicator Light for Signal
    if(signalQuality >= 3){
        ofSetColor(255,0,0);
        ofDrawRectangle(0, 0, 8, 8);
    }else if (signalQuality == 2){
        ofSetColor(255,255,0);
        ofDrawRectangle(0, 0, 8, 8);
    }else if (signalQuality == 1){
        ofSetColor(0,255,0);
        ofDrawRectangle(0, 0, 8, 8);
    }
    
    
    ofSetColor(0);
    ofDrawRectangle(0, WindowHeight-50, WindowWidth, 50);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if(key == 'r') {
        tracker.reset();
    }
    
    if(key == 'v'){
        viewMode ++;
        if(viewMode > 3) viewMode = 0;
    }
    
    if(key == OF_KEY_SHIFT) viewMode = 3;
    
    if(key == 'f') ofToggleFullscreen();
}