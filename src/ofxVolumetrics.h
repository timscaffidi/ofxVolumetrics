#pragma once

/*      ofxVolumetrics - render volumetric data on the GPU

 Written by Timothy Scaffidi (http://timothyscaffidi.com)
 Volumetric rendering algorithm adapted from Peter Trier (http://www.daimi.au.dk/~trier/?page_id=98)

*/

#include "ofFbo.h"
#include "ofShader.h"
#include "ofxTexture3d.h"
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
    ofVec3f getSlicePoint();
    ofVec3f getSliceNormal();
    void setXyQuality(float q);
    void setZQuality(float q);
    void setThreshold(float t);
    void setDensity(float d);
    void setRenderSettings(float xyQuality, float zQuality, float dens, float thresh);
    void setVolumeTextureFilterMode(GLint filterMode);
    // slice point: the intersection (center) of the slicing plane
    // slice normal: the normal vector of the slicing plane
    //               ofVec(0,  1, 0) to render back of skeleton
    //               ofVec(0, -1, 0) to render front of skeleton
    void setSlice(ofVec3f, ofVec3f);
    void setSlicePoint(ofVec3f);
    void setSliceNormal(ofVec3f);
protected:
private:
    void drawRGBCube();
    void updateRenderDimentions();

    ofFbo fboRender;
    ofShader volumeShader;
    ofxTexture3d volumeTexture;
    //ofMesh volumeMesh; //unfortunately this only supports 2d texture coordinates at the moment.
    ofVec3f volVerts[24];
    ofVec3f volNormals[24];
    ofVec3f voxelRatio;
    bool bIsInitialized;
    int volWidth, volHeight, volDepth;
    int volWidthPOT, volHeightPOT, volDepthPOT;
    bool bIsPowerOfTwo;
    ofVec3f quality;
    float threshold;
    float density;
    int renderWidth, renderHeight;
    ofVec3f slice_p, slice_n;
};
