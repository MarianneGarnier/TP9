#version 330

out vec4 outBuffer;

uniform sampler2D depthmap;

void main() {
  outBuffer = texelFetch(depthmap,ivec2(gl_FragCoord.xy),0);
}
