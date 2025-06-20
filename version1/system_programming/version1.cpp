#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <cmath>
#include <string>
#include <algorithm>
#include <unordered_set>
#include <set>

namespace fs = std::filesystem;


struct Pixel {
    int r, g, b;
};


bool readPPM(const std::string& path, std::vector<Pixel>& pixels, int& width, int& height) {  //oku vectore yukle
    std::ifstream file(path);  
    std::string magic; 
    file >> magic;

    
    if (magic != "P3") return false;

    std::string line;
    std::getline(file, line);  
    do {
        std::getline(file, line);  
    } while (line[0] == '#');
    
    
    std::istringstream dim(line);
    dim >> width >> height;

    int maxVal;
    file >> maxVal;  // pixel yogunluk degeri

    int r, g, b;
   
    while (file >> r >> g >> b) {
        pixels.push_back({r, g, b});
    }

    return true;
}


double blockDifference(const std::vector<Pixel>& ref, const std::vector<Pixel>& test, int x, int y, int w, int h, int imgW) { //fark kismi
    double total = 0;
    int count = 0;

    
    for (int dy = 0; dy < h; ++dy) {
        for (int dx = 0; dx < w; ++dx) {
            int idx = (y + dy) * imgW + (x + dx);  // pixel index?
            if (idx >= ref.size() || idx >= test.size()) continue;  
            auto& p1 = ref[idx];  
            auto& p2 = test[idx];  
            
            int dr = p1.r - p2.r, dg = p1.g - p2.g, db = p1.b - p2.b; // rgb farklari
            total += std::sqrt(dr * dr + dg * dg + db * db);  
        }
    }

    
    return count > 0 ? total / count : 999999.0;
}

int main() {
    std::string baseFolder = "test";  
    std::string outputFolder = "outputs";  
    std::string analyzeFolder = "analyze";  
    fs::create_directory(outputFolder);  
    fs::create_directory(analyzeFolder);  
    const int blockSize = 10;  
    const double blockThreshold = 7.0;  //esik deger
    const double defectRatioThreshold = 0.01;  // defect orani max

    std::vector<std::string> groupPaths;

   
    for (const auto& entry : fs::directory_iterator(baseFolder)) {
        if (fs::is_directory(entry)) {
            groupPaths.push_back(fs::canonical(entry).string());
        }
    }

    
    std::sort(groupPaths.begin(), groupPaths.end());

    std::set<std::string> processedGroups;  

   
    for (const auto& groupPath : groupPaths) {
        if (processedGroups.count(groupPath)) continue;  
        processedGroups.insert(groupPath);  

        std::string groupName = fs::path(groupPath).filename().string();
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
            if (file.path().extension() != ".ppm" || file.path() == referencePath) continue;  

            std::vector<Pixel> testPixels;
            int testW, testH;
            
            if (!readPPM(file.path().string(), testPixels, testW, testH)) continue;

            
            if (testW != refW || testH != refH) {
                std::cout << file.path().string() << " = incompatible size\n";
                continue;
            }

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

            
            double defectRatio = (double)defectBlocks / totalBlocks;
            std::string result = (defectRatio > defectRatioThreshold) ? "defection" : "perfect";  

            
            if (result == "perfect") {
                perfectOut << fname << " is a perfect fabric." << std::endl;
            } else {
                defectOut << fname << " is a defected fabric." << std::endl;
            }

            std::cout << file.path().string() << " = " << result << "\n";  
        }

        perfectOut.close();  
        defectOut.close();  
    }

    return 0;  
}
