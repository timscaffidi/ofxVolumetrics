#pragma once

/* ofxVolumetrics - render volumetric data on the GPU

 Written by Timothy Scaffidi (http://timothyscaffidi.com)
 Volumetric rendering algorithm adapted from Peter Trier (http://www.daimi.au.dk/~trier/?page_id=98)

*/
#include "ofMain.h"

#ifndef OFX_VOLUMETRICS_EMULATE_3D_TEXTURE
    #ifndef GL_TEXTURE_3D
       #define OFX_VOLUMETRICS_EMULATE_3D_TEXTURE
    #endif
#endif

#include "ofFbo.h"
#include "ofShader.h"

#ifdef OFX_VOLUMETRICS_EMULATE_3D_TEXTURE
    #include "ofxTexture2d.h"
 #else
    #include "ofxTexture3d.h"
#endif

#include "ofxImageSequencePlayer.h"

class ofxVolumetrics
{
public:
    ofxVolumetrics();
    virtual ~ofxVolumetrics();
    void setup(int w, int h, int d, ofVec3f voxelSize, bool usePowerOfTwoTexSize=false);
    void destroy();
    void updateVolumeData(unsigned char * data, int w, int h, int d, int xOffset, int yOffset, int zOffset);
    void drawVolume(float x, float y, float z, float size, int zTexOffset);
    void drawVolume(float x, float y, float z, float w, float h, float d, int zTexOffset);
    bool isInitialized();
    int getVolumeWidth();
    int getVolumeHeight();
    int getVolumeDepth();
    ofFbo & getFboReference();
    int getRenderWidth();
    int getRenderHeight();
    float getXyQuality();
    float getZQuality();
    float getThreshold();
    float getDensity();
    void setXyQuality(float q);
    void setZQuality(float q);
    void setThreshold(float t);
    void setDensity(float d);
    void setRenderSettings(float xyQuality, float zQuality, float dens, float thresh);
    void setVolumeTextureFilterMode(GLint filterMode);
protected:
private:
    void drawRGBCube();
    void updateRenderDimentions();

    ofFbo fboRender;
    ofShader volumeShader;
    GLint gl_max_tex_size;
#ifndef OFX_VOLUMETRICS_EMULATE_3D_TEXTURE
    GLint gl_max_tex_3d_depth;
    ofxTexture3d volumeTexture;
#else
    ofxTexture2d volumeTexture;
    unsigned int numFramesX, numFramesY;
#endif
    //ofMesh volumeMesh; //unfortunately this only supports 2d texture coordinates at the moment.
    ofVec3f volVerts[24];
    ofVec3f volNormals[24];
    ofVec3f voxelRatio;
    bool bIsInitialized;
    int volWidth, volHeight, volDepth;
    int volTexWidth, volTexHeight, volTexDepth;
    bool bIsPowerOfTwo;
    ofVec3f quality;
    float threshold;
    float density;
    int renderWidth, renderHeight;
};
