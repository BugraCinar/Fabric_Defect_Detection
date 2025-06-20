#include "group_worker.h"  
#include <filesystem>  
#include <thread>  
#include <vector>  
#include <iostream>  

namespace fs = std::filesystem;  

int main() {
    const std::string baseFolder = "test";  
    const std::string outputFolder = "outputs";  
    fs::create_directory(outputFolder);  

    std::vector<std::string> groups = {"group1", "group2"};

 
    std::vector<std::thread> groupThreads;

    
    for (const auto& group : groups) {
        
        std::string groupPath = baseFolder + "/" + group;
        
        
        std::string referencePath = groupPath + "/perfect" + group.substr(5) + ".ppm";

        
        
        groupThreads.emplace_back(groupWorker, groupPath, referencePath, group);
    }

    
    for (auto& t : groupThreads) {
        t.join();  
    }

    
    std::cout << "All groups processed " << std::endl;
    
    return 0;  
}