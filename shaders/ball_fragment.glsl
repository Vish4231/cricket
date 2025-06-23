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
    vec3 BallCenter;
} fs_in;

// Material properties
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D roughnessMap;
uniform sampler2D metallicMap;
uniform sampler2D aoMap;

// Ball properties
uniform float spin;
uniform float seam;
uniform float bounceHeight;
uniform float trajectoryCurve;
uniform float time;

// Lighting
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

// Shadow mapping
uniform sampler2D shadowMap;
uniform float shadowBias;

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
    
    // Seam effect
    float seamMask = 0.0;
    if (abs(seam) > 0.01) {
        float seamLine = sin(fs_in.TexCoords.x * 6.28318);
        seamMask = smoothstep(0.95, 1.0, abs(seamLine)) * seam;
        albedo = mix(albedo, vec3(0.1, 0.05, 0.02), seamMask * 0.8); // Darker seam
        roughness = mix(roughness, 0.9, seamMask * 0.5); // Rougher seam
    }
    
    // Spin visualization
    float spinEffect = 0.0;
    if (abs(spin) > 0.01) {
        float spinAngle = spin * time;
        float spinMask = sin(fs_in.TexCoords.x * 6.28318 + spinAngle);
        spinEffect = smoothstep(0.8, 1.0, abs(spinMask)) * abs(spin) * 0.3;
        albedo = mix(albedo, vec3(0.8, 0.6, 0.4), spinEffect); // Lighter spin trails
    }
    
    // Bounce effect
    float bounceGlow = 0.0;
    if (bounceHeight > 0.01) {
        float bounce = bounceHeight * sin(time * 10.0) * exp(-time * 2.0);
        bounceGlow = abs(bounce) * 0.2;
        albedo = mix(albedo, vec3(1.0, 0.9, 0.7), bounceGlow); // Brighten on bounce
    }
    
    // Trajectory curve effect
    float curveGlow = 0.0;
    if (abs(trajectoryCurve) > 0.01) {
        float curve = trajectoryCurve * sin(time * 5.0);
        curveGlow = abs(curve) * 0.15;
        albedo = mix(albedo, vec3(0.9, 0.8, 0.6), curveGlow);
    }
    
    // Ambient lighting
    vec3 ambient = 0.3 * lightColor;
    
    // Diffuse lighting
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = 0.7 * diff * lightColor;
    
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
    
    // Fresnel effect for leather
    float fresnel = pow(1.0 - max(dot(normal, viewDir), 0.0), 3.0);
    vec3 fresnelColor = mix(vec3(0.8, 0.6, 0.4), vec3(1.0, 0.9, 0.7), fresnel);
    
    // Combine lighting
    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);
    vec3 color = lighting * albedo * ao;
    
    // Add fresnel effect
    color = mix(color, fresnelColor, fresnel * 0.3);
    
    // Add motion blur effect for high-speed balls
    float speed = length(fs_in.FragPos - fs_in.BallCenter);
    if (speed > 10.0) {
        float blur = smoothstep(10.0, 20.0, speed) * 0.2;
        color = mix(color, vec3(0.5), blur);
    }
    
    FragColor = vec4(color, 1.0);
} 