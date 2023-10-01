#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D characterTexture;
uniform float time;
uniform float alphaRate;

void main() {
    vec4 characterColor = texture(characterTexture, UV);
    
    float alpha = abs(sin(time * alphaRate));
    
    characterColor.a *= alpha;
    
    FragColor = characterColor;
}
