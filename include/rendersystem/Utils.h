#pragma once

#include <iostream>
#include <functional>

namespace Utils {
    unsigned int TextureFromFile(const std::string& path,
        std::function<void(void)> textureSettingsCallback = []() {});
}