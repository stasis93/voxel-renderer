#include "application.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "constants.h"
#include <iostream>
#include "utils.h"
#include "random.h"
#include "heightmapprovider.h"
#include "Settings.h"


Application::Application()
    : m_camera(glm::vec3{0.0f, 100.0f, 0.0f},
               180.0f, -20.0f)
{
    initGL();
    registerCallbacks();
    Random::init();
    HeightMapProvider::init(std::time(nullptr));

    m_shader_chunk.load(ShaderFiles::vertex_shader_chunk,
                        ShaderFiles::fragment_shader_chunk);
    /* some chunk manager initialization */
    m_chunkManager = std::make_unique<ChunkManager>(m_shader_chunk);
    m_chunkManager->setFrustrum(m_frustrum);

    int w, h, ch;
    stbi_set_flip_vertically_on_load(1);
    auto data = stbi_load("textures/blocks.png", &w, &h, &ch, 4);

    if (!data)
        onError("stbi_load failed");

    GLuint blk_tex_id;
    glGenTextures(1, &blk_tex_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, blk_tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    //glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    m_shader_chunk.use();
    m_shader_chunk.setInt("blockTexture", 0);
    Utils::glCheckError();

    // Testing
    auto& s = Settings::get();
}

void Application::initGL()
{
    if (!glfwInit())
        onError("glfwInit failed");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(m_config.rendering().width,
                                m_config.rendering().height,
                                "Voxel world", nullptr, nullptr);
    int w, h;
    glfwGetWindowSize(m_window, &w, &h);
    updateProjectionMatrix(w, h);

    if (!m_window)
        onError("glfwCreateWindow failed");

    glfwSetWindowUserPointer(m_window, (void*)this);
    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        onError("gladLoadGLLoader failed");

    glfwSwapInterval(1);

    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwGetCursorPos(m_window, &m_xprev, &m_yprev);

    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void Application::registerCallbacks()
{
    glfwSetKeyCallback(m_window, [](GLFWwindow* w, int key, int scancode, int action, int mods)
    {
        Application* This = (Application*)glfwGetWindowUserPointer(w);
        This->keyCallback(key, action);
    });
    glfwSetErrorCallback([](int code, const char* msg)
    {
        std::cerr << msg << " | Error code: " << code << std::endl;
    });
    glfwSetCursorPosCallback(m_window, [](GLFWwindow* w, double xpos, double ypos)
    {
        Application* This = (Application*)glfwGetWindowUserPointer(w);
        This->cursorPosCallback(xpos, ypos);
    });
}

void Application::onError(const char* msg)
{
    std::cerr << msg << std::endl;
    cleanUp();
    exit(EXIT_FAILURE);
}

void Application::cleanUp()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Application::keyCallback(int key, int action)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window, GLFW_TRUE);
}

void Application::cursorPosCallback(double x, double y)
{
    double dx = x - m_xprev;
    double dy = y - m_yprev;
    m_xprev = x;
    m_yprev = y;
    m_camera.rotate(dx / 10, -dy / 10);
}

void Application::run()
{
    m_timer.restart();

    while (!glfwWindowShouldClose(m_window))
    {
        m_timeSlice += m_timer.getElapsedSecs();
        m_timer.restart();

        pollEvents();

        for (; m_timeSlice > Consts::FIXED_TIMESTEP;
               m_timeSlice -= Consts::FIXED_TIMESTEP)
            update(Consts::FIXED_TIMESTEP);
        render();
    }
}

void Application::pollEvents()
{
    int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
    glViewport(0, 0, w, h);
    glfwPollEvents();
}

void Application::update(float dt_sec)
{
    handleKbd(dt_sec);
    const glm::vec3 &camPos = m_camera.getPosition();
    m_chunkManager->update({(int)camPos.x, (int)camPos.y, (int)camPos.z});
}

void Application::handleKbd(float dt)
{
    float dist = dt * 10;
    if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT))
        dist *= 10.0f;

    if (glfwGetKey(m_window, GLFW_KEY_W))
        m_camera.move(Camera::Forward, dist);
    else if (glfwGetKey(m_window, GLFW_KEY_S))
        m_camera.move(Camera::Back, dist);
    if (glfwGetKey(m_window, GLFW_KEY_A))
        m_camera.move(Camera::Left, dist);
    else if (glfwGetKey(m_window, GLFW_KEY_D))
        m_camera.move(Camera::Right, dist);
}

void Application::render()
{
    glClearColor(0.1f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
    m_proj = glm::perspective(glm::radians(40.0f), w / (float)h, 0.1f, 500.0f);
    m_view = m_camera.getViewMatrix();


    m_frustrum.updatePlanes(m_camera.getPosition(), m_camera.getDirection(),
                            m_camera.getUp(), m_camera.getRight(),
                            glm::radians(40.0f), w / (float)h, 0.1f, 500.0f);

    auto proj_x_view = m_proj * m_view;
    m_chunkManager->render(proj_x_view);

    glfwSwapBuffers(m_window);
    Utils::glCheckError();
}

Application::~Application()
{
    cleanUp();
}
