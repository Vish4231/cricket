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

// Auction hall specific uniforms
uniform float time;
uniform bool isAuctionActive;
uniform float auctionIntensity;

void main()
{
    vec3 worldPos = aPos;
    
    // Add subtle animation to hall elements
    if (isAuctionActive) {
        // Gentle swaying effect for hanging elements
        float sway = sin(time * 0.5) * 0.01;
        worldPos.x += sway * aNormal.x;
        worldPos.z += sway * aNormal.z;
    }
    
    vs_out.FragPos = vec3(model * vec4(worldPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    vs_out.Tangent = mat3(model) * aTangent;
    vs_out.Bitangent = mat3(model) * aBitangent;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    vs_out.WorldPos = worldPos;
    
    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
} 