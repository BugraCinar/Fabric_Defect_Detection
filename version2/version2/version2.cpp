#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <cmath>
#include <string>
#include <algorithm>
#include <set>

namespace fs = std::filesystem;


struct Pixel {
    int r, g, b;
};


constexpr int blockSize = 10;  //sabitlendiler
constexpr double blockThreshold = 1000.0; 
constexpr double defectRatioThreshold = 0.01; 



bool readPPM(const std::string& path, std::vector<Pixel>& pixels, int& width, int& height) {
    std::ifstream file(path);  
    std::string magic;
    file >> magic;  
    if (magic != "P3") return false;  

    std::string line;
    std::getline(file, line); 
    do {
        std::getline(file, line);  
    } while (!line.empty() && line[0] == '#');  

   
    std::istringstream dim(line);
    dim >> width >> height;

    int maxVal;
    file >> maxVal;  

   
    pixels.reserve(width * height);  
    int r, g, b;
    while (file >> r >> g >> b) {
        pixels.push_back({r, g, b});
    }

    return true;  
}


inline double blockDifference(const std::vector<Pixel>& ref, const std::vector<Pixel>& test, //inline eklendi
                       int x, int y, int w, int h, int imgW) {
    double total = 0; 
    int count = 0;  

    
    for (int dy = 0; dy < h; ++dy) {
        int row = y + dy;  
        for (int dx = 0; dx < w; ++dx) {
            int idx = row * imgW + x + dx; 
            const Pixel& p1 = ref[idx];  
            const Pixel& p2 = test[idx];  

        
            int dr = p1.r - p2.r, dg = p1.g - p2.g, db = p1.b - p2.b;
            total += dr * dr + dg * dg + db * db;  
            ++count;  
        }
    }

    return count > 0 ? total / count : 999999.0;
}

int main() {
    const std::string baseFolder = "test";  
    const std::string outputFolder = "outputs";  
    fs::create_directory(outputFolder);  

    
    std::vector<fs::directory_entry> groups;
    for (const auto& entry : fs::directory_iterator(baseFolder)) {
        if (fs::is_directory(entry)) {
            groups.push_back(entry);
        }
    }

    
    std::sort(groups.begin(), groups.end(), [](const auto& a, const auto& b) {  //lambda fonksiyonu eklendi
        return a.path().string() < b.path().string();
    });

   
    for (const auto& group : groups) {
        std::string groupPath = group.path().string();
        std::string groupName = group.path().filename().string();
        std::string referencePath = groupPath + "/perfect" + groupName.substr(5) + ".ppm";  

        std::vector<Pixel> refPixels;
        int refW, refH;
        if (!readPPM(referencePath, refPixels, refW, refH)) {
            std::cerr << "Failed to read reference: " << referencePath << "\n";
            continue;  
        }

       
        std::ofstream perfectOut(outputFolder + "/" + groupName + "_perfect_output.txt");
        std::ofstream defectOut(outputFolder + "/" + groupName + "_defect_output.txt");

        
        for (const auto& file : fs::directory_iterator(groupPath)) {
            std::string fname = file.path().filename().string();
            if (file.path().extension() != ".ppm" || file.path() == referencePath) continue;  // Yalnızca PPM dosyalarını işle

            
            std::vector<Pixel> testPixels;
            int testW, testH;
            if (!readPPM(file.path().string(), testPixels, testW, testH)) continue;

           
            if (testW != refW || testH != refH) continue;

            
            int blocksX = refW / blockSize;
            int blocksY = refH / blockSize;
            int totalBlocks = blocksX * blocksY;
            int defectBlocks = 0;

            
            for (int by = 0; by < blocksY; ++by) {
                for (int bx = 0; bx < blocksX; ++bx) {
                    int x = bx * blockSize;
                    int y = by * blockSize;
                    double diff = blockDifference(refPixels, testPixels, x, y, blockSize, blockSize, refW);
                    if (diff > blockThreshold) defectBlocks++;  
                }
            }

           
            double defectRatio = static_cast<double>(defectBlocks) / totalBlocks;
            std::string result = (defectRatio > defectRatioThreshold) ? "defection" : "perfect";

           
            if (result == "perfect") {
                perfectOut << fname << " is a perfect fabric." << std::endl;
            } else {
                defectOut << fname << " is a defected fabric." << std::endl;
            }
        }

        
        perfectOut.close();
        defectOut.close();
    }

    return 0;  
}
