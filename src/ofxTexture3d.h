#pragma once

#include "ofTexture.h"
#include "ofMain.h"

class ofxTextureData3d : public ofTextureData
{
    public:
    ofxTextureData3d() {
		textureID = 0;

		glTypeInternal = GL_RGB8;
		textureTarget = GL_TEXTURE_3D;

		glType = GL_RGB;
		pixelType = GL_UNSIGNED_BYTE;

		tex_t = 0;
		tex_u = 0;
		tex_v = 0;
		tex_w = 0;
		tex_h = 0;
		tex_d = 0;
		width = 0;
		height = 0;
		depth = 0;

		bFlipTexture = false;
		compressionType = OF_COMPRESS_NONE;
		bAllocated = false;
	}

	float tex_v;
	float tex_d;
	float depth;
    int glType, pixelType;
    int glTypeInternal;
};

class ofxTexture3d
{
    public:
        ofxTexture3d();
        virtual ~ofxTexture3d();
        void allocate(int w, int h, int d, int internalGlDataType);
        void loadData(unsigned char * data, int w, int h, int d, int xOffset, int yOffset, int zOffset, int glFormat);
        void loadData(float* data, int w, int h, int d, int xOffset, int yOffset, int zOffset, int glFormat);
        void loadData(unsigned short* data, int w, int h, int d, int xOffset, int yOffset, int zOffset, int glFormat);
        void loadData(ofPixels & pix, int d, int xOffset, int yOffset, int zOffset);
        void loadData(ofShortPixels & pix, int d, int xOffset, int yOffset, int zOffset);
        void loadData(ofFloatPixels & pix, int d, int xOffset, int yOffset, int zOffset);
        void bind();
        void unbind();
        void clear();
        ofxTextureData3d getTextureData();
    protected:
        void loadData(void * data, int w, int h, int d, int xOffset, int yOffset, int zOffset, int glFormat);
    private:
        ofxTextureData3d texData;
};
