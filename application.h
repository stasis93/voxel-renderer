#ifndef APPLICATION_H_INCLUDED
#define APPLICATION_H_INCLUDED

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include "shader.h"
#include "camera.h"
#include "superchunk.h"
#include "timer.h"
#include "frustrum.h"

class GLFWwindow;

class Application
{
public:
            Application();
           ~Application();


    void    run();

private:
    void initGL();
    void registerCallbacks();

    void update(float dt = 1/60.f);
    void render();
    void pollEvents();

    void onError(const char* msg);
    void cleanUp();

    void keyCallback(int key, int action);
    void cursorPosCallback(double x, double y);

private:
    void    handleKbd(float dt_sec);

private:
    GLFWwindow*             m_window {nullptr};

    Shader                  m_shader_chunk;
    std::unique_ptr<ChunkManager> m_chunkManager;
    Frustrum                m_frustrum;

    Camera                  m_camera;
    double                  m_xprev {0},
                            m_yprev {0};

    glm::mat4               m_proj,
                            m_view;

    Timer                   m_timer;
    float                   m_timeSlice {0};
};

#endif // APPLICATION_H_INCLUDED
