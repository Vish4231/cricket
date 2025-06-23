#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
    vec3 Bitangent;
    vec4 FragPosLightSpace;
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

// Weather effects
uniform bool isRaining;
uniform float rainIntensity;
uniform float fogDensity;
uniform vec3 fogColor;

// Shadow mapping
uniform sampler2D shadowMap;
uniform float shadowBias;

// Field properties
uniform vec3 fieldColor;
uniform float grassHeight;
uniform float pitchWear;

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
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse lighting
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;
    
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
    
    // Rain effect
    if (isRaining) {
        float rainNoise = fract(sin(dot(fs_in.FragPos.xz, vec2(12.9898, 78.233))) * 43758.5453);
        float rainMask = step(0.95, rainNoise) * rainIntensity;
        albedo = mix(albedo, vec3(0.2, 0.3, 0.4), rainMask * 0.3);
        roughness = mix(roughness, 0.9, rainMask * 0.5);
    }
    
    // Grass effect
    float grassNoise = fract(sin(dot(fs_in.TexCoords * 50.0, vec2(12.9898, 78.233))) * 43758.5453);
    float grassMask = step(0.7, grassNoise) * grassHeight;
    albedo = mix(albedo, albedo * 0.8, grassMask);
    
    // Pitch wear effect
    albedo = mix(albedo, albedo * 0.6, pitchWear);
    
    // Combine lighting
    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);
    vec3 color = lighting * albedo * ao;
    
    // Fog effect
    float distance = length(fs_in.FragPos - viewPos);
    float fogFactor = 1.0 - exp(-fogDensity * distance);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    color = mix(color, fogColor, fogFactor);
    
    FragColor = vec4(color, 1.0);
} 