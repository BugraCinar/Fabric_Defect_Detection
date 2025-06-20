#pragma once

#include <string>
#include <vector>
#include <filesystem>

struct Pixel {
    unsigned char r, g, b;
};

bool readPPM_P6(const std::string& path, std::vector<Pixel>& pixels, int& width, int& height);