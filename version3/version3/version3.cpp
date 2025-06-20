#include <iostream>
#include <thread>  
#include <filesystem>  
#include "group_worker.h"  

namespace fs = std::filesystem;  

int main() {
    const std::string baseFolder = "test";  
    const std::string outputFolder = "outputs";  
    fs::create_directory(outputFolder);  

    
    std::vector<std::string> groups = {"group1", "group2"};
    
    
    std::vector<std::thread> parentThreads;

   
    for (const auto& groupName : groups) {
        
        std::string groupPath = baseFolder + "/" + groupName;
        
        
        std::string referenceFile = groupPath + "/perfect" + groupName.substr(5) + ".ppm";

        // parent baslama yeri
        parentThreads.emplace_back(groupWorker, groupPath, referenceFile, groupName);
    }

   
    for (auto& t : parentThreads) {
        t.join();  
    }

    
    std::cout << "All groups processed with parent threads." << std::endl;
    
    return 0;  
}
