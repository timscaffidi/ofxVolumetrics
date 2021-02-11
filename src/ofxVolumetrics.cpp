#include "ofxVolumetrics.h"
#include "ofMain.h"

using namespace glm;

ofxVolumetrics::ofxVolumetrics() {
	quality = ofVec3f(1.0);
	threshold = 1.0 / 255.0;
	density = 1.0;
	volWidth = renderWidth = 0;
	volHeight = renderHeight = 0;
	volDepth = 0;
	bIsInitialized = false;

	vector<vec3> vertices {
		// front side
		vec3(1.0, 1.0, 1.0),
		vec3(0.0, 1.0, 1.0),
		vec3(1.0, 0.0, 1.0),
		vec3(0.0, 0.0, 1.0),
		// right side
		vec3(1.0, 1.0, 0.0),
		vec3(1.0, 1.0, 1.0),
		vec3(1.0, 0.0, 0.0),
		vec3(1.0, 0.0, 1.0),
		// top side
		vec3(0.0, 1.0, 0.0),
		vec3(0.0, 1.0, 1.0),
		vec3(1.0, 1.0, 0.0),
		vec3(1.0, 1.0, 1.0),		
		// left side
		vec3(0.0, 1.0, 1.0),
		vec3(0.0, 1.0, 0.0),		
		vec3(0.0, 0.0, 1.0),
		vec3(0.0, 0.0, 0.0),
		// bottom side
		vec3(1.0, 0.0, 1.0),
		vec3(0.0, 0.0, 1.0),
		vec3(1.0, 0.0, 0.0),
		vec3(0.0, 0.0, 0.0),		
		// back side
		vec3(0.0, 1.0, 0.0),
		vec3(1.0, 1.0, 0.0),	
		vec3(0.0, 0.0, 0.0),
		vec3(1.0, 0.0, 0.0)		
	};	
	vector<unsigned int> indices {
		// front side
		0, 2, 1, 1, 2, 3,
		// right side
		4, 6, 5, 5, 6, 7,
		// top side
		8, 10, 9, 9, 10, 11,
		// left side
		12, 14, 13, 13, 14, 15,
		// bottom side
		16, 18, 17, 17, 18, 19,
		// back side
		20, 22, 21, 21, 22, 23
	};
	volumeMesh.addVertices(vertices);
	volumeMesh.addIndices(indices);
}

ofxVolumetrics::~ofxVolumetrics() {
	destroy();
}

void ofxVolumetrics::setup(int w, int h, int d, ofVec3f voxelSize, bool usePowerOfTwoTexSize) {
	string shadersPath = "ofxVolumetrics/shaders/";
	shadersPath += ofIsGLProgrammableRenderer() ? "gl2/" : "gl3/";
	volumeShader.unload();
	volumeShader.load(shadersPath + "raycast.vert", shadersPath + "raycast.frag");

	bIsPowerOfTwo = usePowerOfTwoTexSize;

	volWidthPOT = volWidth = renderWidth = w;
	volHeightPOT = volHeight = renderHeight = h;
	volDepthPOT = volDepth = d;

	if (bIsPowerOfTwo) {
		volWidthPOT = ofNextPow2(w);
		volHeightPOT = ofNextPow2(h);
		volDepthPOT = ofNextPow2(d);

		ofLogVerbose() << "ofxVolumetrics::setup(): Using power of two texture size. Requested: " << w << "x" << h << "x" << d << ". Actual: " << volWidthPOT << "x" << volHeightPOT << "x" << volDepthPOT << ".\n";
	}

	fboRender.allocate(w, h, GL_RGBA8);
	volumeTexture.allocate(volWidthPOT, volHeightPOT, volDepthPOT, GL_RGBA8);
	if (bIsPowerOfTwo) {
		// if using cropped power of two, blank out the extra memory
		unsigned char* d;
		d = new unsigned char[volWidthPOT * volHeightPOT * volDepthPOT * 4];
		memset(d, 0, volWidthPOT * volHeightPOT * volDepthPOT * 4);
		volumeTexture.loadData(d, volWidthPOT, volHeightPOT, volDepthPOT, 0, 0, 0, GL_RGBA);
	}
	voxelRatio = voxelSize;

	bIsInitialized = true;
}

void ofxVolumetrics::destroy() {
	volumeShader.unload();
	volumeTexture.clear();

	volWidth = renderWidth = 0;
	volHeight = renderHeight = 0;
	volDepth = 0;
	bIsInitialized = false;
}

void ofxVolumetrics::updateVolumeData(unsigned char* data, int w, int h, int d, int xOffset, int yOffset, int zOffset) {
	volumeTexture.loadData(data, w, h, d, xOffset, yOffset, zOffset, GL_RGBA);
}

void ofxVolumetrics::drawVolume(float x, float y, float z, float size, int zTexOffset) {
	ofVec3f volumeSize = voxelRatio * ofVec3f(volWidth, volHeight, volDepth);
	float maxDim = glm::max(glm::max(volumeSize.x, volumeSize.y), volumeSize.z);
	volumeSize = volumeSize * size / maxDim;

	drawVolume(x, y, z, volumeSize.x, volumeSize.y, volumeSize.z, zTexOffset);
}

void ofxVolumetrics::drawVolume(float x, float y, float z, float w, float h, float d, int zTexOffset) {
	updateRenderDimentions();

	ofVec3f cubeSize = ofVec3f(w, h, d);

	// store current color
	GLint color[4];
	glGetIntegerv(GL_CURRENT_COLOR, color);

	// store current cull mode
	GLint cull_mode;
	glGetIntegerv(GL_FRONT_FACE, &cull_mode);

	// set fbo cull mode
	GLfloat matModelview[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, matModelview);	
	ofVec3f scale, t;
	ofQuaternion a, b;
	ofMatrix4x4(matModelview).decompose(t, a, scale, b);
	GLint cull_mode_fbo = (scale.x * scale.y * scale.z) > 0 ? GL_CCW : GL_CW;	

	// raycasting pass
	fboRender.begin(OF_FBOMODE_NODEFAULTS);
	{
		ofClear(0, 0);

		ofTranslate(x - cubeSize.x / 2, y - cubeSize.y / 2, z - cubeSize.z / 2);
		ofScale(cubeSize.x, cubeSize.y, cubeSize.z);

		volumeShader.begin();
		volumeShader.setUniformTexture("volume_tex", GL_TEXTURE_3D, volumeTexture.getTextureData().textureID, 0);
		volumeShader.setUniform3f("vol_d", (float)volWidth, (float)volHeight, (float)volDepth); //dimensions of the volume texture
		volumeShader.setUniform3f("vol_d_pot", (float)volWidthPOT, (float)volHeightPOT, (float)volDepthPOT); //dimensions of the volume texture power of two
		volumeShader.setUniform2f("bg_d", (float)renderWidth, (float)renderHeight); // dimensions of the background texture
		volumeShader.setUniform1f("zoffset", zTexOffset); // used for animation so that we dont have to upload the entire volume every time
		volumeShader.setUniform1f("quality", quality.z); // 0 ... 1
		volumeShader.setUniform1f("density", density); // 0 ... 1
		volumeShader.setUniform1f("threshold", threshold);//(float)mouseX/(float)ofGetWidth());

		glFrontFace(cull_mode_fbo);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		volumeMesh.drawFaces();
		glDisable(GL_CULL_FACE);
		glFrontFace(cull_mode);

		volumeShader.end();

		//volumeMesh.drawWireframe();
	}
	fboRender.end();

	ofPushView();

	glColor4iv(color);
	ofSetupScreenOrtho();		
	fboRender.draw(0, 0, ofGetWidth(), ofGetHeight());

	ofPopView();
}

void ofxVolumetrics::updateRenderDimentions() {
	if ((int)(ofGetWidth() * quality.x) != renderWidth) {
		renderWidth = ofGetWidth() * quality.x;
		renderHeight = ofGetHeight() * quality.x;
		fboRender.allocate(renderWidth, renderHeight, GL_RGBA);
	}
}

void ofxVolumetrics::setXyQuality(float q) {
	quality.x = MAX(q, 0.01);

	updateRenderDimentions();
}
void ofxVolumetrics::setZQuality(float q) {
	quality.z = MAX(q, 0.01);
}
void ofxVolumetrics::setThreshold(float t) {
	threshold = ofClamp(t, 0.0, 1.0);
}
void ofxVolumetrics::setDensity(float d) {
	density = MAX(d, 0.0);
}
void ofxVolumetrics::setRenderSettings(float xyQuality, float zQuality, float dens, float thresh) {
	setXyQuality(xyQuality);
	setZQuality(zQuality);
	setDensity(dens);
	setThreshold(thresh);
}

void ofxVolumetrics::setVolumeTextureFilterMode(GLint filterMode) {
	volumeTexture.setTextureMinMagFilter(filterMode);
}

bool ofxVolumetrics::isInitialized() {
	return bIsInitialized;
}
int ofxVolumetrics::getVolumeWidth() {
	return volWidth;
}
int ofxVolumetrics::getVolumeHeight() {
	return volHeight;
}
int ofxVolumetrics::getVolumeDepth() {
	return volDepth;
}
int ofxVolumetrics::getRenderWidth() {
	return renderWidth;
}
int ofxVolumetrics::getRenderHeight() {
	return renderHeight;
}
float ofxVolumetrics::getXyQuality() {
	return quality.x;
}
float ofxVolumetrics::getZQuality() {
	return quality.z;
}
float ofxVolumetrics::getThreshold() {
	return threshold;
}
float ofxVolumetrics::getDensity() {
	return density;
}
ofFbo& ofxVolumetrics::getFboReference() {
	return fboRender;
}
