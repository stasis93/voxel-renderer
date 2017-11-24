#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <array>


class Settings
{
public:
    using SkyboxNames = std::array<std::string, 6>;

    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;

    static Settings& get();
    void parse();

    struct World
    {
        int seed;
    };
    struct Rendering
    {
        int maxUpdatesPerFrame;
        int maxExtraUpdatesPerFrame;
        int loadRadius;
        int fpsLimit;
        bool vsync;
    };

    World& world()              {return m_world;}
    Rendering& rendering()      {return m_rendering;}
    SkyboxNames& skyboxNames()  {return m_skyboxTexturePaths;}

    void setDefaults();

private:
    Settings();

    bool parseRenderingParam(const std::string& name,
                             const std::string& value);
    bool parseWorldParam(const std::string& name,
                         const std::string& value);
    bool parseSkyboxParam(const std::string& name,
                          const std::string& value);

    int parseInt(const std::string& value,
                 int min, int max, int def);

    World           m_world;
    Rendering       m_rendering;
    SkyboxNames     m_skyboxTexturePaths;
};

#endif // SETTINGS_H
