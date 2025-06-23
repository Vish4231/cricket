#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <unordered_map>

class Shader {
public:
    Shader();
    ~Shader();
    
    // Shader creation
    bool loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    bool loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath);
    bool loadFromStrings(const std::string& vertexSource, const std::string& fragmentSource);
    bool loadFromStrings(const std::string& vertexSource, const std::string& fragmentSource, const std::string& geometrySource);
    
    // Shader usage
    void use();
    void unuse();
    GLuint getProgramId() const { return programId; }
    
    // Uniform setters
    void setBool(const std::string& name, bool value);
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setVec2(const std::string& name, const glm::vec2& value);
    void setVec3(const std::string& name, const glm::vec3& value);
    void setVec4(const std::string& name, const glm::vec4& value);
    void setMat3(const std::string& name, const glm::mat3& value);
    void setMat4(const std::string& name, const glm::mat4& value);
    
    // Array uniforms
    void setIntArray(const std::string& name, const int* values, int count);
    void setFloatArray(const std::string& name, const float* values, int count);
    void setVec3Array(const std::string& name, const glm::vec3* values, int count);
    void setMat4Array(const std::string& name, const glm::mat4* values, int count);
    
    // Utility
    bool isValid() const { return programId != 0; }
    std::string getLastError() const { return lastError; }
    void printUniforms() const;

private:
    GLuint programId;
    std::unordered_map<std::string, GLint> uniformLocations;
    std::string lastError;
    
    // Helper methods
    bool compileShader(GLuint& shaderId, GLenum shaderType, const std::string& source);
    bool linkProgram();
    void checkCompileErrors(GLuint shader, const std::string& type);
    GLint getUniformLocation(const std::string& name);
    std::string readFile(const std::string& filepath);
}; 