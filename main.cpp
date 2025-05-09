#include "Window.h"
#include "Shader.h"
Window* win = new Window("vulkan test");
int main() {
   
    GLFWwindow* currentWindow = win->GetCurrentWindow();

    while (!glfwWindowShouldClose(currentWindow)) {
        glfwPollEvents();
    }



    return 0;
}