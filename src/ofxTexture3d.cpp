#include "ofxTexture3d.h"
#include "ofTexture.cpp"

ofxTexture3d::ofxTexture3d() {
	//ctor
}

ofxTexture3d::~ofxTexture3d() {
	release(texData.textureID);
}

void ofxTexture3d::allocate(int w, int h, int d, int internalGlDataType) {
	texData.tex_w = w;
	texData.tex_h = h;
	texData.tex_d = d;
	texData.tex_t = w;
	texData.tex_u = h;
	texData.tex_v = d;
	texData.textureTarget = GL_TEXTURE_3D;

	texData.glTypeInternal = internalGlDataType;
	texData.glInternalFormat = internalGlDataType;
	// get the glType (format) and pixelType (type) corresponding to the glTypeInteral (internalFormat)
	texData.glType = ofGetGLFormatFromInternal(texData.glTypeInternal);
	texData.pixelType = ofGetGlTypeFromInternal(texData.glTypeInternal);
	// attempt to free the previous bound texture, if we can:
	clear();

	glGenTextures(1, (GLuint*)&texData.textureID);
	retain(texData.textureID);
	glEnable(texData.textureTarget);
	glBindTexture(texData.textureTarget, (GLuint)texData.textureID);

	glTexImage3D(texData.textureTarget, 0, texData.glTypeInternal, (GLint)texData.tex_w, (GLint)texData.tex_h, (GLint)texData.tex_d, 0, texData.glType, texData.pixelType, 0);

	glTexParameteri(texData.textureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(texData.textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(texData.textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texData.textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(texData.textureTarget, GL_TEXTURE_WRAP_R, GL_REPEAT);

	#ifndef TARGET_EMSCRIPTEN
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	#endif
	
	glDisable(texData.textureTarget);

	texData.width = w;
	texData.height = h;
	texData.depth = d;
	texData.bFlipTexture = false;
	texData.bAllocated = true;
}

void ofxTexture3d::loadData(unsigned char* data, int w, int h, int d, int xOffset, int yOffset, int zOffset, int glFormat) {
	loadData((void*)data, w, h, d, xOffset, yOffset, zOffset, glFormat);
}
void ofxTexture3d::loadData(float* data, int w, int h, int d, int xOffset, int yOffset, int zOffset, int glFormat) {
	loadData((void*)data, w, h, d, xOffset, yOffset, zOffset, glFormat);
}
void ofxTexture3d::loadData(unsigned short* data, int w, int h, int d, int xOffset, int yOffset, int zOffset, int glFormat) {
	loadData((void*)data, w, h, d, xOffset, yOffset, zOffset, glFormat);
}
void ofxTexture3d::loadData(ofPixels& pix, int d, int xOffset, int yOffset, int zOffset) {
	loadData(pix.getData(), pix.getWidth(), pix.getHeight(), d, xOffset, yOffset, zOffset, ofGetGlFormat(pix));
}
void ofxTexture3d::loadData(ofShortPixels& pix, int d, int xOffset, int yOffset, int zOffset) {
	loadData(pix.getData(), pix.getWidth(), pix.getHeight(), d, xOffset, yOffset, zOffset, ofGetGlFormat(pix));
}
void ofxTexture3d::loadData(ofFloatPixels& pix, int d, int xOffset, int yOffset, int zOffset) {
	loadData(pix.getData(), pix.getWidth(), pix.getHeight(), d, xOffset, yOffset, zOffset, ofGetGlFormat(pix));
}

void ofxTexture3d::loadData(void* data, int w, int h, int d, int xOffset, int yOffset, int zOffset, int glFormat) {
	if (glFormat != texData.glType) {
		ofLogError() << "ofxTexture3d::loadData() failed to upload format " << ofGetGlInternalFormatName(glFormat) << " data to " << ofGetGlInternalFormatName(texData.glType) << " texture" << std::endl;
		return;
	}

	if (w > texData.tex_w || h > texData.tex_h || d > texData.tex_d) {
		ofLogError() << "ofxTexture3d::loadData() failed to upload " << w << "x" << h << "x" << d << " data to " << texData.tex_w << "x" << texData.tex_h << "x" << texData.tex_d << " texture";
		return;
	}

	ofSetPixelStoreiAlignment(GL_UNPACK_ALIGNMENT, w, 1, ofGetNumChannelsFromGLFormat(glFormat));
	glEnable(texData.textureTarget);
	glBindTexture(texData.textureTarget, (GLuint)texData.textureID);
	glTexSubImage3D(texData.textureTarget, 0, xOffset, yOffset, zOffset, w, h, d, texData.glType, texData.pixelType, data);
	glDisable(texData.textureTarget);

}

void ofxTexture3d::loadTexture(int xOffset, int yOffset, int zOffset, int x, int y, int width, int height) {
	glEnable(texData.textureTarget);
	glBindTexture(texData.textureTarget, (GLuint)texData.textureID);
	glCopyTexSubImage3D(texData.textureTarget, 0, xOffset, yOffset, zOffset, x, y, width, height);
	glDisable(texData.textureTarget);
}

void ofxTexture3d::clear() {
	release(texData.textureID);
	texData.textureID = 0;
	texData.bAllocated = false;
}

void ofxTexture3d::setTextureMinMagFilter(GLint filterMode)
{
	if (filterMode != GL_NEAREST && filterMode != GL_LINEAR) return;

	bind();
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filterMode);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, filterMode);
	unbind();
}

void ofxTexture3d::bind() {
	//we could check if it has been allocated - but we don't do that in draw()
	glEnable(texData.textureTarget);
	glBindTexture(texData.textureTarget, (GLuint)texData.textureID);
}

void ofxTexture3d::bindAsImage(GLuint unit, GLenum access, GLint level, GLboolean layered, GLint layer) {
	glBindImageTexture(unit, texData.textureID, level, layered, layer, access, texData.glInternalFormat);
}

//----------------------------------------------------------
void ofxTexture3d::unbind() {
	glBindTexture(texData.textureTarget, 0);
	glDisable(texData.textureTarget);
}

ofxTextureData3d ofxTexture3d::getTextureData() {
	if (!texData.bAllocated) {
		ofLog(OF_LOG_ERROR, "getTextureData() - texture has not been allocated");
	}

	return texData;
}
