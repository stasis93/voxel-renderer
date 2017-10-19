#include "application.h"
#include <glad/glad.h>
#include "constants.h"
#include <iostream>
#include "utils.h"
#include "random.h"
#include "heightmapprovider.h"
#include <SFML/Graphics/Image.hpp>


Application::Application()
    : m_window({Consts::WINDOW_WIDTH, Consts::WINDOW_HEIGHT},
               Consts::WINDOW_NAME,
               sf::Style::Default,
               sf::ContextSettings(24))
    , m_camera(glm::vec3{0.0f, 100.0f, 0.0f},
               180.0f, -20.0f)
{
    initGL();
    Random::init();
    HeightMapProvider::init(std::time(nullptr));

    m_window.setFramerateLimit(Consts::FRAMERATE_LIMIT);
    m_window.setMouseCursorGrabbed(true);
    m_window.setMouseCursorVisible(false);
    m_shader_chunk = std::make_unique<Shader>(ShaderFiles::vertex_shader_chunk,
                                        ShaderFiles::fragment_shader_chunk);
    m_frustrum = std::make_unique<Frustrum>();


    /* some chunk manager initialization */
    m_chunkManager = std::make_unique<ChunkManager>(*m_shader_chunk);
    m_chunkManager->setFrustrum(*m_frustrum);

    sf::Image img;
    if (!img.loadFromFile("textures/blocks.png"))
        std::cout << "Can't load block textures!" << std::endl;
    img.flipVertically();

    GLuint blk_tex_id;
    glGenTextures(1, &blk_tex_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, blk_tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getSize().x, img.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
    //glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    m_shader_chunk->use();
    glUniform1i(glGetUniformLocation(m_shader_chunk->id(), "blockTexture"), 0);

    Utils::glCheckError();
}

void Application::initGL()
{
    gladLoadGL();
    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
}

void Application::run()
{
    m_running = true;
    resetMousePos();
    m_timer.restart();

    while (m_running)
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
    sf::Event ev;
    while (m_window.pollEvent(ev))
    {
        switch (ev.type)
        {
        case sf::Event::Closed:
            m_running = false;
            break;
        case sf::Event::KeyPressed:
            if (ev.key.code == sf::Keyboard::Escape)
                m_running = false;
            break;
        case sf::Event::Resized:
            glViewport(0, 0, ev.size.width, ev.size.height);
            break;
        default:
            break;
        }
    }
}

void Application::update(float dt_sec)
{
    handleKbd(dt_sec);
    handleMouse();
    const glm::vec3 &camPos = m_camera.getPosition();
    m_chunkManager->update({(int)camPos.x, (int)camPos.y, (int)camPos.z});
}

void Application::handleKbd(float dt)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
        dt *= 10.0f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        m_camera.move(CameraMovement::FORWARD, dt);
    else
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        m_camera.move(CameraMovement::BACK, dt);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        m_camera.move(CameraMovement::LEFT, dt);
    else
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        m_camera.move(CameraMovement::RIGHT, dt);
}

void Application::handleMouse()
{
    static int  mouse_y         {(int)Consts::WINDOW_HEIGHT/2},
                mouse_x         {(int)Consts::WINDOW_WIDTH/2},
                mouse_y_prev    {(int)Consts::WINDOW_HEIGHT/2},
                mouse_x_prev    {(int)Consts::WINDOW_WIDTH/2};

    if (!m_window.hasFocus())
        return;

    mouse_y = sf::Mouse::getPosition(m_window).y;
    mouse_x = sf::Mouse::getPosition(m_window).x;
    resetMousePos();

    m_camera.rotate(mouse_x - mouse_x_prev, mouse_y - mouse_y_prev);

    mouse_y_prev = sf::Mouse::getPosition(m_window).y;
    mouse_x_prev = sf::Mouse::getPosition(m_window).x;
}


void Application::resetMousePos()
{
    sf::Mouse::setPosition({(int)m_window.getSize().x / 2,
                           (int)m_window.getSize().y / 2},
                           m_window);
}


void Application::render()
{
    glClearColor(0.1f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_proj = glm::perspective(glm::radians(40.0f), m_window.getSize().x / (float)m_window.getSize().y, 0.1f, 500.0f);
    m_view = m_camera.getViewMatrix();
    auto proj_x_view = m_proj * m_view;

    m_frustrum->updatePlanes(m_camera.getPosition(), m_camera.getDirection(),
                            m_camera.getUp(), m_camera.getRight(),
                            glm::radians(40.0f), m_window.getSize().x / (float)m_window.getSize().y, 0.1f, 500.0f);
//    m_frustrum->updateMatrices(proj_x_view);
//    m_frustrum->render();


    m_shader_chunk->use();
    glUniformMatrix4fv(glGetUniformLocation(m_shader_chunk->id(), "projection"), 1, GL_FALSE, glm::value_ptr(m_proj));
    glUniformMatrix4fv(glGetUniformLocation(m_shader_chunk->id(), "view"), 1, GL_FALSE, glm::value_ptr(m_view));



    m_chunkManager->render(proj_x_view);

    m_window.display();
    Utils::glCheckError();
}
