#include "ofxTexture2d.h"

void ofxTexture2d::loadData(unsigned char * data, int w, int h, int xOffset, int yOffset, int glFormat)
{
    ofSetPixelStorei(w,1,ofGetNumChannelsFromGLFormat(glFormat));
    loadData((void *)data, w, h, xOffset, yOffset, glFormat, GL_UNSIGNED_BYTE);
}
void ofxTexture2d::loadData(float * data, int w, int h, int xOffset, int yOffset, int glFormat)
{
    ofSetPixelStorei(w,4,ofGetNumChannelsFromGLFormat(glFormat));
    loadData((void *)data, w, h, xOffset, yOffset, glFormat, GL_FLOAT);
}
void ofxTexture2d::loadData(unsigned short* data, int w, int h, int xOffset, int yOffset, int glFormat)
{
    ofSetPixelStorei(w,2,ofGetNumChannelsFromGLFormat(glFormat));
    loadData((void *)data, w, h, xOffset, yOffset, glFormat, GL_UNSIGNED_SHORT);
}
void ofxTexture2d::loadData(ofPixels & pix, int xOffset, int yOffset)
{
    ofSetPixelStorei(pix.getWidth(),pix.getBytesPerChannel(),pix.getNumChannels());
    loadData(pix.getPixels(), pix.getWidth(), pix.getHeight(), xOffset, yOffset, ofGetGlFormat(pix), ofGetGlType(pix));
}
void ofxTexture2d::loadData(ofShortPixels & pix, int xOffset, int yOffset)
{
    ofSetPixelStorei(pix.getWidth(),pix.getBytesPerChannel(),pix.getNumChannels());
    loadData(pix.getPixels(), pix.getWidth(), pix.getHeight(), xOffset, yOffset, ofGetGlFormat(pix), ofGetGlType(pix));
}
void ofxTexture2d::loadData(ofFloatPixels & pix, int xOffset, int yOffset)
{
    ofSetPixelStorei(pix.getWidth(),pix.getBytesPerChannel(),pix.getNumChannels());
    loadData(pix.getPixels(), pix.getWidth(), pix.getHeight(), xOffset, yOffset, ofGetGlFormat(pix), ofGetGlType(pix));
}

void ofxTexture2d::loadData(void * data, int w, int h, int xOffset, int yOffset, int glFormat, int glType)
{
    if(glFormat!=texData.glTypeInternal)
    {
        ofLogError() << "ofxTexture2d::loadData() failed to upload format " <<  ofGetGlInternalFormatName(glFormat) << " data to " << ofGetGlInternalFormatName(texData.glTypeInternal) << " texture" <<endl;
        return;
    }

    if(w > texData.tex_w || h > texData.tex_h )
    {
        ofLogError() << "ofxTexture2d::loadData() failed to upload " <<  w << "x" << h << " data to " << texData.tex_w << "x" << texData.tex_h << "x" << " texture";
        return;
    }
    
        //update the texture image: 
        enableTextureTarget();

        glBindTexture(texData.textureTarget, (GLuint) texData.textureID);

        glTexSubImage2D(texData.textureTarget, 0, xOffset, yOffset, w, h, glFormat, glType, data);

        disableTextureTarget();
}