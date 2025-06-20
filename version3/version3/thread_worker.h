
#pragma once

#include <string>
#include <vector>
#include <filesystem>

void analyzeFiles(const std::vector<std::filesystem::path>& files,
                  const std::string& referencePath,
                  const std::string& groupName,
                  int threadIndex);