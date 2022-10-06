#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define wHeight 800
#define wWidth 800

void ErrorCallback(int errorCode, const char* message)
{
    std::cout << "ERROR: " << message << "\nERROR CODE: " << errorCode << std::endl;
}

void ShouldWindowClose(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
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

    GLFWwindow *window = glfwCreateWindow(wWidth, wHeight, "A* Visualizer", NULL, NULL);
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

    glViewport(0, 0, wWidth, wHeight);

    while(!glfwWindowShouldClose(window))
    {
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();

        ShouldWindowClose(window);
    }
}