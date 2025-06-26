#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>
#include <map>

// Forward declarations
class Shader;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

struct Texture {
    GLuint id;
    std::string type;
    std::string path;
};

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    float opacity;
    bool hasDiffuseMap;
    bool hasNormalMap;
    bool hasSpecularMap;
    bool hasRoughnessMap;
    bool hasMetallicMap;
};

struct Animation {
    std::string name;
    float duration;
    std::vector<glm::mat4> keyframes;
    std::vector<float> keyframeTimes;
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    Material material;
    GLuint VAO, VBO, EBO;
    
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, 
         const std::vector<Texture>& textures, const Material& material);
    ~Mesh();
    
    void draw(Shader& shader);
    void setupMesh();
};

class Model {
public:
    Model();
    ~Model();
    
    // Loading
    bool loadFromFile(const std::string& filepath);
    bool loadFromMemory(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    
    // Rendering
    void draw(Shader& shader);
    void drawInstanced(Shader& shader, int instanceCount);
    
    // Transform
    void setTransform(const glm::mat4& transform);
    void setPosition(const glm::vec3& position);
    void setRotation(const glm::vec3& rotation);
    void setScale(const glm::vec3& scale);
    
    // Getters
    const glm::mat4& getTransform() const { return transform; }
    const glm::vec3& getPosition() const { return position; }
    const glm::vec3& getRotation() const { return rotation; }
    const glm::vec3& getScale() const { return scale; }
    
    // Bounding box
    glm::vec3 getMinBounds() const { return minBounds; }
    glm::vec3 getMaxBounds() const { return maxBounds; }
    glm::vec3 getCenter() const { return center; }
    float getRadius() const { return radius; }
    
    // Materials and textures
    void setMaterial(const Material& material);
    void setTexture(const std::string& type, GLuint textureId);
    const std::vector<Texture>& getTextures() const { return textures; }
    const Material& getMaterial() const { return material; }
    
    // Animation
    void setAnimationTime(float time);
    float getAnimationTime() const { return animationTime; }
    bool hasAnimation() const { return !animations.empty(); }
    
    // Animation support
    void addAnimation(const std::string& name, const Animation& animation);
    void playAnimation(const std::string& name, float time = 0.0f);
    void updateAnimation(float deltaTime);
    glm::mat4 getCurrentTransform() const;
    
    // Model properties
    const std::string& getName() const { return name; }
    const std::string& getPath() const { return path; }
    bool hasAnimation(const std::string& name) const;
    
    // Utility
    bool isValid() const { return !meshes.empty(); }
    size_t getMeshCount() const { return meshes.size(); }
    size_t getVertexCount() const { return totalVertices; }
    size_t getIndexCount() const { return totalIndices; }

private:
    std::vector<std::unique_ptr<Mesh>> meshes;
    std::vector<Texture> textures;
    Material material;
    
    // Transform
    glm::mat4 transform;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    
    // Bounding box
    glm::vec3 minBounds;
    glm::vec3 maxBounds;
    glm::vec3 center;
    float radius;
    
    // Animation
    float animationTime;
    std::vector<std::string> animations;
    
    // Animation support
    std::map<std::string, Animation> animationsMap;
    std::string currentAnimation;
    bool isAnimating;
    
    // Statistics
    size_t totalVertices;
    size_t totalIndices;
    
    // Model properties
    std::string name;
    std::string path;
    
    // Helper methods
    void processNode(void* node, void* scene);
    std::unique_ptr<Mesh> processMesh(void* mesh, void* scene);
    std::vector<Texture> loadMaterialTextures(void* material, const std::string& typeName);
    void calculateBoundingBox();
    void updateTransform();
    GLuint loadTexture(const std::string& path);
    GLuint loadTextureFromMemory(const unsigned char* data, int width, int height, int channels);
    void setupMesh();
    void createDefaultPlayerModel();
    void addDefaultAnimations();
    void cleanup();
    glm::mat4 interpolateKeyframes(const Animation& anim, float time) const;
}; 