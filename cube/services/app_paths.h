#pragma once

#include <string>

//初始化工程运行路径
void initAppPaths(const char* exePath);

//统一获取资源和数据文件路径
std::string getAppRootPath();
std::string getImagePath(const std::string& fileName);
std::string getMusicPath(const std::string& fileName);
std::string getRecordFilePath();
