#extension GL_ARB_texture_rectangle : enable
varying vec3 cameraPosition;

uniform sampler3D volume_tex;
uniform vec3 vol_d;
uniform vec2 bg_d;
uniform float zoffset;
uniform float quality;
uniform float threshold;
uniform float density;
uniform bool enable_lighting;

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

    vec3 minv = vec3(0.)+1./vol_d;
    vec3 maxv = vec3(1.)-1./vol_d;
    vec3 vec;
    vec3 vold = (maxv-minv)*vol_d;
    float vol_l = length(vold);

    vec4 col_acc = vec4(0,0,0,0);
    vec3 zOffsetVec = vec3(0.0,0.0,zoffset/vold.z);
    vec3 backPos = gl_TexCoord[0].xyz;//*maxv+minv;
    vec3 lookVec = normalize(backPos - cameraPosition);
    
    vec3 light_pos = vec3(0.75, 0.1, .05);
    const vec4 light_color = vec4(1.0);
    const float light_intensity = 5.0;


    Ray eye = Ray( cameraPosition, lookVec);
    BoundingBox box = BoundingBox(minv, maxv);

    float tnear, tfar;
    IntersectBox(eye, box, tnear, tfar);
    if (tnear < 0.15) tnear = 0.15;
    if(tnear > tfar) discard;

    vec3 rayStart = eye.Origin + eye.Dir * tnear;
    vec3 rayStop = eye.Origin + eye.Dir * tfar;

    vec3 dir = rayStop - rayStart; // starting position of the ray
    vec = rayStart;
    float dl = length(dir);
    if(dl == clamp(dl,0.,vol_l)) {
        float steps = floor(length(vold * dir) * quality);
        vec3 delta_dir = dir/steps;
        vec4 color_sample;
        float aScale =  density/quality;

        //raycast
        for(int i = 0; i < int(steps); i++)
        {
            color_sample = texture3D(volume_tex, vec + zOffsetVec);
            if(color_sample.a > threshold) {
                
                if(enable_lighting) {
                    vec4 light = light_intensity*light_color/vec4(distance(vec,light_pos));
                    vec3 light_vec = vec;
                    int light_steps = int(steps)/2;
                    vec3 light_step = (light_pos - vec)/float(light_steps);
                    for(int l=0;l<light_steps;l++) {
                        vec4 light_sample = texture3D(volume_tex, light_vec + zOffsetVec);
                        if(light_sample.a > threshold){
                            light *= vec4(1.)-light_sample * aScale*0.5;
                        }
                        light_vec += light_step;
                        if(light.a <= 0.0 || light_vec != clamp(light_vec,minv,maxv)) break;
                    }
                    color_sample.rgb *= light.a;
                }

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
    gl_FragColor = col_acc;//vec4(abs(rayStop-rayStart),1.);

}
