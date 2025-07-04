#include "Shader.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader() : programId(0) {}
Shader::~Shader() { if (programId) glDeleteProgram(programId); }

bool Shader::loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile(vertexPath), fShaderFile(fragmentPath);
    if (!vShaderFile.is_open() || !fShaderFile.is_open()) {
        std::cerr << "Shader file not found: " << vertexPath << " or " << fragmentPath << std::endl;
        return false;
    }
    std::stringstream vStream, fStream;
    vStream << vShaderFile.rdbuf();
    fStream << fShaderFile.rdbuf();
    vertexCode = vStream.str();
    fragmentCode = fStream.str();
    vShaderFile.close();
    fShaderFile.close();
    
    GLuint vertex, fragment;
    GLint success;
    GLchar infoLog[512];
    
    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    const char* vCode = vertexCode.c_str();
    glShaderSource(vertex, 1, &vCode, nullptr);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        std::cerr << "Vertex Shader compilation failed: " << infoLog << std::endl;
        return false;
    }
    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fCode = fragmentCode.c_str();
    glShaderSource(fragment, 1, &fCode, nullptr);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
        std::cerr << "Fragment Shader compilation failed: " << infoLog << std::endl;
        return false;
    }
    // Shader Program
    programId = glCreateProgram();
    glAttachShader(programId, vertex);
    glAttachShader(programId, fragment);
    glLinkProgram(programId);
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programId, 512, nullptr, infoLog);
        std::cerr << "Shader Program linking failed: " << infoLog << std::endl;
        return false;
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return true;
}

void Shader::use() { glUseProgram(programId); }
void Shader::unuse() { glUseProgram(0); }

void Shader::setMat4(const std::string& name, const glm::mat4& value) {
    glUniformMatrix4fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setVec3(const std::string& name, const glm::vec3& value) {
    glUniform3fv(glGetUniformLocation(programId, name.c_str()), 1, glm::value_ptr(value));
}
void Shader::setVec4(const std::string& name, const glm::vec4& value) {
    glUniform4fv(glGetUniformLocation(programId, name.c_str()), 1, glm::value_ptr(value));
}
void Shader::setInt(const std::string& name, int value) {
    glUniform1i(glGetUniformLocation(programId, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) {
    glUniform1f(glGetUniformLocation(programId, name.c_str()), value);
}
void Shader::setBool(const std::string& name, bool value) {
    glUniform1i(glGetUniformLocation(programId, name.c_str()), (int)value);
} 