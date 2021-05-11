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
    unsigned char * volumeData;
    int volWidth, volHeight, volDepth;
    ofImage background;
    ofxImageSequencePlayer imageSequence;
    bool linearFilter;
    bool drawDebug;
};
