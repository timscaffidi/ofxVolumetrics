#extension GL_ARB_texture_rectangle : enable
varying vec3 cameraPosition;
varying vec4 pos;
uniform sampler2DRect frontface;
uniform sampler3D volume_tex;
uniform vec3 vol_d;
uniform vec2 bg_d;
uniform float zoffset;
uniform float quality;
uniform float threshold;
uniform float density;

void main()
{
    vec3 minv = 1./vol_d;
    vec3 maxv = vec3(1.)-2./vol_d;
    vec3 vec;

    vec4 col_acc = vec4(0,0,0,0);
    vec3 zOffsetVec = vec3(0.0,0.0,zoffset/vol_d.z);
    vec3 backPos = gl_TexCoord[0].xyz*maxv+minv;
    vec3 lookVec = normalize(backPos - cameraPosition);
    vec4 frontTexSample = texture2DRect(frontface, (((pos.xy / pos.w)+1.) / 2.)*bg_d);
    if(frontTexSample.a < 0.1) {
        vec = cameraPosition + lookVec*0.15;
    }
    else {
        vec = cameraPosition + lookVec * max(abs(length((frontTexSample.xyz*maxv+minv) - cameraPosition)),0.15);
    }

    vec3 dir = clamp(backPos,minv,maxv) - clamp(vec,minv,maxv); // starting position of the ray

    if(dir!= vec3(0.,0.,0.)) {
        float steps = floor(length(vol_d * dir) * quality);
        vec3 delta_dir = dir/steps;
        vec4 color_sample;
        float aScale =  density/quality;

        //raycast
        for(int i = 0; i < int(steps); i++)
        {
            color_sample = texture3D(volume_tex, vec + zOffsetVec);
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

}
