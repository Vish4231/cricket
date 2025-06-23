#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
    vec3 Bitangent;
    vec4 FragPosLightSpace;
    vec3 WorldPos;
} fs_in;

// Material properties
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;
uniform sampler2D roughnessMap;
uniform sampler2D metallicMap;
uniform sampler2D aoMap;

// Team colors
uniform vec3 teamColor;
uniform bool useTeamColor;

// Lighting
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform float ambientStrength;
uniform float diffuseStrength;

// Shadow mapping
uniform sampler2D shadowMap;
uniform float shadowBias;

// Animation
uniform float animationTime;
uniform bool isMoving;
uniform bool isBatting;
uniform bool isBowling;

void main()
{
    // Sample textures
    vec3 albedo = texture(diffuseMap, fs_in.TexCoords).rgb;
    float specular = texture(specularMap, fs_in.TexCoords).r;
    float roughness = texture(roughnessMap, fs_in.TexCoords).r;
    float metallic = texture(metallicMap, fs_in.TexCoords).r;
    float ao = texture(aoMap, fs_in.TexCoords).r;
    
    // Apply team colors to uniform areas
    if (useTeamColor) {
        // Detect uniform areas (simplified - would need mask texture)
        float uniformMask = texture(diffuseMap, fs_in.TexCoords).a;
        albedo = mix(albedo, teamColor, uniformMask * 0.8);
    }
    
    // Normal mapping
    vec3 normal = normalize(fs_in.Normal);
    if (textureSize(normalMap, 0).x > 0) {
        vec3 tangent = normalize(fs_in.Tangent);
        vec3 bitangent = normalize(fs_in.Bitangent);
        mat3 TBN = mat3(tangent, bitangent, normal);
        normal = normalize(TBN * (texture(normalMap, fs_in.TexCoords).rgb * 2.0 - 1.0));
    }
    
    // Ambient lighting
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse lighting
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;
    
    // Specular lighting
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specularLight = specular * spec * lightColor;
    
    // Shadow calculation
    float shadow = 0.0;
    vec3 projCoords = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if (projCoords.z <= 1.0) {
        float closestDepth = texture(shadowMap, projCoords.xy).r;
        float currentDepth = projCoords.z;
        shadow = currentDepth - shadowBias > closestDepth ? 1.0 : 0.0;
    }
    
    // Animation effects
    float animationGlow = 0.0;
    if (isMoving) {
        animationGlow = 0.1 * sin(animationTime * 10.0);
    }
    if (isBatting) {
        animationGlow = 0.2 * sin(animationTime * 15.0);
    }
    if (isBowling) {
        animationGlow = 0.15 * sin(animationTime * 12.0);
    }
    
    // Combine lighting
    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specularLight);
    vec3 color = lighting * albedo * ao;
    
    // Add animation glow
    color += animationGlow * teamColor;
    
    // Fresnel effect for metallic parts (like helmets, pads)
    float fresnel = pow(1.0 - max(dot(normal, viewDir), 0.0), 5.0);
    color += fresnel * metallic * 0.3;
    
    FragColor = vec4(color, 1.0);
} 