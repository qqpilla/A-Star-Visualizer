#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "constants.h"
#include "grid.h"
#include "shader_program.h"

Grid grid;
bool is_setting_start = true;

void ErrorCallback(int errorCode, const char* message)
{
    std::cout << "ERROR: " << message << "\nERROR CODE: " << errorCode << std::endl;
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (key = GLFW_KEY_SPACE && action == GLFW_PRESS)
        is_setting_start = !is_setting_start;
}

void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double cursor_x, cursor_y;
        glfwGetCursorPos(window, &cursor_x, &cursor_y);
        cursor_y = W_Side - cursor_y;
    
        Cell *cell = grid.FindCellAround(cursor_x, cursor_y);

        if (is_setting_start)
            grid.SetStartCell(cell);
        else
            grid.SetDestinationCell(cell);
    }
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

    GLFWwindow *window = glfwCreateWindow(W_Side, W_Side, "A* Visualizer", NULL, NULL);
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

    glViewport(0, 0, W_Side, W_Side);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);


    grid.InitializeGrid();
    grid.InitializeMainCells();

    ShaderProgram vertical_grid_shader("../shaders/v_grid.vs", "../shaders/grid.fs");
    ShaderProgram horizontal_grid_shader("../shaders/h_grid.vs", "../shaders/grid.fs");
    ShaderProgram main_cells_shader("../shaders/main_cells.vs", "../shaders/main_cells.fs");

    while(!glfwWindowShouldClose(window))
    {
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(main_cells_shader.ID());
        grid.DrawStart();
        grid.DrawDestination();        

        glUseProgram(vertical_grid_shader.ID());
        grid.DrawSetOfGridLines();
        glUseProgram(horizontal_grid_shader.ID());
        grid.DrawSetOfGridLines();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}