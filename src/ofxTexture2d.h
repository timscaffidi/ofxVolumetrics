#pragma once

#include "ofTexture.h"
#include "ofMain.h"

class ofxTexture2d : public ofTexture
{
    public:
        void loadData(unsigned char * data, int w, int h, int xOffset, int yOffset, int glFormat);
        void loadData(float* data, int w, int h, int xOffset, int yOffset, int glFormat);
        void loadData(unsigned short* data, int w, int h, int xOffset, int yOffset, int glFormat);
        void loadData(ofPixels & pix, int xOffset, int yOffset);
        void loadData(ofShortPixels & pix, int xOffset, int yOffset);
        void loadData(ofFloatPixels & pix, int xOffset, int yOffset);
        void loadData(void * data, int w, int h, int xOffset, int yOffset, int glFormat, int glType);
};
