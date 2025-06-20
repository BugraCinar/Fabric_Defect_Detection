#include "group_worker.h"
#include "thread_worker.h"

#include <thread>  
#include <mutex>  
#include <iostream>  
#include <algorithm>  

namespace fs = std::filesystem; 


std::mutex outputMutex;


void groupWorker(const std::string& groupPath, const std::string& referencePath, const std::string& groupName) {
    std::vector<fs::path> imageFiles;  

    
    for (const auto& entry : fs::directory_iterator(groupPath)) {
       
        if (entry.path().extension() == ".ppm") {
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

        
        threads.emplace_back(analyzeFiles, subFiles, referencePath, groupName, i);
    }

  
    for (auto& t : threads) {
        t.join();  
    }

    
    std::cout << "[Parent] Group '" << groupName << "' finished processing with 5 threads.\n";
}
