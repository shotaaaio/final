#pragma once

#include <string>
#include <fstream>
#include "json-develop/single_include/nlohmann/json.hpp"
#include "Graphics/GraphicsManager.h"
using json = nlohmann::json;

template<typename T>
bool SaveSettings(const T& settings, const std::string& path)
{
    // �����̃t�@�C��������΍폜�i�㏑���ۑ��j
    std::ofstream ofs(path, std::ios::out | std::ios::trunc);  // �t�@�C�����㏑�����[�h�ŊJ��
    if (!ofs.is_open()) {
        std::cerr << "Error opening file for writing: " << path << std::endl;
        return false;
    }

    // �ݒ���e��JSON�Ƃ��ăV���A���C�Y���ď�������
    json j = settings;
    ofs << j.dump(4);  // �C���f���g�����ď�������
    std::cout << "Settings saved to " << path << std::endl;

    return true;
}

template<typename T>
bool LoadSettings(T& settings, const std::string& path)
{
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        std::cerr << "Error opening file for reading: " << path << std::endl;
        return false;
    }
    json j;
    ifs >> j;
    settings = j.get<T>();
    std::cout << "Settings loaded from " << path << std::endl;
    return true;
}
//===========================================================================

//               <---Glitch Shader--->

inline void to_json(json& j, const GlitchData& d)
{
    j = json{
        {"time", d.time},
        {"density", d.density},
        {"shift", d.shift},
        {"line_impact", d.line_impact},
        {"x_shifting", d.x_shifting},
        {"y_shifting", d.y_shifting},
        {"brightness", d.brightness},
        {"glitch_sampling_count", d.glitch_sampling_count},
        {"rect_noise_width", d.rect_noise_width},
        {"rect_noise_height", d.rect_noise_height},
        {"rect_noise_strength", d.rect_noise_strength},
      
    };
}

inline void from_json(const json& j, GlitchData& d)
{
    d.time = j.value("time", d.time);
    d.density = j.value("density", d.density);
    d.shift = j.value("shift", d.shift);
    d.line_impact = j.value("line_impact", d.line_impact);
    d.x_shifting = j.value("x_shifting", d.x_shifting);
    d.y_shifting = j.value("y_shifting", d.y_shifting);
    d.brightness = j.value("brightness", d.brightness);
    d.glitch_sampling_count = j.value("glitch_sampling_count", d.glitch_sampling_count);
    d.rect_noise_width = j.value("rect_noise_width", d.rect_noise_width);
    d.rect_noise_height = j.value("rect_noise_height", d.rect_noise_height);
    d.rect_noise_strength = j.value("rect_noise_strength", d.rect_noise_strength);
 
}