#pragma once

/*      
 ofxVolumetrics - render volumetric data on the GPU

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
	
	void setup(int w, int h, int d, glm::vec3 voxelSize, bool usePowerOfTwoTexSize = false);
	
	void destroy();
	
	void updateVolumeData(unsigned char* data, int w, int h, int d, int xOffset, int yOffset, int zOffset);
	void updateTexture(int xOffset, int yOffset, int zOffset, int x, int y, int width, int height);
	
	void drawVolume(float x, float y, float z, float size, int zTexOffset);
	void drawVolume(float x, float y, float z, float w, float h, float d, int zTexOffset);
	
	bool isInitialized();	
	int getVolumeWidth();
	int getVolumeHeight();
	int getVolumeDepth();
	const ofFbo& getFbo() const;
	int getRenderWidth();
	int getRenderHeight();
	float getXyQuality();
	float getZQuality();
	float getThreshold();
	float getDensity();
	ofxTextureData3d getTextureData();
	
	void setXyQuality(float q);
	void setZQuality(float q);
	void setThreshold(float t);
	void setDensity(float d);
	void setRenderSettings(float xyQuality, float zQuality, float dens, float thresh);
	void setVolumeTextureFilterMode(GLint filterMode);
	void setDrawDebugVolume(bool b);

protected:

private:

	void updateRenderDimentions();

	ofFbo fboRender;
	ofShader volumeShader;
	ofxTexture3d volumeTexture;
	ofVboMesh volumeMesh; 	
	
	glm::vec3 voxelRatio;
	bool bIsInitialized;
	int volWidth, volHeight, volDepth;
	int volWidthPOT, volHeightPOT, volDepthPOT;
	bool bIsPowerOfTwo;
	glm::vec3 quality;
	float threshold;
	float density;
	int renderWidth, renderHeight;
	
	bool bDrawDebugVolume;
};
