#version 300 es

uniform sampler2D Texture;
uniform sampler2D shadowTexture;
uniform highp vec3 modelColor;
uniform highp vec3 sunDirection;

in highp vec3 fragNormal;
in highp vec2 uvPos;
in highp vec4 shadowPos;

out highp vec4 color;

highp float bias = 0.005f;

mediump vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

void main() {
	//color = clamp(vec3(1.0f, 1.0f, 1.0f) * (float(gl_FrontFacing)*2.0f - 1.0f) * dot(fragNormal, vec3(0.0f, 1.0f, 0.0f)) + vec3(0.2f, 0.2f, 0.2f), 0.1f, 1.0f);
	color = texture(Texture, vec2(1.0f, 1.0f) - uvPos) * vec4(modelColor * clamp(vec3(1.0f, 1.0f, 1.0f) * (float(gl_FrontFacing)*2.0f - 1.0f) * dot(fragNormal, sunDirection) + vec3(0.2f, 0.2f, 0.2f), 0.1f, 1.0f), 0.0f);
	//color = abs(fragNormal);
	//color = vec3(1.0f, 1.0f, 1.0f);
	//color = vec4(uvPos, texture(shadowTexture, uvPos).z, 0.0f);
	//color = texture(shadowTexture, uvPos);

	for(int n = 0; n<4; n++){
		for(int i = 0; i<4; i++){
			if (texture(shadowTexture, (shadowPos.xy/10.0f) + poissonDisk[i]/(14000.0f + 100.0f*float(n))).z < shadowPos.z - bias){
				color = color * vec4(0.95f, 0.95f, 0.95f, 1.0f);
			}
		}
	}
	
	//if (texture2D(shadowTexture, shadowPos.xy/10).z < shadowPos.z - bias){
		//color = color * vec4(0.5f, 0.5f, 0.5f, 1.0f);
	//}
}
