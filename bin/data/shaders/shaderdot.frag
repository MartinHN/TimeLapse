#version 120


#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_texture_rectangle : enable
#define GL_FRAGMENT_PRECISION_HIGH 1

void main (void) {
    float a = 1-(2*length(gl_TexCoord[0].st-vec2(0.5,0.5)));
    gl_FragColor = vec4(gl_Color.x,gl_Color.y,gl_Color.z,gl_Color.w*a);


    
}