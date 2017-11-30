#ifndef APPLICATION_H_INCLUDED
#define APPLICATION_H_INCLUDED

#include <glm/mat4x4.hpp>

#include "shader.h"
#include "camera.h"
#include "chunkmanager.h"
#include "timer.h"
#include "frustrum.h"
#include "Skybox.h"
#include "fpscounter.h"
#include "debuginfo.h"


class Settings;
class GLFWwindow;

class Application
{
public:
        Application();
       ~Application();

    void run();

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
    void resizeCallback(int width, int height);

    void updateFrustrum();
    void updateProjectionMatrix(int width, int height);

    void handleKbd(float dt_sec);

private:
    GLFWwindow*             m_window {nullptr};

    Camera                  m_camera;
    Frustrum                m_frustrum;
    ChunkManager            m_chunkManager;
    Skybox                  m_skyBox;

    FPSCounter              m_fpsCounter;
    DebugInfo               m_info;

    double                  m_xprev {0},
                            m_yprev {0};

    glm::mat4               m_proj,
                            m_view;

    Timer                   m_timerMain, m_timerFpsCap;
    double                  m_timeSlice {0};

    Settings&               m_config;
};

#endif // APPLICATION_H_INCLUDED
