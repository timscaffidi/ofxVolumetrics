
varying vec4 pos;
void main()
{
	// Transforming The Vertex
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	pos = gl_Position;//(gl_Vertex*gl_ModelViewProjectionMatrix);
	//gl_Color

	// Passing The Texture Coordinate Of Texture Unit 0 To The Fragment Shader
    gl_TexCoord[0] = gl_MultiTexCoord0;
}
