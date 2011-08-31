#extension GL_ARB_texture_rectangle : enable
varying vec4 pos;
uniform sampler2DRect backface;
uniform sampler3D volume_tex;
uniform vec3 vol_d;
uniform vec2 bg_d;
uniform float zoffset;
uniform float quality;
uniform float threshold;
uniform float density;

void main()
{

    vec3 start = vec3(gl_TexCoord[0]);//  // the start position of the ray is stored in the texturecoordinate
    vec4 back_position = texture2DRect(backface, (((pos.xy / pos.w)+1) / 2)*bg_d);//texture2DRect(backface, texc);
    vec3 dir = vec3(0,0,0);
    dir.x = back_position.x - start.x;
    dir.y = back_position.y - start.y;
    dir.z = back_position.z - start.z;
    float len = length(dir.xyz);
    float steps = len * length(vol_d.xyz) * quality;
    float stepsize = len / steps;
    vec3 delta_dir = normalize(dir) * stepsize;
    float delta_dir_len = length(delta_dir);
    vec3 vec = start;
    vec4 col_acc = vec4(0,0,0,0);
    float alpha_acc = 0;
    vec4 color_sample;
    float alpha_sample;
    float aScale =  density * (1.0/quality);

    //raycast
    for(int i = 0; i < steps; i++)
    {
        color_sample = texture3D(volume_tex,vec+vec3(0.0f,0.0f,zoffset/vol_d.z));
        if(color_sample.a > threshold) {
            color_sample.a *= aScale;
            col_acc.rgb = col_acc.rgb * col_acc.a + color_sample.rgb * color_sample.a * (1.0 - col_acc.a);
            col_acc.a += color_sample.a * (1 - col_acc.a);
            col_acc.rgb/=col_acc.a;
            if(col_acc.a >= 1.0f) {
                break; // terminate if opacity > 1
            }
        }
        vec += delta_dir;
    }

    // export the rendered color
	gl_FragColor = col_acc;

}
