#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
    ofSetFrameRate(60);
    background.loadImage("background.png");
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);

    imageSequence.init("volumes/head/cthead-8bit",3,".tif", 1);
    volWidth = imageSequence.getWidth();
    volHeight = imageSequence.getHeight();
    volDepth = imageSequence.getSequenceLength();

    cout << "setting up volume data buffer at " << volWidth << "x" << volHeight << "x" << volDepth <<"\n";

    volumeData = new unsigned char[volWidth*volHeight*volDepth*4];

    for(int z=0; z<volDepth; z++)
    {
        imageSequence.loadFrame(z);
        for(int x=0; x<volWidth; x++)
        {
            for(int y=0; y<volHeight; y++)
            {
                // convert from greyscale to RGBA, false color
                int i4 = ((x+volWidth*y)+z*volWidth*volHeight)*4;
                int sample = imageSequence.getPixels()[x+y*volWidth];
                ofColor c;
                c.setHsb(sample, 255-sample, sample);

                volumeData[i4] = c.r;
                volumeData[i4+1] = c.g;
                volumeData[i4+2] = c.b;
                volumeData[i4+3] = sample;
            }
        }
    }

    myVolume.setup(volWidth, volHeight, volDepth, ofVec3f(1,1,2));
    myVolume.updateVolumeData(volumeData,volWidth,volHeight,volDepth,0,0,0);
    myVolume.setRenderSettings(0.5, 1.0, 0.75, 0.1);

    linearFilter = false;
}

//--------------------------------------------------------------
void testApp::update()
{
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofSetColor(255,255,255,255);
    background.draw(0,0,ofGetWidth(),ofGetHeight());

    ofPushMatrix();
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2, -100);
    ofRotateX(mouseY*-360/(float)ofGetHeight());
    ofRotateZ(mouseX*360/(float)ofGetWidth());

    myVolume.drawVolume(0,0,0, ofGetHeight()*0.75, 0);
    ofPopMatrix();

    ofSetColor(0,0,0,100);
    ofRect(0,0,270, 90);
    ofSetColor(255,255,255,255);

    ofDrawBitmapString("volume dimensions: " + ofToString(myVolume.getVolumeWidth()) + "x" + ofToString(myVolume.getVolumeHeight()) + "x" + ofToString(myVolume.getVolumeDepth()) + "\n" +
                       "FBO quality (q/Q): " + ofToString(myVolume.getRenderWidth()) + "x" + ofToString(myVolume.getRenderHeight()) + "\n" +
                       "Z quality (z/Z):   " + ofToString(myVolume.getZQuality()) + "\n" +
                       "Threshold (t/T):   " + ofToString(myVolume.getThreshold()) + "\n" +
                       "Density (d/D):     " + ofToString(myVolume.getDensity()) + "\n" +
                       "Filter mode (l/n): " + (linearFilter?"linear":"nearest"),20,20);

}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
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
    }

}

//--------------------------------------------------------------
void testApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y )
{

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo)
{

}
