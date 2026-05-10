#pragma once

#include <string>

void initAppPaths(const char* exePath);

std::string getAppRootPath();
std::string getMusicPath(const std::string& fileName);
std::string getRecordFilePath();
