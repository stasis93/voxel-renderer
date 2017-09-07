#ifndef APPLICATION_H_INCLUDED
#define APPLICATION_H_INCLUDED

#include <SFML/Window.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include "shader.h"
#include "camera.h"
#include "superchunk.h"
#include "timer.h"

class Application
{
public:
            Application();


    void    run();

private:
    void    initGL();
    void    pollEvents();
    void    update(float dt_sec);
    void    render();

private:
    void    resetMousePos();
    void    handleKbd(float dt_sec);
    void    handleMouse();

private:
    sf::Window              m_window;
    bool                    m_running {false};

    std::unique_ptr<Shader> m_shader_chunk;
    Camera                  m_camera;

    glm::mat4               m_proj,
                            m_view;

    std::unique_ptr<Superchunk> m_chunk;

    Timer                   m_timer;
    float                   m_timeSlice {0};
};

#endif // APPLICATION_H_INCLUDED
