#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <iostream>

#include "../include/drawing_lib.h"
#include "../include/scene.h"


int main() {

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    DrawingLib drawingLib = DrawingLib();

    GLFWwindow* window = drawingLib.createWindow();
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    drawingLib.defineCallbackFunction(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    Scene scene = Scene();
    drawingLib.defineScene(scene);

    while (!glfwWindowShouldClose(window))
    {
        drawingLib.getWindowSize(window);
        drawingLib.drawScene(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

