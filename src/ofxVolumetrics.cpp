

#include "ofxVolumetrics.h"
#include "ofMain.h"
#define STRINGIFY( A) #A
ofxVolumetrics::ofxVolumetrics()
{
    quality = ofVec3f(1.0);
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

void ofxVolumetrics::setup(int w, int h, int d, ofVec3f voxelSize, bool usePowerOfTwoTexSize)
{
    string vertexShader = STRINGIFY(
                                    varying vec3 cameraPosition;
                                    void main()
                                    {
                                        gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
                                        gl_TexCoord[0] = gl_MultiTexCoord0; //poop
                                        cameraPosition = (gl_ModelViewMatrixInverse * vec4(0.,0.,0.,1.)).xyz;
                                    }); // END VERTEX SHADER STRINGIFY

    string fragmentShader = STRINGIFY((#extension GL_ARB_texture_rectangle : enable \n
                                        varying vec3 cameraPosition;

                                        uniform sampler3D volume_tex;
                                        uniform vec3 vol_d;
                                        uniform vec3 vol_d_pot;
                                        uniform vec2 bg_d;
                                        uniform float zoffset;
                                        uniform float quality;
                                        uniform float threshold;
                                        uniform float density;

                                        struct Ray {
                                            vec3 Origin;
                                            vec3 Dir;
                                        };

                                        struct BoundingBox {
                                            vec3 Min;
                                            vec3 Max;
                                        };

                                        bool IntersectBox(Ray r, BoundingBox box, out float t0, out float t1)
                                        {
                                            vec3 invR = 1.0 / r.Dir;
                                            vec3 tbot = invR * (box.Min-r.Origin);
                                            vec3 ttop = invR * (box.Max-r.Origin);
                                            vec3 tmin = min(ttop, tbot);
                                            vec3 tmax = max(ttop, tbot);
                                            vec2 t = max(tmin.xx, tmin.yz);
                                            t0 = max(t.x, t.y);
                                            t = min(tmax.xx, tmax.yz);
                                            t1 = min(t.x, t.y);
                                            return t0 <= t1;
                                        }

                                        void main()
                                        {

                                            vec3 minv = vec3(0.)+1./vol_d_pot;
                                            vec3 maxv = (vol_d/vol_d_pot)-1./vol_d_pot;
                                            vec3 vec;
                                            vec3 vold = (maxv-minv)*vol_d;
                                            float vol_l = length(vold);

                                            vec4 col_acc = vec4(0,0,0,0);
                                            vec3 zOffsetVec = vec3(0.0,0.0,zoffset/vol_d_pot.z);
                                            vec3 backPos = gl_TexCoord[0].xyz;
                                            vec3 lookVec = normalize(backPos - cameraPosition);


                                            Ray eye = Ray( cameraPosition, lookVec);
                                            BoundingBox box = BoundingBox(vec3(0.),vec3(1.));

                                            float tnear, tfar;
                                            IntersectBox(eye, box, tnear, tfar);
                                            if(tnear < 0.15) tnear = 0.15;
                                            if(tnear > tfar) discard;

                                            vec3 rayStart = (eye.Origin + eye.Dir * tnear)*(maxv-minv)+minv;//vol_d/vol_d_pot;
                                            vec3 rayStop = (eye.Origin + eye.Dir * tfar)*(maxv-minv)+minv;//vol_d/vol_d_pot;

                                            vec3 dir = rayStop - rayStart; // starting position of the ray

                                            vec = rayStart;
                                            float dl = length(dir);
                                            if(dl == clamp(dl,0.,vol_l)) {
                                                int steps = int(floor(length(vold * dir) * quality));
                                                vec3 delta_dir = dir/float(steps);
                                                vec4 color_sample;
                                                float aScale =  density/quality;

                                                float random = fract(sin(gl_FragCoord.x * 12.9898 + gl_FragCoord.y * 78.233) * 43758.5453);
                                                vec += delta_dir * random;

                                                //raycast
                                                for(int i = 0; i < steps; i++)
                                                {
                                                    vec3 vecz = vec + zOffsetVec;
                                                    if(vecz.z > maxv.z) vecz.z-=maxv.z;
                                                    color_sample = texture3D(volume_tex, vecz);
                                                    if(color_sample.a > threshold) {

                                                        float oneMinusAlpha = 1. - col_acc.a;
                                                        color_sample.a *= aScale;
                                                        col_acc.rgb = mix(col_acc.rgb, color_sample.rgb * color_sample.a, oneMinusAlpha);
                                                        col_acc.a += color_sample.a * oneMinusAlpha;
                                                        col_acc.rgb /= col_acc.a;
                                                        if(col_acc.a >= 1.0) {
                                                            break; // terminate if opacity > 1
                                                        }
                                                    }
                                                    vec += delta_dir;
                                                }
                                            }
                                            // export the rendered color
                                            gl_FragColor = col_acc;

                                        } )); // END FRAGMENT SHADER STRINGIFY

    // For whatever reason, the stringify macro takes the fragment shader code as 2 arguments,
    // wrapping it in () makes it compile, so trim them off
    fragmentShader = fragmentShader.substr(1,fragmentShader.size()-2);

    volumeShader.unload();
    volumeShader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
    volumeShader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    volumeShader.linkProgram();

    bIsPowerOfTwo = usePowerOfTwoTexSize;

    volWidthPOT = volWidth = renderWidth = w;
    volHeightPOT = volHeight = renderHeight = h;
    volDepthPOT = volDepth = d;

    if(bIsPowerOfTwo){
        volWidthPOT = ofNextPow2(w);
        volHeightPOT = ofNextPow2(h);
        volDepthPOT = ofNextPow2(d);

        ofLogVerbose() << "ofxVolumetrics::setup(): Using power of two texture size. Requested: " << w << "x" <<h<<"x"<<d<<". Actual: " << volWidthPOT<<"x"<<volHeightPOT<<"x"<<volDepthPOT<<".\n";
    }

    fboRender.allocate(w, h, GL_RGBA);
    volumeTexture.allocate(volWidthPOT, volHeightPOT, volDepthPOT, GL_RGBA);
    if(bIsPowerOfTwo){
        // if using cropped power of two, blank out the extra memory
        unsigned char * d;
        d = new unsigned char[volWidthPOT*volHeightPOT*volDepthPOT*4];
        memset(d,0,volWidthPOT*volHeightPOT*volDepthPOT*4);
        volumeTexture.loadData(d,volWidthPOT, volHeightPOT, volDepthPOT, 0,0,0,GL_RGBA);
    }
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
    glActiveTexture(GL_TEXTURE1);
    volumeTexture.bind();
    volumeShader.setUniform1i("volume_tex", 1); // volume texture reference
    volumeTexture.unbind();
    glActiveTexture(GL_TEXTURE0);

    volumeShader.setUniform3f("vol_d", (float)volWidth, (float)volHeight, (float)volDepth); //dimensions of the volume texture
    volumeShader.setUniform3f("vol_d_pot", (float)volWidthPOT, (float)volHeightPOT, (float)volDepthPOT); //dimensions of the volume texture power of two
    volumeShader.setUniform2f("bg_d", (float)renderWidth, (float)renderHeight); // dimensions of the background texture
    volumeShader.setUniform1f("zoffset",zTexOffset); // used for animation so that we dont have to upload the entire volume every time
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
    float oldQuality = quality.x;
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
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filterMode);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, filterMode);
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
