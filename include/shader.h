//
// Created by elina on 2024-07-22.
//

#ifndef PROJECT_5_SHADER_H
#define PROJECT_5_SHADER_H

#include <glm/glm.hpp>
#include <string>

class ShaderProgram{
public:
    explicit ShaderProgram(const char* vertexPath, const char* fragmentPath);
    void use() const;

    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setVec4(const std::string &name, float x, float y, float z, float w) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    void setBool(const std::string &name, bool value) const;
    unsigned int ProgramId(){return id_;};

private:
    unsigned int id_;

    void checkCompileErrors(unsigned int shader, std::string type);

};
#endif //PROJECT_5_SHADER_H
