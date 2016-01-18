//#version 120
//
//
//#extension GL_EXT_gpu_shader4 : enable
//#extension GL_ARB_texture_rectangle : enable
//#define GL_FRAGMENT_PRECISION_HIGH 1

uniform sampler2D tex;

void main (void) {

//    vec4 coord = texture2D(tex, gl_TexCoord[0].st);
//
//        gl_FragColor = vec4(coord.x*100.0,gl_TexCoord[0].s,gl_TexCoord[0].s,1.0);//gl_Color;
    gl_FragColor = texture2D(tex, gl_TexCoord[0].st) * gl_Color;
    
}