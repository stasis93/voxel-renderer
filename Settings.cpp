#include "Settings.h"
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <climits>
#include <functional>

static char const* configFileName = "../../config.txt";

Settings::Settings()
{
    parse();
}

Settings& Settings::get()
{
    static Settings instance;
    return instance;
}

void Settings::setDefaults()
{
    m_world.seed = 777;

    m_rendering.maxUpdatesPerFrame = 10;
    m_rendering.maxExtraUpdatesPerFrame = 1;
    m_rendering.loadRadius = 20;
    m_rendering.fpsLimit = 60;
    m_rendering.vsync = true;

    m_skyboxTexturePaths = {
        "ame_greenhaze/greenhaze_rt.tga",
        "ame_greenhaze/greenhaze_lf.tga",
        "ame_greenhaze/greenhaze_up.tga",
        "ame_greenhaze/greenhaze_dn.tga",
        "ame_greenhaze/greenhaze_bk.tga",
        "ame_greenhaze/greenhaze_ft.tga"
    };
}

void Settings::parse()
{
    setDefaults();

    std::ifstream ifs{configFileName};
    std::string line;
    std::function<bool(const std::string&, const std::string&)> parseLineFunc {nullptr};

    if (!ifs.is_open())
    {
        std::cerr << "Can't open " << configFileName << std::endl;
        return;
    }
    while (std::getline(ifs, line))
    {
        if (line.empty() || line.at(0) == '#')
            continue;

        using std::placeholders::_1;
        using std::placeholders::_2;

        if (line == "[Skybox]")
        {
            parseLineFunc = std::bind(&Settings::parseSkyboxParam, this, _1, _2);
            continue;
        }
        else if (line == "[World]")
        {
            parseLineFunc = std::bind(&Settings::parseWorldParam, this, _1, _2);
            continue;
        }

        else if (line == "[Rendering]")
        {
            parseLineFunc = std::bind(&Settings::parseRenderingParam, this, _1, _2);
            continue;
        }

        int nameEnd;
        if ((nameEnd = line.find('=')) != std::string::npos)
        {
            std::string name = line.substr(0, nameEnd);
            std::string value = line.substr(nameEnd + 1);
            if (!parseLineFunc(name, value))
                std::cerr << "Unknown parameter: " << name << std::endl;
        }
    }
    ifs.close();
}

bool Settings::parseRenderingParam(const std::string& name, const std::string& value)
{
    bool ok = true;
    if (name == "max_updates_per_frame")
        m_rendering.maxUpdatesPerFrame = parseInt(value, 1, 100, m_rendering.maxUpdatesPerFrame);
    else if (name == "max_extra_updates_per_frame")
        m_rendering.maxExtraUpdatesPerFrame = parseInt(value, 1, 10, m_rendering.maxExtraUpdatesPerFrame);
    else if (name == "load_radius")
        m_rendering.loadRadius = parseInt(value, 10, 100, m_rendering.loadRadius);
    else if (name == "fps_limit")
        m_rendering.fpsLimit = parseInt(value, 30, 300, m_rendering.fpsLimit);
    else if (name == "vsync")
        m_rendering.vsync = parseInt(value, 0, 1, m_rendering.vsync);
    else
        ok = false;
    return ok;
}

bool Settings::parseWorldParam(const std::string& name, const std::string& value)
{
    bool ok = true;
    if (name == "seed")
        m_world.seed = parseInt(value, INT_MIN, INT_MAX, 0);
    else
        ok = false;
    return ok;
}

bool Settings::parseSkyboxParam(const std::string& name, const std::string& value)
{
    bool ok = true;
    if (name == "posx")
        m_skyboxTexturePaths[0] = value;
    else if (name == "negx")
        m_skyboxTexturePaths[1] = value;
    else if (name == "posy")
        m_skyboxTexturePaths[2] = value;
    else if (name == "negy")
        m_skyboxTexturePaths[3] = value;
    else if (name == "posz")
        m_skyboxTexturePaths[4] = value;
    else if (name == "negz")
        m_skyboxTexturePaths[5] = value;
    else
        ok = false;
    return ok;
}

int Settings::parseInt(const std::string& value, int min, int max, int def)
{
    int res;
    try
    {
        res = std::stoi(value);
        if (res < min) res = min;
        if (res > max) res = max;
    }
    catch (std::exception& e)
    {
        std::cerr << "Parse failed for value \"" << value << "\"" << std::endl;
        res = def;
    }
    return res;
}
