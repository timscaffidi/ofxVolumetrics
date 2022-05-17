#pragma once
#include "ofMain.h"
#include "ofxVolumetrics.h"

class ofApp : public ofBaseApp
{
public:

    void setup();
    void update();
    void draw();
    void keyPressed  (int key);
   
    ofEasyCam cam;
    
    ofxVolumetrics myVolume;
    int volWidth, volHeight, volDepth;
    ofImage background;
    ofFbo fbo;
    bool linearFilter;
    bool drawDebug;
};
