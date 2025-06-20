#include "readPPM_binary.h"
#include <fstream>  
#include <sstream>  
#include <iostream>  
#include "globals.h"  


bool readPPM_P6(const std::string& path, std::vector<Pixel>& pixels, int& width, int& height) {
    std::ifstream file(path, std::ios::binary);  
    if (!file.is_open()) return false;  

    std::string magic;
    file >> magic;  
    if (magic != "P6") return false;  

    file.ignore(); 

    std::string line;
    do {
        std::getline(file, line);  
    } while (line[0] == '#');  

    
    std::istringstream dim(line);
    dim >> width >> height;

    int maxVal;
    file >> maxVal; 
    file.ignore();  

    size_t pixelCount = width * height;  
    pixels.resize(pixelCount);  

    
    for (size_t i = 0; i < pixelCount; ++i) {
        unsigned char rgb[3];  
        if (!file.read(reinterpret_cast<char*>(rgb), 3)) return false;  
        pixels[i] = {rgb[0], rgb[1], rgb[2]};  
    }

    return true; 
}
