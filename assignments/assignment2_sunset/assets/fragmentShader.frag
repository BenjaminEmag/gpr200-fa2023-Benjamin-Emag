#version 450
out vec4 FragColor;
in vec2 UV;
uniform vec2 aspectRatio;
uniform float sunSpeed;
uniform float Time;
uniform vec3 dayColorTop;
uniform vec3 dayColorBottom;
uniform vec3 nightColorTop;
uniform vec3 nightColorBottom;
uniform float sunSize;
uniform vec3 sunColor;
uniform vec3 buildingColor;

float circleSDF(vec2 p, float r){
    return length(p)-r;
}


void main(){   
    float t = mod(floor(UV.x * 16.0), 2.0);
    t = step(t, UV.y + 0.4 + (sin(UV.x) * 0.05) * 5.0);
    t = step(t, UV.x - 0.5);
    t = 1.0 - t;
    vec3 rightSideBuilding = vec3(t / 0.5);
   
    
    t = mod(floor(UV.x * 16.5), 2.0);
    t = step(t, UV.y + 0.5 + (cos(UV.x) * 0.03) * 7.0);
    t = step(t, 0.5 - UV.x);
    t = 1.0 - t;
    vec3 leftSideBuilding = vec3(t / 0.5);
    
    vec3 Buidings = mix(rightSideBuilding, leftSideBuilding,0.5);
    Buidings = mix(buildingColor,Buidings,0.5);

     vec2 UV = (2.0 * gl_FragCoord.xy - aspectRatio.xy) / aspectRatio.y;
    
    vec2 circlePos = vec2(0.0,sin(Time * sunSpeed) * (sunSize - 1.0) - (2.0 * sunSize));
    float d = circleSDF(UV - circlePos, sunSize);
    
    d = 1.0 - smoothstep(-0.05,0.05,d);
    
    vec3 sun = vec3(sunColor * d);
   
    vec3 daySkyColor = mix(dayColorTop,dayColorBottom, UV.y);
    vec3 nightSkyColor = mix(nightColorTop,nightColorBottom, UV.y);
   
    vec3 skyColor = mix(daySkyColor, nightSkyColor, -1.0 * circlePos.y);
    
    vec3 Final = mix(skyColor, vec3(1.0),sun);
    
    Final = mix(Final,Buidings,0.5);

    FragColor = vec4(Final, 1.0);
}
