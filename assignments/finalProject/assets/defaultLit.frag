#version 450
out vec4 FragColor;

in Surface {
    vec2 UV;
    vec3 WorldPosition;
    vec3 WorldNormal;
} fs_in;

uniform sampler2D _Texture;

struct Light {
    vec3 position;
    vec3 color;
};

uniform Light _Lights[5];
uniform int _numLights;

struct Material {
    float ambientK;
    float diffuseK;
    float specular;
    float shininess;
};

uniform Material _Material;

uniform vec3 _Camerapose;

void main() 
{
    vec3 normal = normalize(fs_in.WorldNormal);
    vec3 viewDir = normalize(_Camerapose - fs_in.WorldPosition);

    vec3 totalAmbient = vec3(0.0);
    vec3 totalDiffuse = vec3(0.0);
    vec3 totalSpecular = vec3(0.0);

    for (int i = 0; i < _numLights; i++) {
        vec3 lightDir = normalize(_Lights[i].position - fs_in.WorldPosition);

        // Diffuse
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = _Lights[i].color * _Material.diffuseK * diff;

        // Specular
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), _Material.shininess * _Material.specular);
        vec3 specular = _Lights[i].color * spec;

        // Ambient
        vec3 ambient = _Lights[i].color * _Material.ambientK;

        totalAmbient += ambient;
        totalDiffuse += diffuse;
        totalSpecular += specular;
    }

    vec3 finalColor = texture(_Texture, fs_in.UV).rgb * (totalAmbient + totalDiffuse + totalSpecular);
    FragColor = vec4(finalColor, 1.0);
}