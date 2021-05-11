#version 120

varying vec3 cameraPosition;

void main() 
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

    // ofMesh / ofVboMesh only supports 2d texture coordinates
    // as our vertex coordinates are normalized (0..1) 
    // we can reuse them as 3d texture coordinates
    gl_TexCoord[0] = gl_Vertex;
    
    cameraPosition = (gl_ModelViewMatrixInverse * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
}