#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
    vec3 Bitangent;
    vec4 FragPosLightSpace;
    vec3 WorldPos;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

// Team representative specific uniforms
uniform float time;
uniform bool isBidding;
uniform bool isActive;
uniform vec3 teamColor;
uniform float bidIntensity;
uniform float animationTime;

void main()
{
    vec3 worldPos = aPos;
    
    // Bidding animation - subtle movement when bidding
    if (isBidding) {
        float bidPulse = sin(time * 8.0) * 0.02;
        worldPos.y += bidPulse;
        
        // Slight forward lean when bidding
        float lean = sin(time * 4.0) * 0.01;
        worldPos.z += lean;
    }
    
    // Active team animation - gentle breathing effect
    if (isActive) {
        float breathing = sin(time * 2.0) * 0.005;
        worldPos.y += breathing;
    }
    
    // Idle animation - subtle swaying
    float idleSway = sin(time * 1.5) * 0.003;
    worldPos.x += idleSway;
    
    vs_out.FragPos = vec3(model * vec4(worldPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    vs_out.Tangent = mat3(model) * aTangent;
    vs_out.Bitangent = mat3(model) * aBitangent;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    vs_out.WorldPos = worldPos;
    
    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
} 