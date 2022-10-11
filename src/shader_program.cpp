#include "shader_program.h"
#include <iostream>
#include <fstream>
#include <sstream>

ShaderProgram::ShaderProgram(std::string vertex_shader_path, std::string fragment_shader_path)
{
    std::ifstream v_shader_file;
    std::ifstream f_shader_file;
    std::ostringstream v_shader_source;
    std::ostringstream f_shader_source;

    v_shader_file.exceptions(std::ifstream::badbit);
    f_shader_file.exceptions(std::ifstream::badbit);

    try
    {
        v_shader_file.open(vertex_shader_path);
        v_shader_source << v_shader_file.rdbuf();
    }
    catch(const std::ifstream::failure& e)
    {
        std::cout << "ERROR: FAILED TO READ SHADER FILE: VERTEX SHADER" << std::endl;
    }

    try
    {
        f_shader_file.open(fragment_shader_path);
        f_shader_source << f_shader_file.rdbuf();
    }
    catch(const std::ifstream::failure& e)
    {
        std::cout << "ERROR: FAILED TO READ SHADER FILE: FRAGMENT SHADER" << std::endl;
    }

    CreateShaderProgram(v_shader_source.str().c_str(), f_shader_source.str().c_str());
}

void ShaderProgram::CreateShaderProgram(const char *vertex_source, const char *fragment_source)
{
    program = glCreateProgram();
    unsigned int vertex_shader = CompileShader(GL_VERTEX_SHADER, vertex_source);
    unsigned int fragment_shader = CompileShader(GL_FRAGMENT_SHADER, fragment_source);

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    int linking_result;
    glGetProgramiv(program, GL_LINK_STATUS, &linking_result);
    if (linking_result == GL_FALSE)
    {
        char info_log[512];
        glGetProgramInfoLog(program, 512, NULL, info_log);
        std::cout << "ERROR: FAILED TO LINK PROGRAM\n" << info_log << std::endl;
    }
}

unsigned int ShaderProgram::CompileShader(GLuint type, const char *source)
{
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int compilation_result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compilation_result);
    if (compilation_result == GL_FALSE)
    {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        std::cout << "ERROR: FAILED TO COMPILE SHADER: "
                  << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT")
                  << " SHADER\n" << info_log << std::endl;
    }

    return shader;
}

unsigned int ShaderProgram::ID() const
{
    return program;
}