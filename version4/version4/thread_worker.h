#pragma once

#include <filesystem>
#include <string>
#include <vector>

void threadAnalyzeWorker(const std::vector<std::filesystem::path>& files,
                         const std::string& referencePath,
                         const std::string& groupName,
                         int threadIndex);
