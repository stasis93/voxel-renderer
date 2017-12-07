#include "application.h"

#include <iostream>
#include <memory>
#include <chrono>

#ifndef _GLIBCXX_HAS_GTHREADS
#include <mingw.thread.h> // threads are still missing in MinGW GCC :(
#else
#include <thread>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include "constants.h"
#include "utils.h"
#include "random.h"
#include "heightmapprovider.h"
#include "settings.h"
#include "textureloader.h"
#include "drawcalltrack.h"


Application::Application()
    : m_camera(glm::vec3{0.0f, 100.0f, 0.0f},
               180.0f, -20.0f)
    , m_chunkManager(m_frustrum)
    , m_config(Settings::get())
{
    initGL();
    registerCallbacks();
    Random::init();
    HeightMapProvider::init(m_config.world().seed == 0 ? std::time(nullptr) : m_config.world().seed);

    std::unique_ptr<Shader> shader = std::make_unique<Shader>();
    shader->load(ShaderFiles::vertex_shader_chunk, ShaderFiles::fragment_shader_chunk);
    shader->use();
    shader->setInt("blockTexture", 0);

    m_chunkManager.setShader(std::move(shader));
    m_chunkManager.setBlockTexture(TextureLoader::loadTexture(m_config.world().blockTextureName));

    shader = std::make_unique<Shader>();
    shader->load(ShaderFiles::vertex_shader_skybox, ShaderFiles::fragment_shader_skybox);
    shader->use();
    shader->setInt("skybox", 0);

    m_skyBox.initialize();
    m_skyBox.setShader(std::move(shader));
    m_skyBox.setTexture(TextureLoader::loadCubeMap(m_config.skyboxNames(), false));

    m_fpsCounter.setContext(m_window);
    m_info.setContext(m_window);

    Utils::glCheckError();

    m_player.bindCamera(&m_camera);
    m_player.setWorldData(&m_chunkManager);
    m_player.setPosition({0, 100, 0});
    m_player.setDirection({0, 0, 1});
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

    glfwSwapInterval(m_config.rendering().vsync ? 1 : 0);
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwGetCursorPos(m_window, &m_xprev, &m_yprev);

    glClearColor(0.1f, 0.2f, 0.4f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
//    glCullFace(GL_FRONT);
}

void Application::registerCallbacks()
{
    glfwSetKeyCallback(m_window, [](GLFWwindow* w, int key, int /*scancode*/, int action, int /*mods*/)
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
    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* w, int width, int height)
    {
        Application* This = (Application*)glfwGetWindowUserPointer(w);
        This->resizeCallback(width, height);
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
    m_player.rotate(dx / 10, -dy / 10);
}

void Application::resizeCallback(int width, int height)
{
    updateProjectionMatrix(width, height);
    glViewport(0, 0, width, height);
}

void Application::run()
{
    m_timerMain.restart();
    m_timerFpsCap.restart();

    while (!glfwWindowShouldClose(m_window))
    {
        m_timeSlice += m_timerMain.getElapsedSecs();
        m_timerMain.restart();

        if(m_config.rendering().fpsLimit > 0 &&
           m_config.rendering().vsync == false)
        {
            double dt = m_timerFpsCap.getElapsedSecs();
            double wait = 1.0 / m_config.rendering().fpsLimit - dt;

            if (wait > 0)
                std::this_thread::sleep_for(std::chrono::duration<double>(wait));

            m_timerFpsCap.restart(); // must not count sleep time
        }

        pollEvents();

        for (; m_timeSlice > Consts::FIXED_TIMESTEP;
               m_timeSlice -= Consts::FIXED_TIMESTEP)
            update(Consts::FIXED_TIMESTEP);

        render();
        m_fpsCounter.tick();
    }
}

void Application::pollEvents()
{
    glfwPollEvents();
}

void Application::update(float dt_sec)
{
    handleKbd(dt_sec);
    const glm::vec3& camDir = m_camera.getDirection();
    const glm::vec3& playerPos = m_player.getPosition();
    m_chunkManager.update({(int)playerPos.x, (int)playerPos.y, (int)playerPos.z});
    m_player.update();

    m_info.setPositionInfo(playerPos.x, playerPos.y, playerPos.z);
    m_info.setViewDirectionInfo(camDir.x, camDir.y, camDir.z);
}

void Application::handleKbd(float dt)
{
    float dist = dt * 10;
    if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT))
        dist *= 10.0f;
    else if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL))
        dist /= 10.0f;

    if (glfwGetKey(m_window, GLFW_KEY_W))
        m_player.move(Player::Forward, dist);
    else if (glfwGetKey(m_window, GLFW_KEY_S))
        m_player.move(Player::Back, dist);
    if (glfwGetKey(m_window, GLFW_KEY_A))
        m_player.move(Player::Left, dist);
    else if (glfwGetKey(m_window, GLFW_KEY_D))
        m_player.move(Player::Right, dist);
}

void Application::render()
{
#ifdef TRACK_GL_DRAWCALLS
    m_info.setDrawCallCount(DrawCallTrack::getDrawCallCount());
    m_info.setTriangleCount(DrawCallTrack::getTriangleCount());
    DrawCallTrack::resetCount();
#endif
    updateFrustrum();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_view = m_camera.getViewMatrix();

    m_chunkManager.render(m_proj * m_view);
    m_skyBox.render(m_proj * glm::mat4(glm::mat3(m_view)));
    m_fpsCounter.render();
    m_info.render();

    glfwSwapBuffers(m_window);
    Utils::glCheckError();
}

void Application::updateFrustrum()
{
    int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
    m_frustrum.updatePlanes(m_camera.getPosition(), m_camera.getDirection(),
                            m_camera.getUp(), m_camera.getRight(),
                            glm::radians((float)m_config.rendering().fovy), w / (float)h, 0.1f, 1000.0f);
}

void Application::updateProjectionMatrix(int width, int height)
{
    if (width > 0 && height > 0)
        m_proj = glm::perspective(glm::radians((float)m_config.rendering().fovy), width / (float)height, 0.1f, 1000.0f);
}

Application::~Application()
{
    cleanUp();
}
