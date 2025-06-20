#include <string>
#include <vector>
#include <fstream>
#include <mutex>  
#include <filesystem>  
#include <cmath> 
#include <sstream>  
#include <iostream>  

namespace fs = std::filesystem;  


struct Pixel {
    int r, g, b;
};


extern std::mutex outputMutex;


constexpr int blockSize = 20;  
constexpr double blockThreshold = 1000.0;  
constexpr double defectRatioThreshold = 0.01;  


inline double blockDifference(const std::vector<Pixel>& ref, const std::vector<Pixel>& test,
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


inline bool readPPM(const std::string& path, std::vector<Pixel>& pixels, int& width, int& height) {
    std::ifstream file(path, std::ios::in);  
    if (!file.is_open()) return false;  

    std::string line, magic;
    file >> magic;  
    if (magic != "P3") return false;  

   
    while (file.peek() == '#' || file.peek() == '\n') {
        std::getline(file, line);
    }

    
    file >> width >> height;

    int maxVal;
    file >> maxVal; 

    pixels.reserve(width * height);  

    
    int r, g, b;
    while (file >> r >> g >> b) {
        pixels.emplace_back(Pixel{r, g, b});  
    }

    return true;  
}


void analyzeFiles(const std::vector<fs::path>& files, const std::string& referencePath,
                  const std::string& groupName, int threadIndex) {
    std::vector<Pixel> refPixels;  
    int refW, refH;
    
    
    if (!readPPM(referencePath, refPixels, refW, refH)) {
        std::cerr << "[Thread " << threadIndex << "] Failed to read reference: " << referencePath << "\n";
        return;  
    }

    
    std::ofstream perfectOut("outputs/" + groupName + "_perfect_output.txt", std::ios::app);
    std::ofstream defectOut("outputs/" + groupName + "_defect_output.txt", std::ios::app);

    
    for (const auto& file : files) {
        std::vector<Pixel> testPixels;  
        int testW, testH;
        
        
        if (!readPPM(file.string(), testPixels, testW, testH)) continue;

        
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

       // yazmayi kitleme
        std::lock_guard<std::mutex> lock(outputMutex);
        
        
        if (result == "perfect") {
            perfectOut << file.filename().string() << " is a perfect fabric.\n";
        } else {
            defectOut << file.filename().string() << " is a defected fabric.\n";
        }
    }

   
    perfectOut.close();
    defectOut.close();
}
