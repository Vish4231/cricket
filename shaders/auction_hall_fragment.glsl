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
uniform sampler2D roughnessMap;
uniform sampler2D metallicMap;
uniform sampler2D aoMap;

// Lighting
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform float ambientStrength;
uniform float diffuseStrength;

// Shadow mapping
uniform sampler2D shadowMap;
uniform float shadowBias;

// Auction hall specific
uniform float time;
uniform bool isAuctionActive;
uniform float auctionIntensity;
uniform vec3 hallAmbientColor;
uniform vec3 hallSpotlightColor;
uniform vec3 stageSpotlightPos;

void main()
{
    // Sample textures
    vec3 albedo = texture(diffuseMap, fs_in.TexCoords).rgb;
    float roughness = texture(roughnessMap, fs_in.TexCoords).r;
    float metallic = texture(metallicMap, fs_in.TexCoords).r;
    float ao = texture(aoMap, fs_in.TexCoords).r;
    
    // Normal mapping
    vec3 normal = normalize(fs_in.Normal);
    if (textureSize(normalMap, 0).x > 0) {
        vec3 tangent = normalize(fs_in.Tangent);
        vec3 bitangent = normalize(fs_in.Bitangent);
        mat3 TBN = mat3(tangent, bitangent, normal);
        normal = normalize(TBN * (texture(normalMap, fs_in.TexCoords).rgb * 2.0 - 1.0));
    }
    
    // Ambient lighting
    vec3 ambient = ambientStrength * hallAmbientColor;
    
    // Main directional lighting
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;
    
    // Stage spotlight effect
    vec3 stageLightDir = normalize(stageSpotlightPos - fs_in.FragPos);
    float stageDiff = max(dot(normal, stageLightDir), 0.0);
    float stageDistance = length(stageSpotlightPos - fs_in.FragPos);
    float stageAttenuation = 1.0 / (1.0 + 0.1 * stageDistance + 0.01 * stageDistance * stageDistance);
    vec3 stageLight = hallSpotlightColor * stageDiff * stageAttenuation * auctionIntensity;
    
    // Specular lighting
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = 0.5 * spec * lightColor;
    
    // Shadow calculation
    float shadow = 0.0;
    vec3 projCoords = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if (projCoords.z <= 1.0) {
        float closestDepth = texture(shadowMap, projCoords.xy).r;
        float currentDepth = projCoords.z;
        shadow = currentDepth - shadowBias > closestDepth ? 1.0 : 0.0;
    }
    
    // Auction atmosphere effects
    if (isAuctionActive) {
        // Pulsing effect on stage area
        float stagePulse = sin(time * 2.0) * 0.1 + 0.9;
        float distanceToStage = length(fs_in.FragPos - stageSpotlightPos);
        if (distanceToStage < 20.0) {
            stageLight *= stagePulse;
        }
        
        // Dynamic ambient lighting
        float ambientPulse = sin(time * 0.5) * 0.1 + 0.9;
        ambient *= ambientPulse;
        
        // Add excitement glow
        float excitement = sin(time * 3.0) * 0.05 + 0.95;
        albedo = mix(albedo, albedo * vec3(1.1, 1.05, 1.0), excitement * auctionIntensity);
    }
    
    // Combine lighting
    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular) + stageLight;
    vec3 color = lighting * albedo * ao;
    
    // Add professional hall atmosphere
    // Slight blue tint for professional look
    color = mix(color, color * vec3(0.95, 0.97, 1.0), 0.1);
    
    // Add subtle vignette effect
    float distanceFromCenter = length(fs_in.TexCoords - vec2(0.5));
    float vignette = 1.0 - distanceFromCenter * 0.3;
    color *= vignette;
    
    FragColor = vec4(color, 1.0);
} 