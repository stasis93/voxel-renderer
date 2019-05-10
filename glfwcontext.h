#ifndef GLFWCONTEXT_H
#define GLFWCONTEXT_H

#include <string_view>
#include "utils/noncopyable.h"

class GLFWwindow;

class GlfwContext : public NonCopyable
{
public:
    GlfwContext(std::string_view windowTitle, int windowWidth, int windowHeight);
    ~GlfwContext();
    GLFWwindow* getWindow();

private:
    GLFWwindow* m_window;
};

#endif // GLFWCONTEXT_H
