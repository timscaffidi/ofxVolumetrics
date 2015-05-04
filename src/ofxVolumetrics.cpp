

#include "ofxVolumetrics.h"
#include "ofMain.h"

#define STRINGIFY(A) #A
ofxVolumetrics::ofxVolumetrics()
{
    quality = ofVec3f(1.0);
    threshold = 1.0/255.0;
    density = 1.0;
    volWidth = renderWidth = 0;
    volHeight = renderHeight = 0;
    volDepth = 0;
    bIsInitialized = false;

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &gl_max_tex_size);
    ofLogNotice() << "ofxVolumetrics: GL_MAX_TEXTURE_SIZE = " << gl_max_tex_size;

#ifdef OFX_VOLUMETRICS_EMULATE_3D_TEXTURE
    ofDisableArbTex();
    ofLogNotice() << "ofxVolumetrics: GL_TEXTURE_3D not supported in this build, using 3D texture emulation.";
#else
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &gl_max_tex_3d_depth);
    ofLogNotice() << "ofxVolumetrics: GL_MAX_3D_TEXTURE_SIZE = " << gl_max_tex_3d_depth;
#endif

    /* Front side */
    volNormals[0] = ofVec3f(0.0, 0.0, 1.0);
    volNormals[1] = ofVec3f(0.0, 0.0, 1.0);
    volNormals[2] = ofVec3f(0.0, 0.0, 1.0);
    volNormals[3] = ofVec3f(0.0, 0.0, 1.0);

    volVerts[0] = ofVec3f(1.0, 1.0, 1.0);
    volVerts[1] = ofVec3f(0.0, 1.0, 1.0);
    volVerts[2] = ofVec3f(0.0, 0.0, 1.0);
    volVerts[3] = ofVec3f(1.0, 0.0, 1.0);


    /* Right side */
    volNormals[4] = ofVec3f(1.0, 0.0, 0.0);
    volNormals[5] = ofVec3f(1.0, 0.0, 0.0);
    volNormals[6] = ofVec3f(1.0, 0.0, 0.0);
    volNormals[7] = ofVec3f(1.0, 0.0, 0.0);

    volVerts[4] = ofVec3f(1.0, 1.0, 1.0);
    volVerts[5] = ofVec3f(1.0, 0.0, 1.0);
    volVerts[6] = ofVec3f(1.0, 0.0, 0.0);
    volVerts[7] = ofVec3f(1.0, 1.0, 0.0);

    /* Top side */
    volNormals[8] = ofVec3f(0.0, 1.0, 0.0);
    volNormals[9] = ofVec3f(0.0, 1.0, 0.0);
    volNormals[10] = ofVec3f(0.0, 1.0, 0.0);
    volNormals[11] = ofVec3f(0.0, 1.0, 0.0);

    volVerts[8] = ofVec3f(1.0, 1.0, 1.0);
    volVerts[9] = ofVec3f(1.0, 1.0, 0.0);
    volVerts[10] = ofVec3f(0.0, 1.0, 0.0);
    volVerts[11] = ofVec3f(0.0, 1.0, 1.0);

    /* Left side */
    volNormals[12] = ofVec3f(-1.0, 0.0, 0.0);
    volNormals[13] = ofVec3f(-1.0, 0.0, 0.0);
    volNormals[14] = ofVec3f(-1.0, 0.0, 0.0);
    volNormals[15] = ofVec3f(-1.0, 0.0, 0.0);

    volVerts[12] = ofVec3f(0.0, 1.0, 1.0);
    volVerts[13] = ofVec3f(0.0, 1.0, 0.0);
    volVerts[14] = ofVec3f(0.0, 0.0, 0.0);
    volVerts[15] = ofVec3f(0.0, 0.0, 1.0);

    /* Bottom side */
    volNormals[16] = ofVec3f(0.0, -1.0, 0.0);
    volNormals[17] = ofVec3f(0.0, -1.0, 0.0);
    volNormals[18] = ofVec3f(0.0, -1.0, 0.0);
    volNormals[19] = ofVec3f(0.0, -1.0, 0.0);

    volVerts[16] = ofVec3f(0.0, 0.0, 0.0);
    volVerts[17] = ofVec3f(1.0, 0.0, 0.0);
    volVerts[18] = ofVec3f(1.0, 0.0, 1.0);
    volVerts[19] = ofVec3f(0.0, 0.0, 1.0);

    /* Back side */
    volNormals[20] = ofVec3f(0.0, 0.0, -1.0);
    volNormals[21] = ofVec3f(0.0, 0.0, -1.0);
    volNormals[22] = ofVec3f(0.0, 0.0, -1.0);
    volNormals[23] = ofVec3f(0.0, 0.0, -1.0);

    volVerts[20] = ofVec3f(1.0, 0.0, 0.0);
    volVerts[21] = ofVec3f(0.0, 0.0, 0.0);
    volVerts[22] = ofVec3f(0.0, 1.0, 0.0);
    volVerts[23] = ofVec3f(1.0, 1.0, 0.0);
}

ofxVolumetrics::~ofxVolumetrics()
{
    destroy();
}

void ofxVolumetrics::setup(int w, int h, int d, ofVec3f voxelSize, bool usePowerOfTwoTexSize)
{
    string vertexShader = 
#ifdef OFX_VOLUMETRICS_EMULATE_3D_TEXTURE
    #include "shaders/gles2/vert.glsl"
#else
    #include "shaders/gl/vert.glsl"
#endif

    string fragmentShader = 
#ifdef OFX_VOLUMETRICS_EMULATE_3D_TEXTURE
    #include "shaders/gles2/frag.glsl"
#else
    #include "shaders/gl/frag.glsl"
#endif

    voxelRatio = voxelSize;
    bIsPowerOfTwo = usePowerOfTwoTexSize;

    volumeShader.unload();
    volumeShader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
    volumeShader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    volumeShader.linkProgram();

    fboRender.allocate(w, h, GL_RGBA);

#ifdef OFX_VOLUMETRICS_EMULATE_3D_TEXTURE
// using emulated 3D textures

    // find out how many frames we can fit in x dimension
    unsigned int maxNumFramesX = gl_max_tex_size / w;
    ofLogVerbose() << "ofxVolumetrics: maximum number of frames at " << w << " pixels wide in the X dimension is " << maxNumFramesX;

    // find out how many frames we can fit in y dimension
    unsigned int maxNumFramesY = gl_max_tex_size / h;
    ofLogVerbose() << "ofxVolumetrics: maximum number of frames at " << h << " pixels tall in the Y dimension is " << maxNumFramesY;

    unsigned int maxNumFrames = maxNumFramesX * maxNumFramesY;
    ofLogVerbose() << "ofxVolumetrics: maximum emulated 3D texture size for requested width and height is (" << w << "," << h << "," << maxNumFrames <<")";

    if (d > maxNumFrames) {
        ofLogError() << "ofxVolumetrics: tried to allocate emulated 3d texture with dimensions (" << w << "," << h << "," << d << "), but maximum frames at " << w << "x" << h << " is " << maxNumFrames;
        std::exit(1);
    }

    numFramesX = min(maxNumFramesX, (unsigned int)d);
    ofLogVerbose() << "ofxVolumetrics: emulated texture will be " << numFramesX << " frames wide";

    numFramesY = d / numFramesX;
    if (d % numFramesX > 0) {
        numFramesY++;
    }

    ofLogVerbose() << "ofxVolumetrics: emulated texture will be " << numFramesY << " frames tall";

    volWidth = renderWidth = w;
    volHeight = renderHeight = h;
    volDepth = d;
    volTexWidth = w * numFramesX;
    volTexHeight = h * numFramesY;
    volTexDepth = 1;

    if(bIsPowerOfTwo){
        volTexWidth = ofNextPow2(volTexWidth);
        volTexHeight = ofNextPow2(volTexHeight);

        ofLogVerbose() << "ofxVolumetrics::setup(): Using power of two texture size.";
    }
    ofLogVerbose() << "ofxVolumetrics: Emulated 3D texutre size: " << volTexWidth << "x" << volTexHeight;
    volumeTexture.allocate(volTexWidth, volTexHeight, GL_RGBA);

#else 
// using real 3D textures
    volTexWidth = volWidth = renderWidth = w;
    volTexHeight = volHeight = renderHeight = h;
    volTexDepth = volDepth = d;

    if(bIsPowerOfTwo){
        volTexWidth = ofNextPow2(volTexWidth);
        volTexHeight = ofNextPow2(volTexHeight);
        volTexDepth = ofNextPow2(volTexDepth);

        ofLogVerbose() << "ofxVolumetrics::setup(): Using power of two texture size. Requested: " << w << "x" <<h<<"x"<<d<<". Actual: " << volTexWidth<<"x"<<volTexHeight<<"x"<<volTexDepth;
    }

    volumeTexture.allocate(volTexWidth, volTexHeight, volTexDepth, GL_RGBA);

    if(bIsPowerOfTwo){
        // if using cropped power of two, blank out the extra memory
        unsigned char * d;
        d = new unsigned char[volTexWidth*volTexHeight*volTexDepth*4];
        memset(d,0,volTexWidth*volTexHeight*volTexDepth*4);
        volumeTexture.loadData(d,volTexWidth, volTexHeight, volTexDepth, 0,0,0,GL_RGBA);

        // free the memory used to blank the texture
        delete [] d;
    }

#endif

    bIsInitialized = true;
}

void ofxVolumetrics::destroy()
{
    volumeShader.unload();
//    fboBackground.destroy();
//    fboRender.destroy();
    volumeTexture.clear();

    volWidth = renderWidth = 0;
    volHeight = renderHeight = 0;
    volDepth = 0;
    bIsInitialized = false;
}

void ofxVolumetrics::updateVolumeData(unsigned char * data, int w, int h, int d, int xOffset, int yOffset, int zOffset)
{
#ifdef OFX_VOLUMETRICS_EMULATE_3D_TEXTURE
    // upload each sub frame
    for (int z = zOffset; z < (zOffset + d); z++)
    {
        int x = (z % numFramesX) * volWidth + xOffset;
        int y = (z / numFramesX) * volHeight + yOffset;

        // ofLogVerbose() << "uploading " << w << "x" << h << " pixels to location " << x << "," << y;

        volumeTexture.loadData(data + w * h * z * 4, w, h, x, y, GL_RGBA);
    }
#else
    volumeTexture.loadData(data, w, h, d, xOffset, yOffset, zOffset, GL_RGBA);
#endif
}

void ofxVolumetrics::drawVolume(float x, float y, float z, float size, int zTexOffset)
{
    ofVec3f volumeSize = voxelRatio * ofVec3f(volWidth,volHeight,volDepth);
    float maxDim = max(max(volumeSize.x, volumeSize.y), volumeSize.z);
    volumeSize = volumeSize * size / maxDim;

    drawVolume(x, y, z, volumeSize.x, volumeSize.y, volumeSize.z, zTexOffset);
}

void ofxVolumetrics::drawVolume(float x, float y, float z, float w, float h, float d, int zTexOffset)
{
    updateRenderDimentions();

    ofVec3f cubeSize = ofVec3f(w, h, d);

    GLfloat modl[16], proj[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, modl);
    glGetFloatv(GL_PROJECTION_MATRIX, proj);
    GLint color[4];
    glGetIntegerv(GL_CURRENT_COLOR, color);

    ofVec3f scale,t;
    ofQuaternion a,b;
    ofMatrix4x4(modl).decompose(t, a, scale, b);

    GLint cull_mode;
    glGetIntegerv(GL_FRONT_FACE, &cull_mode);
    GLint cull_mode_fbo = (scale.x*scale.y*scale.z) > 0 ? GL_CCW : GL_CW;

    /* raycasting pass */
    fboRender.begin();
    volumeShader.begin();
    ofClear(0,0,0,0);

    //load matricies from outside the FBO
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(proj);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(modl);

    ofTranslate(x-cubeSize.x/2, y-cubeSize.y/2, z-cubeSize.z/2);
    ofScale(cubeSize.x,cubeSize.y,cubeSize.z);

    //pass variables to the shader

#ifdef OFX_VOLUMETRICS_EMULATE_3D_TEXTURE
    volumeShader.setUniform2f("frame_layout", (float)numFramesX, (float)numFramesY); // how the emulated 3d texture is layed out
    volumeShader.setUniformTexture("volume_tex", volumeTexture, 0); // volume texture reference
#else
    glActiveTexture(GL_TEXTURE1);
    volumeTexture.bind();
    volumeShader.setUniform1i("volume_tex", 1); // volume texture reference
    volumeTexture.unbind();
    glActiveTexture(GL_TEXTURE0);
#endif

    volumeShader.setUniform3f("vol_d", (float)volWidth, (float)volHeight, (float)volDepth); //dimensions of the volume
    volumeShader.setUniform3f("vol_tex_d", (float)volTexWidth, (float)volTexHeight, (float)volTexDepth); //dimensions of the volume texture

    volumeShader.setUniform1f("zoffset", zTexOffset); // used for animation so that we dont have to upload the entire volume every time
    volumeShader.setUniform1f("quality", quality.z); // 0 ... 1
    volumeShader.setUniform1f("density", density); // 0 ... 1
    volumeShader.setUniform1f("threshold", threshold);//(float)mouseX/(float)ofGetWidth());

    glFrontFace(cull_mode_fbo);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    drawRGBCube();
    glDisable(GL_CULL_FACE);
    glFrontFace(cull_mode);

    volumeShader.end();
    fboRender.end();

    ofPushView();

    glColor4iv(color);
    ofSetupScreenOrtho();//ofGetWidth(), ofGetHeight(),OF_ORIENTATION_DEFAULT,false,0,1000);
    fboRender.draw(0,0,ofGetWidth(),ofGetHeight());

    ofPopView();

}

void ofxVolumetrics::drawRGBCube()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, sizeof(ofVec3f), volVerts);
    glNormalPointer(GL_FLOAT, sizeof(ofVec3f), volNormals);
    glColorPointer(3,GL_FLOAT, sizeof(ofVec3f), volVerts);
    glTexCoordPointer(3, GL_FLOAT, sizeof(ofVec3f), volVerts);

    glDrawArrays(GL_QUADS, 0, 24);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void ofxVolumetrics::updateRenderDimentions()
{
    if((int)(ofGetWidth() * quality.x) != renderWidth)
    {
        renderWidth = ofGetWidth()*quality.x;
        renderHeight = ofGetHeight()*quality.x;
        fboRender.allocate(renderWidth, renderHeight, GL_RGBA);
    }
}

void ofxVolumetrics::setXyQuality(float q)
{
    quality.x = MAX(q,0.01);

    updateRenderDimentions();
}
void ofxVolumetrics::setZQuality(float q)
{
    quality.z = MAX(q,0.01);
}
void ofxVolumetrics::setThreshold(float t)
{
    threshold = ofClamp(t,0.0,1.0);
}
void ofxVolumetrics::setDensity(float d)
{
    density = MAX(d,0.0);
}
void ofxVolumetrics::setRenderSettings(float xyQuality, float zQuality, float dens, float thresh)
{
    setXyQuality(xyQuality);
    setZQuality(zQuality);
    setDensity(dens);
    setThreshold(thresh);
}

void ofxVolumetrics::setVolumeTextureFilterMode(GLint filterMode) {
    if(filterMode != GL_NEAREST && filterMode != GL_LINEAR) return;

    volumeTexture.bind();
#ifdef OFX_VOLUMETRICS_EMULATE_3D_TEXTURE
    GLint target = GL_TEXTURE_2D;
#else
    GLint target = GL_TEXTURE_3D;
#endif
    glTexParameteri(target GL_TEXTURE_MIN_FILTER, filterMode);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filterMode);
    volumeTexture.unbind();

}

bool ofxVolumetrics::isInitialized()
{
    return bIsInitialized;
}
int ofxVolumetrics::getVolumeWidth()
{
    return volWidth;
}
int ofxVolumetrics::getVolumeHeight()
{
    return volHeight;
}
int ofxVolumetrics::getVolumeDepth()
{
    return volDepth;
}
int ofxVolumetrics::getRenderWidth()
{
    return renderWidth;
}
int ofxVolumetrics::getRenderHeight()
{
    return renderHeight;
}
float ofxVolumetrics::getXyQuality()
{
    return quality.x;
}
float ofxVolumetrics::getZQuality()
{
    return quality.z;
}
float ofxVolumetrics::getThreshold()
{
    return threshold;
}
float ofxVolumetrics::getDensity()
{
    return density;
}
ofFbo & ofxVolumetrics::getFboReference(){
    return fboRender;
}
