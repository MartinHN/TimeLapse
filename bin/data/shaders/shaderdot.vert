#version 120


#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_texture_rectangle : enable
#define GL_FRAGMENT_PRECISION_HIGH 1

void main() {
    
    gl_Position   = gl_ModelViewProjectionMatrix * gl_Vertex;
//    float size    = gl_Normal.x;
//    gl_PointSize  = size;
    gl_FrontColor = gl_Color;

}