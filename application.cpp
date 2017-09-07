#include "application.h"
#include <glad/glad.h>
#include "constants.h"
#include <iostream>
#include "utils.h"
#include "random.h"
#include "perlin.h"


Application::Application()
    : m_window({Consts::WINDOW_WIDTH, Consts::WINDOW_HEIGHT},
               Consts::WINDOW_NAME,
               sf::Style::Default,
               sf::ContextSettings(24))
    , m_camera(glm::vec3{0.0f, 0.0f, 0.0f},
               0.0f, 0.0f)
{
    initGL();
    Random::init();

    m_window.setFramerateLimit(Consts::FRAMERATE_LIMIT);
    m_window.setMouseCursorGrabbed(true);
    m_window.setMouseCursorVisible(false);
    m_shader_chunk = std::make_unique<Shader>(ShaderFiles::vertex_shader_chunk,
                                        ShaderFiles::fragment_shader_chunk);
    m_chunk = std::make_unique<Superchunk>(*m_shader_chunk);

    PerlinNoiseGenerator gen;
    auto data1 = gen.generate(CX * SCX, // "height" (in 3D it's not height actually)
                              CZ * SCZ, // width
                              1.0f, (float)(SCY * CY - 1), 4, 4);

    auto data2 = gen.generate(CX * SCX, // "height" (in 3D it's not height actually)
                              CZ * SCZ, // width
                              -10.0f, 10.0f, 16, 8);
    PerlinData data;
    data.resize(data1.size());
    for (auto &row : data)
        row.resize(data1[0].size());

    for (auto y = 0u; y < data1.size(); y++)
    for (auto x = 0u; x < data1[0].size(); x++)
    {
        auto val = data1[y][x] + data2[y][x];
        data[y][x] = (val < 1.0f) ? 1.0f : (val > SCY * CY - 1) ? (SCY * CY - 1) : val;
    }

    for (auto x = 0; x < CX * SCX; x++)
    for (auto z = 0; z < CZ * SCZ; z++)
    for (auto y = 0; y < data[x][z]; y++)
    {
        //std::cout << x << " " << y << " " << z << std::endl;
        m_chunk->set(x, y, z, Random::intInRange(50, 150));
    }


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
}

void Application::handleKbd(float dt)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        m_camera.move(CameraMovement::FORWARD, dt);
    }
    else
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        m_camera.move(CameraMovement::BACK, dt);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        m_camera.move(CameraMovement::LEFT, dt);
    }
    else
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        m_camera.move(CameraMovement::RIGHT, dt);
    }
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

    m_proj = glm::perspective(glm::radians(60.0f), m_window.getSize().x / (float)m_window.getSize().y, 0.1f, 500.0f);
    m_view = m_camera.getViewMatrix();

    m_shader_chunk->use();
    glUniformMatrix4fv(glGetUniformLocation(m_shader_chunk->id(), "projection"), 1, GL_FALSE, glm::value_ptr(m_proj));
    glUniformMatrix4fv(glGetUniformLocation(m_shader_chunk->id(), "view"), 1, GL_FALSE, glm::value_ptr(m_view));

    //Chunk::trianglesDrawn = 0;
    m_chunk->render();
    static bool showed {false};
    if (!showed)
    {
        std::cout << "Triagle count: " << Chunk::trianglesAdded << std::endl;
        showed = true;
    }

    m_window.display();

    Utils::glCheckError();
}
