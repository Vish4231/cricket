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

// Team colors and state
uniform vec3 teamColor;
uniform bool isBidding;
uniform bool isActive;
uniform float bidIntensity;
uniform float bidAmount;
uniform bool isWinningBidder;

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
uniform float time;
uniform float animationTime;

void main()
{
    // Sample textures
    vec3 albedo = texture(diffuseMap, fs_in.TexCoords).rgb;
    float specular = texture(specularMap, fs_in.TexCoords).r;
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
    
    // Apply team colors to uniform areas
    float uniformMask = texture(diffuseMap, fs_in.TexCoords).a;
    albedo = mix(albedo, teamColor, uniformMask * 0.8);
    
    // Bidding effects
    if (isBidding) {
        // Pulsing glow effect
        float bidPulse = sin(time * 8.0) * 0.3 + 0.7;
        vec3 bidGlow = teamColor * bidPulse * bidIntensity;
        albedo = mix(albedo, bidGlow, 0.3);
        
        // Highlight edges
        float edgeHighlight = 1.0 - max(dot(normal, normalize(viewPos - fs_in.FragPos)), 0.0);
        albedo += teamColor * edgeHighlight * 0.2;
    }
    
    // Winning bidder effect
    if (isWinningBidder) {
        // Golden glow for winning bidder
        vec3 goldenGlow = vec3(1.0, 0.8, 0.2) * (sin(time * 4.0) * 0.3 + 0.7);
        albedo = mix(albedo, goldenGlow, 0.4);
        
        // Sparkle effect
        float sparkle = sin(time * 12.0 + fs_in.TexCoords.x * 10.0) * 
                       sin(time * 8.0 + fs_in.TexCoords.y * 8.0);
        sparkle = max(0.0, sparkle);
        albedo += vec3(1.0, 1.0, 1.0) * sparkle * 0.3;
    }
    
    // Active team effect
    if (isActive) {
        // Subtle breathing glow
        float breathing = sin(time * 2.0) * 0.1 + 0.9;
        albedo *= breathing;
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
    
    // Fresnel effect for professional appearance
    float fresnel = pow(1.0 - max(dot(normal, viewDir), 0.0), 3.0);
    vec3 fresnelColor = mix(teamColor, vec3(1.0, 1.0, 1.0), 0.5);
    
    // Combine lighting
    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specularLight);
    vec3 color = lighting * albedo * ao;
    
    // Add fresnel effect
    color = mix(color, fresnelColor, fresnel * 0.3);
    
    // Professional appearance enhancement
    // Slight desaturation for business look
    float luminance = dot(color, vec3(0.299, 0.587, 0.114));
    color = mix(color, vec3(luminance), 0.1);
    
    // Add subtle rim lighting
    float rim = 1.0 - max(dot(normal, viewDir), 0.0);
    rim = pow(rim, 2.0);
    color += teamColor * rim * 0.2;
    
    FragColor = vec4(color, 1.0);
} 