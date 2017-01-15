#version 300 es

layout(location = 0) in vec3 vertexPosition;

uniform mat4 depthMVP;
uniform vec3 modelPosition;
uniform vec3 modelScale;


void main(){
 gl_Position =  depthMVP * vec4(modelPosition + vertexPosition * modelScale,1);
}
