#ifndef APPLICATION_H_INCLUDED
#define APPLICATION_H_INCLUDED

#include <glm/mat4x4.hpp>

#include "chunkmanager.h"
#include "graphics/camera.h"
#include "graphics/frustrum.h"
#include "graphics/shader.h"
#include "graphics/skybox.h"
#include "objects/player.h"
#include "ui/crosshair.h"
#include "ui/text/debuginfo.h"
#include "ui/text/fpscounter.h"
#include "utils/timer.h"

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

    Player                  m_player;
    Camera                  m_camera;
    Frustrum                m_frustrum;
    ChunkManager            m_chunkManager;
    Skybox                  m_skyBox;

    FPSCounter              m_fpsCounter;
    DebugInfo               m_info;
    Crosshair               m_crosshair;

    double                  m_xprev {0},
                            m_yprev {0};

    glm::mat4               m_proj,
                            m_view;

    Timer                   m_timerMain, m_timerFpsCap;
    double                  m_timeSlice {0};

    Settings&               m_config;
};

#endif // APPLICATION_H_INCLUDED
