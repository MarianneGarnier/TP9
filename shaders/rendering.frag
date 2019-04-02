#version 330

out vec4 outBuffer;
uniform sampler2D heightmap;
uniform sampler2D normalmap;
in vec2 texcoord;





void main() {

vec3 color= texture(normalmap,texcoord).xyz;

outBuffer =vec4(n,value(texture(heightmap,texcoord)));

}
