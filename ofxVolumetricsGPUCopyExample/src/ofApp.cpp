#include "ofApp.h"


void ofApp::setup()
{
    fbo.allocate(256, 256);

    volWidth = 256;
    volHeight = 256;
    volDepth = 256;

    cout << "setting up volume data buffer at " << volWidth << "x" << volHeight << "x" << volDepth <<"\n";

    myVolume.setup(volWidth, volHeight, volDepth, ofVec3f(1,1,1),true);
    myVolume.setRenderSettings(1.0, 1.0, 0.75, 0.1);
    myVolume.setVolumeTextureFilterMode(GL_LINEAR);
    linearFilter = true;

    cam.setDistance(1000);
    cam.enableMouseInput();
}


void ofApp::update()
{
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}


void ofApp::draw()
{
    ofSetColor(255,255,255,255);
    cam.begin();
    myVolume.drawVolume(0, 0, 0, ofGetHeight(), 0);
    if (drawDebug) ofDrawAxis(100);
    cam.end();

    ofSetColor(0,0,0,64);
    ofDrawRectangle(0,0,270,120);
    ofSetColor(255,255,255,255);

    ofDrawBitmapString("Volume dimensions: " + ofToString(myVolume.getVolumeWidth()) + "x" + ofToString(myVolume.getVolumeHeight()) + "x" + ofToString(myVolume.getVolumeDepth()) + "\n" +
                       "FBO quality (q/Q): " + ofToString(myVolume.getRenderWidth()) + "x" + ofToString(myVolume.getRenderHeight()) + "\n" +
                       "Z quality (z/Z):   " + ofToString(myVolume.getZQuality()) + "\n" +
                       "Threshold (t/T):   " + ofToString(myVolume.getThreshold()) + "\n" +
                       "Density (d/D):     " + ofToString(myVolume.getDensity()) + "\n" +
                       "Filter mode (l/n): " + (linearFilter ? "linear" : "nearest") + "\n" +
                       "Draw debug (b):    " + (drawDebug ? "on" : "off"), 20, 25);
    fbo.begin();
    for (int x = 0; x < volDepth; x++) {
        ofClear(0);
        ofSetColor(ofRandom(255), 12, 0);
        ofDrawRectangle(ofRandom(20), ofRandom(20), 50, 50);
        ofSetColor(50, ofRandom(255), 100);
        ofDrawCircle(90 + ofRandom(20), 90 + ofRandom(20), 50);
        myVolume.updateTexture(0, 0, x, 0, 0, fbo.getWidth(), fbo.getHeight());
    }
    fbo.end();
}


void ofApp::keyPressed(int key)
{
    switch(key)
    {
    case 't':
        myVolume.setThreshold(myVolume.getThreshold()-0.01);
        break;
    case 'T':
        myVolume.setThreshold(myVolume.getThreshold()+0.01);
        break;
    case 'd':
        myVolume.setDensity(myVolume.getDensity()-0.01);
        break;
    case 'D':
        myVolume.setDensity(myVolume.getDensity()+0.01);
        break;
    case 'q':
        myVolume.setXyQuality(myVolume.getXyQuality()-0.01);
        break;
    case 'Q':
        myVolume.setXyQuality(myVolume.getXyQuality()+0.01);
        break;
    case 'z':
        myVolume.setZQuality(myVolume.getZQuality()-0.01);
        break;
    case 'Z':
        myVolume.setZQuality(myVolume.getZQuality()+0.01);
        break;
    case 'l':
        myVolume.setVolumeTextureFilterMode(GL_LINEAR);
        linearFilter = true;
        break;
    case 'n':
        myVolume.setVolumeTextureFilterMode(GL_NEAREST);
        linearFilter = false;
        break;
    case 'b':
        drawDebug = !drawDebug;
        myVolume.setDrawDebugVolume(drawDebug);
        break;
    }
}