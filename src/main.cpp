#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "constants.h"
#include "grid.h"
#include "shader_program.h"

void ErrorCallback(int errorCode, const char* message)
{
    std::cout << "ERROR: " << message << "\nERROR CODE: " << errorCode << std::endl;
}

void TrySetWindowShouldClose(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

unsigned int SetUpGrid()
{
    Grid grid;
    // 0.0f, y and 1.0f, y for all horizontal lines and
    // x, 0.0f and x, 1.0f for all vertical lines
    int count = (G_Resolution_Side - 1) * 4 * 2;
    float coords[count];

    for (int i = 0; i < G_Resolution_Side - 1; i++)
    {
        float normalized_horizontal = (grid.HorizontalGrid()[i] / W_Height) * 2.0f - 1.0f;
        float normalized_vertical = (grid.VerticalGrid()[i] / W_Width) * 2.0f - 1.0f;

        coords[8 * i] = -1.0f;
        coords[8 * i + 1] = normalized_horizontal;
        coords[8 * i + 2] = 1.0f;
        coords[8 * i + 3] = normalized_horizontal;

        coords[8 * i + 4] = normalized_vertical;
        coords[8 * i + 5] = -1.0f;
        coords[8 * i + 6] = normalized_vertical;
        coords[8 * i + 7] = 1.0f;
    }

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coords), coords, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    return VAO;
}

int main()
{
    glfwSetErrorCallback(ErrorCallback);

    if (!glfwInit())
    {
        std::cout << "ERROR: Glfw initialization failed" << std::endl;
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow *window = glfwCreateWindow(W_Width, W_Height, "A* Visualizer", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "ERROR: Window creation failed" << std::endl;
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "ERROR: Glad initialization failed" << std::endl;
        glfwTerminate();
        return 1;
    }

    glViewport(0, 0, W_Width, W_Height);

    unsigned int grid_vao = SetUpGrid();
    ShaderProgram grid_shader("../shaders/grid.vs", "../shaders/grid.fs");

    while(!glfwWindowShouldClose(window))
    {
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        glBindVertexArray(grid_vao);
        glUseProgram(grid_shader.ID());
        glDrawArrays(GL_LINES, 0, (G_Resolution_Side - 1) * 4);


        glfwSwapBuffers(window);
        glfwPollEvents();

        TrySetWindowShouldClose(window);
    }

    glfwTerminate();
}