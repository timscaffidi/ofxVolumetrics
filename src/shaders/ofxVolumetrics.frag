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

const vec3 minv = vec3(0.,0.,0.);
const vec3 maxv = vec3(1.,1.,1.);

void main()
{
    vec4 col_acc = vec4(0,0,0,0);
    vec3 vec = clamp(vec3(gl_TexCoord[0]), minv, maxv); // starting position of the ray
    vec3 zOffsetVec = vec3(0.0,0.0,zoffset/vol_d.z);
    vec3 dir = clamp(vec3(texture2DRect(backface, (((pos.xy / pos.w)+1.) / 2.)*bg_d)), minv,maxv) - vec; //direction = backface - starting position

    if(length(dir) > 0.) {
        float steps = floor(length(vol_d * dir) * quality);
        float stepsize = length(dir.xyz) / steps;
        vec3 delta_dir = normalize(dir) * stepsize;
        vec4 color_sample;
        float aScale =  density * (1.0/quality);

        //raycast
        for(int i = 0; i < int(steps); i++)
        {
            color_sample = texture3D(volume_tex, vec + zOffsetVec);
            if(color_sample.a > threshold) {
                float oneMinusAlpha = 1. - col_acc.a;
                color_sample.a *= aScale;
                col_acc.rgb = col_acc.rgb * col_acc.a + color_sample.rgb * color_sample.a * oneMinusAlpha;
                col_acc.a += color_sample.a * oneMinusAlpha;
                col_acc.rgb/=col_acc.a;
                if(col_acc.a >= 1.0) {
                    break; // terminate if opacity > 1
                }
            }
            vec += delta_dir;
        }
    }
    // export the rendered color
    gl_FragColor = col_acc;

}
