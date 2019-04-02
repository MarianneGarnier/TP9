#version 330

layout(location = 0) in vec3 position;

uniform sampler2D heightmap;
uniform sampler2D normalmap;

out vec2 texcoord;


void main() {
vec3 newPos= position;
float heigth= texture(heightmap,position.xy).x;
newPos=position*height;
//gl_Position = vec4(vertex,0,1);
gl_Position = vec4(newPos,1);

texcoord = position.xy*0.5+0.5;

}
/*
#version 330

// input attributes
layout(location = 0) in vec3 position;// position of the vertex in world space
layout(location = 1) in vec3 normal;  // normal of the vertex
layout(location = 2) in vec3 tangent; // tangent of the vertex
layout(location = 3) in vec2 coord;   // uv-coord of the vertex

uniform mat4 mdvMat; // modelview matrix (constant for all the vertices)
uniform mat4 projMat; // projection matrix (constant for all the vertices)
uniform mat3 normalMat; // normal matrix (constant for all the vertices)
uniform sampler2D heightmap;

// output variables that will be interpolated during rasterization (equivalent to varying)
out vec3 normalView;
out vec3 tangentView;
out vec3 eyeView;
out vec2 uvcoord;

void main() {
  uvcoord     = coord*10.0;
  vec3 newPos = position;
  newPos = newPos + normal*texture(heightmap,uvcoord).x*5;
  gl_Position = projMat*mdvMat*vec4(newPos,1.0);
  normalView  = normalize(normalMat*normal);
  tangentView = normalize(normalMat*tangent);
  eyeView     = normalize((mdvMat*vec4(newPos,1.0)).xyz);
}


*/
