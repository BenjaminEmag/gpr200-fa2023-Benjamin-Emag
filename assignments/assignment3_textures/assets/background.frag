#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D background;
uniform sampler2D noiseTexture;
uniform float noiseRate;
uniform float time;
uniform float scrollSpeed;

void main() {
    vec2 scrolledUV = UV + vec2(time * scrollSpeed, 0.0);

    scrolledUV = fract(scrolledUV);

    vec4 backgroundSample = texture(background, scrolledUV);
    vec2 noiseOffset = vec2(UV.x * noiseRate + time, UV.y * noiseRate + time);
    vec4 noise = texture(noiseTexture, noiseOffset);

    vec4 finalColor = backgroundSample + noise;

    FragColor = finalColor;
}
