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
    vec3 BallCenter;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

// Ball physics
uniform float spin;
uniform float seam;
uniform float bounceHeight;
uniform float trajectoryCurve;
uniform float time;

void main()
{
    // Apply ball physics effects
    vec3 worldPos = aPos;
    
    // Spin effect - rotate vertices around ball center
    if (abs(spin) > 0.01) {
        float spinAngle = spin * time;
        mat3 spinMatrix = mat3(
            cos(spinAngle), 0, sin(spinAngle),
            0, 1, 0,
            -sin(spinAngle), 0, cos(spinAngle)
        );
        worldPos = spinMatrix * aPos;
    }
    
    // Seam effect - slight deformation along seam
    if (abs(seam) > 0.01) {
        float seamDeform = seam * sin(aTexCoords.x * 6.28318) * 0.1;
        worldPos += aNormal * seamDeform;
    }
    
    // Bounce effect
    if (bounceHeight > 0.01) {
        float bounce = bounceHeight * sin(time * 10.0) * exp(-time * 2.0);
        worldPos.y += bounce;
    }
    
    // Trajectory curve effect
    if (abs(trajectoryCurve) > 0.01) {
        float curve = trajectoryCurve * sin(time * 5.0);
        worldPos.x += curve * 0.1;
    }
    
    vs_out.FragPos = vec3(model * vec4(worldPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    vs_out.Tangent = mat3(model) * aTangent;
    vs_out.Bitangent = mat3(model) * aBitangent;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    vs_out.WorldPos = worldPos;
    vs_out.BallCenter = vec3(model[3]); // Extract position from model matrix
    
    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
} 