#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <string>

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec4 color;
    float size;
    float life;
    float maxLife;
    float rotation;
    float rotationSpeed;
    bool active;
};

struct ParticleEmitter {
    glm::vec3 position;
    glm::vec3 direction;
    float spread;
    float rate;
    float duration;
    float elapsed;
    bool continuous;
    bool active;
    
    // Particle properties
    float minSpeed;
    float maxSpeed;
    float minSize;
    float maxSize;
    float minLife;
    float maxLife;
    glm::vec4 startColor;
    glm::vec4 endColor;
    float gravity;
    float drag;
};

enum class ParticleType {
    BALL_TRAIL,
    CELEBRATION,
    DUST,
    RAIN,
    SPARKLE,
    EXPLOSION,
    SMOKE,
    FIRE
};

class ParticleSystem {
public:
    ParticleSystem();
    ~ParticleSystem();
    
    // Initialization
    bool initialize(int maxParticles = 1000);
    void cleanup();
    
    // Emitter management
    void addEmitter(const ParticleEmitter& emitter);
    void removeEmitter(size_t index);
    void clearEmitters();
    
    // Particle effects
    void createBallTrail(const glm::vec3& start, const glm::vec3& end, float duration);
    void createCelebration(const glm::vec3& position, int count = 50);
    void createDustCloud(const glm::vec3& position, float radius);
    void createRain(float intensity, const glm::vec3& area);
    void createSparkle(const glm::vec3& position, int count = 20);
    void createExplosion(const glm::vec3& position, float power);
    void createSmoke(const glm::vec3& position, float duration);
    void createFire(const glm::vec3& position, float duration);
    
    // Update and render
    void update(float deltaTime);
    void render(const class Shader& shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
    
    // Configuration
    void setParticleType(ParticleType type);
    void setMaxParticles(int maxParticles);
    void setGravity(float gravity);
    void setWind(const glm::vec3& wind);
    
    // Getters
    int getActiveParticleCount() const { return activeParticles; }
    int getMaxParticleCount() const { return maxParticles; }
    bool isActive() const { return activeParticles > 0 || !emitters.empty(); }

private:
    std::vector<Particle> particles;
    std::vector<ParticleEmitter> emitters;
    std::unique_ptr<class Shader> particleShader;
    
    // OpenGL buffers
    GLuint VAO, VBO, instanceVBO;
    std::vector<glm::mat4> instanceMatrices;
    std::vector<glm::vec4> instanceColors;
    
    // Configuration
    ParticleType particleType;
    int maxParticles;
    int activeParticles;
    float gravity;
    glm::vec3 wind;
    
    // Helper methods
    void spawnParticle(const ParticleEmitter& emitter);
    void updateParticle(Particle& particle, float deltaTime);
    void killParticle(Particle& particle);
    void updateEmitters(float deltaTime);
    
    void setupBallTrailParticle(Particle& particle, const glm::vec3& position);
    void setupCelebrationParticle(Particle& particle, const glm::vec3& position);
    void setupDustParticle(Particle& particle, const glm::vec3& position);
    void setupRainParticle(Particle& particle, const glm::vec3& position);
    void setupSparkleParticle(Particle& particle, const glm::vec3& position);
    void setupExplosionParticle(Particle& particle, const glm::vec3& position, float power);
    void setupSmokeParticle(Particle& particle, const glm::vec3& position);
    void setupFireParticle(Particle& particle, const glm::vec3& position);
    
    void updateInstanceData();
    void renderParticles();
    
    // Utility
    glm::vec3 randomDirection(float spread);
    float randomFloat(float min, float max);
    glm::vec4 interpolateColor(const glm::vec4& start, const glm::vec4& end, float t);
}; 