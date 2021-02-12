#version 120

in vec4	position;

out vec3 v_texcoord;
out vec3 v_cameraPosition;

uniform mat4 modelViewMatrixInverse;

void main() 
{
    gl_Position = gl_ModelViewProjectionMatrix * position;

    // as our vertex coordinates are normalized (0..1) 
    // we can reuse them as 3d texture coordinates
    v_texcoord = position.xyz;
    
    cameraPosition = (modelViewMatrixInverse * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
}