#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include <iostream>

int glRunGraphics()
{
    if (!glfwInit())
    {
        std::cerr << "GLFW initialization failed\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        800, 600, "Ronin Engine", nullptr, nullptr
    );

    if (!window)
    {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress)))
    {
        std::cerr << "GLAD initialization failed\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.10f, 0.12f, 0.18f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}