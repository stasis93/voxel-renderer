#include "application.h"

#include <iostream>
#include <memory>
#include <chrono>

#ifndef _GLIBCXX_HAS_GTHREADS
#include <mingw.thread.h> // threads are still missing in MinGW GCC :(
#else
#include <thread>
#endif

#include <GLFW/glfw3.h>
#include "3rdparty/stb_image.h"
#include "glad/glad.h"

#include "utils/constants.h"
#include "utils/utils.h"
#include "utils/random.h"
#include "terrain/heightmapprovider.h"
#include "settings.h"
#include "graphics//textureloader.h"
#include "utils/drawcalltrack.h"
#include "utils/resourcemanager.h"


Application::Application(GLFWwindow* window)
    : m_window(window)
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
    ResourceManager::shaders().insert("chunk", std::move(shader));

    shader = std::make_unique<Shader>();
    shader->load(ShaderFiles::vertex_shader_skybox, ShaderFiles::fragment_shader_skybox);
    shader->use();
    shader->setInt("skybox", 0);
    ResourceManager::shaders().insert("skybox", std::move(shader));

    shader = std::make_unique<Shader>();
    shader->load("shaders/ui2d_vs.glsl", "shaders/ui2d_fs.glsl");
    shader->use();
    shader->setInt("tex", 0);
    ResourceManager::shaders().insert("crosshair", std::move(shader));

    shader = std::make_unique<Shader>("shaders/outline_vs.glsl", "shaders/outline_fs.glsl");
    ResourceManager::shaders().insert("outline", std::move(shader));

    ResourceManager::textures().insert("blocks", TextureLoader::loadTexture(m_config.world().blockTextureName));
    ResourceManager::textures().insert("skybox", TextureLoader::loadCubeMap(m_config.skyboxNames(), false));
    ResourceManager::textures().insert("crosshair", TextureLoader::loadTexture("textures/crosshair.png"));

    m_skyBox.initialize();
    m_skyBox.setShader(ResourceManager::shaders().get("skybox"));
    m_crosshair.initialize();
    m_crosshair.setShader(ResourceManager::shaders().get("crosshair"));
    m_chunkManager.setShader(ResourceManager::shaders().get("chunk"));
    m_chunkManager.setTexture(ResourceManager::textures().get("blocks"));
    m_skyBox.setTexture(ResourceManager::textures().get("skybox"));
    m_crosshair.setTexture(ResourceManager::textures().get("crosshair"));

    m_fpsCounter.setContext(m_window);
    m_info.setContext(m_window);

    Utils::glCheckError();

    m_player.setControl(std::make_unique<WalkingControl>());
    m_player.bindCamera(&m_camera);
    m_player.setWorldData(&m_chunkManager);
    m_player.setPosition({Random::intInRange(-100, 100), 100, Random::intInRange(-100, 100)});
    m_player.setDirection({0, 0, 1});

    resizeCallback(m_config.rendering().width, m_config.rendering().height);
}

void Application::initGL()
{
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
    exit(EXIT_FAILURE);
}

void Application::keyCallback(int key, int action)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window, GLFW_TRUE);
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        m_player.jump();
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
        m_player.setControl(std::make_unique<FlyingControl>());
    if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
        m_player.setControl(std::make_unique<WalkingControl>());
    if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS) {
        m_fpsCounter.toggleActive();
        m_info.toggleActive();
    }
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
    m_crosshair.setTransform(glm::ortho(0.0f, (float)width, 0.0f, (float)height));
    m_crosshair.setPosition(width / 2 - 8, height / 2 - 8);
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
    m_player.update(dt_sec);

    m_info.setPositionInfo(playerPos.x, playerPos.y, playerPos.z);
    m_info.setViewDirectionInfo(camDir.x, camDir.y, camDir.z);
}

void Application::handleKbd(float dt)
{
    float dist = dt * 10;
    if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT))
        dist *= 2.0f;
    else if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL))
        dist /= 10.0f;

    if (glfwGetKey(m_window, GLFW_KEY_W))
        m_player.move(AbstractPlayerControl::Forward, dist);
    else if (glfwGetKey(m_window, GLFW_KEY_S))
        m_player.move(AbstractPlayerControl::Back, dist);
    if (glfwGetKey(m_window, GLFW_KEY_A))
        m_player.move(AbstractPlayerControl::Left, dist);
    else if (glfwGetKey(m_window, GLFW_KEY_D))
        m_player.move(AbstractPlayerControl::Right, dist);
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

    m_chunkManager.setTransform(m_proj * m_view);
    m_skyBox.setTransform(m_proj * glm::mat4(glm::mat3(m_view)));
    Shader& outlineShader = ResourceManager::shaders().get("outline");
    outlineShader.use();
    outlineShader.setMat4("proj_view", &(m_proj * m_view)[0][0]);

    m_chunkManager.render();
    m_skyBox.render();
    m_fpsCounter.render();
    m_info.render();
    m_crosshair.render();
    m_player.render();

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

Application::~Application() {
    ResourceManager::textures().clear();
    ResourceManager::shaders().clear();
}
