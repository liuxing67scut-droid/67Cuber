#include "app_paths.h"

#include <windows.h>

namespace {
std::string g_appRootPath = ".";

//去掉目录末尾的分隔符，便于逐级向上查找
std::string trimTrailingSlash(std::string path) {
    while (!path.empty() && (path.back() == '\\' || path.back() == '/')) {
        path.pop_back();
    }
    return path;
}

//拼接 Windows 路径片段
std::string joinPath(const std::string& left, const std::string& right) {
    if (left.empty()) return right;
    if (right.empty()) return left;

    char last = left.back();
    if (last == '\\' || last == '/') {
        return left + right;
    }
    return left + "\\" + right;
}

//获取父目录
std::string getParentDir(const std::string& path) {
    std::string cleanPath = trimTrailingSlash(path);
    size_t pos = cleanPath.find_last_of("\\/");
    if (pos == std::string::npos) return "";
    return cleanPath.substr(0, pos);
}

//传入文件或目录路径时，统一得到所在目录
std::string getDirName(const std::string& path) {
    DWORD attrs = GetFileAttributesA(path.c_str());
    if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY)) {
        return trimTrailingSlash(path);
    }
    return getParentDir(path);
}

//把可执行文件路径转成绝对路径
std::string getFullPath(const char* path) {
    if (!path || path[0] == '\0') {
        char currentDir[MAX_PATH] = { 0 };
        DWORD length = GetCurrentDirectoryA(MAX_PATH, currentDir);
        if (length > 0 && length < MAX_PATH) {
            return currentDir;
        }
        return ".";
    }

    char buffer[MAX_PATH] = { 0 };
    DWORD length = GetFullPathNameA(path, MAX_PATH, buffer, nullptr);
    if (length > 0 && length < MAX_PATH) {
        return buffer;
    }
    return path ? path : "";
}

bool fileExists(const std::string& path) {
    DWORD attrs = GetFileAttributesA(path.c_str());
    return attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY);
}

bool dirExists(const std::string& path) {
    DWORD attrs = GetFileAttributesA(path.c_str());
    return attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY);
}

//通过解决方案文件识别工程根目录
bool hasSolutionFile(const std::string& path) {
    WIN32_FIND_DATAA findData;
    HANDLE handle = FindFirstFileA(joinPath(path, "*.sln").c_str(), &findData);
    if (handle == INVALID_HANDLE_VALUE) {
        return false;
    }
    FindClose(handle);
    return true;
}

//工程根目录需要同时包含解决方案、源码目录和音乐目录
bool isProjectRoot(const std::string& path) {
    return hasSolutionFile(path) &&
        dirExists(joinPath(path, "cube")) &&
        dirExists(joinPath(path, "music"));
}

//从 exe 所在目录向上查找工程根目录
std::string findProjectRoot(const std::string& startDir) {
    std::string current = trimTrailingSlash(startDir);
    while (!current.empty()) {
        if (isProjectRoot(current)) {
            return current;
        }

        std::string parent = getParentDir(current);
        if (parent == current) break;
        current = parent;
    }
    return trimTrailingSlash(startDir);
}
}

//程序启动时初始化工程根目录
void initAppPaths(const char* exePath) {
    std::string exeFullPath = getFullPath(exePath);
    std::string exeDir = getDirName(exeFullPath);
    g_appRootPath = findProjectRoot(exeDir);
}

std::string getAppRootPath() {
    return g_appRootPath;
}

//获取音乐资源文件路径
std::string getMusicPath(const std::string& fileName) {
    return joinPath(joinPath(g_appRootPath, "music"), fileName);
}

//获取排行榜记录文件路径
std::string getRecordFilePath() {
    return joinPath(joinPath(g_appRootPath, "data"), "record.csv");
}
