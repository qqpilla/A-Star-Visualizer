#pragma once
#include <string>
#include "glad/glad.h"

class ShaderProgram
{
private:
    unsigned int program;

    void CreateShaderProgram(const char *vertex_source, const char *fragment_source);
    unsigned int CompileShader(GLuint type, const char *source);

public:
    ShaderProgram(std::string vertex_shader_path, std::string fragment_shader_path);
    unsigned int ID() const;
};