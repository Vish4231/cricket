#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <vector>

enum class EffectType {
    NONE,
    SLOW_MOTION,
    REPLAY_TRANSITION,
    CELEBRATION_FLASH,
    WICKET_HIGHLIGHT,
    BOUNDARY_GLOW,
    RAIN_OVERLAY,
    FOG_OVERLAY,
    BLUR,
    SHARPEN,
    BLOOM,
    MOTION_BLUR,
    DEPTH_OF_FIELD,
    COLOR_GRADING
};

struct EffectParameters {
    float intensity;
    float duration;
    float elapsed;
    bool active;
    glm::vec4 color;
    float blurAmount;
    float brightness;
    float contrast;
    float saturation;
    glm::vec3 tint;
};

class ScreenEffect {
public:
    ScreenEffect();
    ~ScreenEffect();
    
    // Initialization
    bool initialize(int width, int height);
    void cleanup();
    void resize(int width, int height);
    
    // Effect management
    void addEffect(EffectType type, float duration, const EffectParameters& params = {});
    void removeEffect(EffectType type);
    void clearEffects();
    
    // Predefined effects
    void addSlowMotionEffect(float duration, float speed = 0.5f);
    void addReplayTransitionEffect(float duration);
    void addCelebrationFlashEffect(float duration);
    void addWicketHighlightEffect(float duration);
    void addBoundaryGlowEffect(float duration, const glm::vec4& color);
    void addRainOverlayEffect(float duration, float intensity);
    void addFogOverlayEffect(float duration, float density);
    
    // Post-processing
    void beginRender();
    void endRender();
    void renderEffects();
    
    // Update
    void update(float deltaTime);
    
    // Configuration
    void setQuality(int quality); // 0=Low, 1=Medium, 2=High
    void enableEffect(EffectType type, bool enabled);
    void setGlobalIntensity(float intensity);
    
    // Getters
    bool hasActiveEffects() const { return !activeEffects.empty(); }
    GLuint getFinalTexture() const { return finalTexture; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    // Framebuffers and textures
    GLuint mainFramebuffer;
    GLuint mainTexture;
    GLuint depthBuffer;
    GLuint effectFramebuffer;
    GLuint effectTexture;
    GLuint finalFramebuffer;
    GLuint finalTexture;
    
    // Shaders
    std::unique_ptr<class Shader> postProcessShader;
    std::unique_ptr<class Shader> blurShader;
    std::unique_ptr<class Shader> bloomShader;
    std::unique_ptr<class Shader> colorGradingShader;
    
    // Effects
    struct ActiveEffect {
        EffectType type;
        EffectParameters params;
        float startTime;
    };
    
    std::vector<ActiveEffect> activeEffects;
    std::vector<bool> enabledEffects;
    
    // Screen quad
    GLuint screenVAO, screenVBO;
    
    // Configuration
    int width, height;
    int quality;
    float globalIntensity;
    
    // Helper methods
    void setupFramebuffers();
    void setupScreenQuad();
    void setupShaders();
    
    void renderEffect(const ActiveEffect& effect);
    void updateEffect(ActiveEffect& effect, float deltaTime);
    
    // Effect implementations
    void renderSlowMotionEffect(const EffectParameters& params);
    void renderReplayTransitionEffect(const EffectParameters& params);
    void renderCelebrationFlashEffect(const EffectParameters& params);
    void renderWicketHighlightEffect(const EffectParameters& params);
    void renderBoundaryGlowEffect(const EffectParameters& params);
    void renderRainOverlayEffect(const EffectParameters& params);
    void renderFogOverlayEffect(const EffectParameters& params);
    void renderBlurEffect(const EffectParameters& params);
    void renderBloomEffect(const EffectParameters& params);
    void renderColorGradingEffect(const EffectParameters& params);
    
    // Utility
    void bindFramebuffer(GLuint framebuffer);
    void unbindFramebuffer();
    void renderScreenQuad();
    glm::vec4 interpolateColor(const glm::vec4& start, const glm::vec4& end, float t);
    float easeInOut(float t);
    float easeOut(float t);
}; 