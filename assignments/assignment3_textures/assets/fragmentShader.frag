#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _Texture;
uniform int noiseTexture;
uniform int brickTexture;

void main(){
	FragColor = texture(_Texture,UV);
}
