//
//  ofApp.h
//  mmMuse
//
//  Created by Albert Kim on 5/8/17.
//
//

#pragma once

#include "ofMain.h"

#include "Particle.h"
#include "ofxFaceTracker.h"
#include "ofxNetwork.h"
#include "ofxOsc.h"
#include <cmath>

const int port = 5000;

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void resetParticles();
    void keyPressed(int key);
    
    int Width;
    int Height;
    int appScreenWidth;
    int appScreenHeight;
    
    ofxOscReceiver receiver;
    ofxOscMessage m;
    
    // Muse values
    int bTouchingForehead;
    float signalQuality;
    float alpha;
    float beta;
    float gamma;
    float theta;
    float delta;
    float alphaRelative;
    float gammaRelative;
    float attention;
    float meditation;
    
    // facetracking
    ofVideoGrabber cam;
    ofxFaceTracker tracker;
    
    ofPolyline leftEye;
    ofPolyline rightEye;
    ofPolyline faceOutline;
    ofPolyline jaw;
    
    particleMode currentMode;
    string currentModeStr;
    
    vector <Particle> p;
    vector <ofPoint> attractPoints;
    vector <ofPoint> attractPointsWithMovement;
    
    Particle particles;
    
    ofPoint centerOfFace;
    ofPoint centerOfRightEye;
    ofPoint centerOfLeftEye;
    ofPoint centerOfMouth;
    ofPoint centerOfJaw;
    
    //ofxCvColorImage colorImage;
    ofPixels pixels;
    
    float sig;
    float att;
    float med;
    
    //    vector <string> parsedMsg;
    float medSize;
    
    int viewMode;
    
    float transAtt, transMed;
    float a1, a2, a3, a4, a5;
    float m1, m2, m3, m4, m5;
    
};
