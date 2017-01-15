#version 300 es

uniform sampler2D ColorBuffer;
uniform highp vec4 SunPos;
uniform highp mat4 vp;

in highp vec4 position;

out highp vec4 color;

void main(){
	highp vec4 SunScreenPos = vp * SunPos;
    if (distance(vec2((position.x-0.5)*1024.0, (position.y-0.5)*768.0), SunScreenPos.xy) < 10.0){
		color = vec4(1.0f, 1.0f, 1.0f, 0.0f);
	} else {
		color = texture(ColorBuffer, position.xy);
	}
	color = texture(ColorBuffer, position.xy);
}
