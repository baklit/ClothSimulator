#version 300 es

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 uvCoord;

out vec3 fragNormal;
out vec2 uvPos;
out vec4 shadowPos;

uniform mat4 VP;
uniform mat4 depthMVP;
uniform vec3 modelPos;
uniform vec3 modelScale;

void main() {
	uvPos = uvCoord;
	fragNormal = vertexNormal;
	gl_Position = VP * vec4(modelPos + vertexPosition * modelScale, 1);
	shadowPos = depthMVP * vec4(modelPos + vertexPosition * modelScale, 1);
}
