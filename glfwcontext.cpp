#include "glfwcontext.h"
#include <GLFW/glfw3.h>
#include <stdexcept>

GlfwContext::GlfwContext(std::string_view windowTitle, int windowWidth, int windowHeight) {
    if (!glfwInit()) {
        throw std::runtime_error("glfwInit failed");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(windowWidth, windowHeight, windowTitle.data(), nullptr, nullptr);
    if (!m_window) {
        throw std::runtime_error("glfwCreateWindow failed");
    }
}

GlfwContext::~GlfwContext() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

GLFWwindow* GlfwContext::getWindow() {
    return m_window;
}
