#include "ofxVolumetrics.h"
#include "ofMain.h"

ofxVolumetrics::ofxVolumetrics()
{
    quality = 1.0;
    threshold = 1.0/255.0;
    density = 1.0;
    volWidth = renderWidth = 0;
    volHeight = renderHeight = 0;
    volDepth = 0;
    bIsInitialized = false;

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

void ofxVolumetrics::setup(int w, int h, int d, ofVec3f voxelSize)
{
    volumeShader.load("shaders/ofxVolumetrics");
    volWidth = renderWidth = w;
    volHeight = renderHeight = h;
    volDepth = d;
    fboBackground.allocate(w, h, GL_RGBA);
    fboRender.allocate(w, h, GL_RGBA);
    volumeTexture.allocate(w, h, d, GL_RGBA);
    voxelRatio = voxelSize;

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
    volumeTexture.loadData(data, w, h, d, xOffset, yOffset, zOffset, GL_RGBA);
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

    GLfloat modl[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, modl );
    GLint color[4];
    glGetIntegerv(GL_CURRENT_COLOR, color);

    /* render backface */
    fboBackground.begin();
    ofClear(0,0,0,0);

    //load matricies from outside the FBO
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(modl);

    ofTranslate(x-cubeSize.x/2, y-cubeSize.y/2, z-cubeSize.z/2);
    ofScale(cubeSize.x,cubeSize.y,cubeSize.z);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    drawRGBCube();
    glDisable(GL_CULL_FACE);
    fboBackground.end();

    /* raycasting pass */
    fboRender.begin();
    volumeShader.begin();
    ofClear(0,0,0,0);

    //load matricies from outside the FBO
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(modl);

    ofTranslate(x-cubeSize.x/2, y-cubeSize.y/2, z-cubeSize.z/2);
    ofScale(cubeSize.x,cubeSize.y,cubeSize.z);

    //pass variables to the shader
    volumeShader.setUniformTexture("backface", fboBackground.getTextureReference(), 0);

    glActiveTexture(GL_TEXTURE1);
    volumeTexture.bind();
    volumeShader.setUniform1i("volume_tex", 1); // volume texture reference
    volumeTexture.unbind();
    glActiveTexture(GL_TEXTURE0);

    volumeShader.setUniform3f("vol_d", (float)volWidth, (float)volHeight, (float)volDepth); //dimensions of the volume texture
    volumeShader.setUniform2f("bg_d", (float)renderWidth, (float)renderHeight); // dimensions of the background texture
    volumeShader.setUniform1f("zoffset",zTexOffset); // used for animation so that we dont have to upload the entire volume every time
    volumeShader.setUniform1f("quality", quality.z); // 0 ... 1
    volumeShader.setUniform1f("density", density); // 0 ... 1
    volumeShader.setUniform1f("threshold", threshold);//(float)mouseX/(float)ofGetWidth());

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    drawRGBCube();
    glDisable(GL_CULL_FACE);

    volumeShader.end();
    fboRender.end();

    glMatrixMode(GL_MODELVIEW);
    ofPushMatrix();
    glMatrixMode(GL_PROJECTION);
    ofPushMatrix();

    glColor4iv(color);
    ofSetupScreenOrtho();//ofGetWidth(), ofGetHeight(),OF_ORIENTATION_DEFAULT,false,0,1000);
    fboRender.draw(0,0,ofGetWidth(),ofGetHeight());

    glMatrixMode(GL_PROJECTION);
    ofPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    ofPopMatrix();

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
        fboBackground.allocate(renderWidth, renderHeight, GL_RGBA);
        fboRender.allocate(renderWidth, renderHeight, GL_RGBA);
    }
}

void ofxVolumetrics::setXyQuality(float q)
{
    float oldQuality = quality.x;
    quality.x = ofClamp(q,0.0,1.0);

    updateRenderDimentions();
}
void ofxVolumetrics::setZQuality(float q)
{
    quality.z = ofClamp(q,0.0,2.0);
}
void ofxVolumetrics::setThreshold(float t)
{
    threshold = ofClamp(t,0.0,1.0);
}
void ofxVolumetrics::setDensity(float d)
{
    density = ofClamp(d,0.0,10.0);
}
void ofxVolumetrics::setRenderSettings(float xyQuality, float zQuality, float dens, float thresh)
{
    setXyQuality(xyQuality);
    setZQuality(zQuality);
    setDensity(dens);
    setThreshold(thresh);
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
