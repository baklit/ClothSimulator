#version 300 es

layout(location = 0) in vec4 vertexPosition;

out vec4 position;

void main(){
	position = vertexPosition;
	gl_Position = vertexPosition*2.0f - vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
