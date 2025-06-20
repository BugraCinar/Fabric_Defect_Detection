#include "readPPM_binary.h"
#include <thread>
#include <filesystem>  
#include <iostream>  
#include <vector>  
#include <algorithm>  
#include <mutex>  
#include "thread_worker.h" 
#include "globals.h"  


extern void analyzeFiles(const std::vector<std::filesystem::path>& files,
                         const std::string& referencePath,
                         const std::string& groupName,
                         int threadIndex);

namespace fs = std::filesystem;  
extern std::mutex outputMutex;  


void groupWorker(const std::string& groupPath,
                 const std::string& referencePath,
                 const std::string& groupName) {
    std::vector<fs::path> imageFiles;  

    
    for (const auto& entry : fs::directory_iterator(groupPath)) {
        
        if (entry.path().extension() == ".ppm" &&
            entry.path().filename() != ("perfect" + groupName.substr(5) + ".ppm")) {
            imageFiles.push_back(entry.path());  
        }
    }

  
    std::sort(imageFiles.begin(), imageFiles.end());

    int totalFiles = imageFiles.size();  
    int threadsPerGroup = 5;  
    
    int filesPerThread = (totalFiles + threadsPerGroup - 1) / threadsPerGroup;

    std::vector<std::thread> threads;  

 
    for (int i = 0; i < threadsPerGroup; ++i) {
        int startIdx = i * filesPerThread; 
        int endIdx = std::min(startIdx + filesPerThread, totalFiles);  
        if (startIdx >= totalFiles) break;  

       
        std::vector<fs::path> subFiles(imageFiles.begin() + startIdx, imageFiles.begin() + endIdx);

        
        threads.emplace_back(threadAnalyzeWorker, subFiles, referencePath, groupName, i);
    }

    
    for (auto& t : threads) {
        t.join(); 
    }

    
    std::lock_guard<std::mutex> lock(outputMutex);  
    std::cout << "[Parent] Group '" << groupName << "' finished processing with 5 threads.\n";
}
