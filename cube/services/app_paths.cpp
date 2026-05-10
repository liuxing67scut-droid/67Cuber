#include "app_paths.h"

#include <windows.h>

namespace {
std::string g_appRootPath = ".";

std::string trimTrailingSlash(std::string path) {
    while (!path.empty() && (path.back() == '\\' || path.back() == '/')) {
        path.pop_back();
    }
    return path;
}

std::string joinPath(const std::string& left, const std::string& right) {
    if (left.empty()) return right;
    if (right.empty()) return left;

    char last = left.back();
    if (last == '\\' || last == '/') {
        return left + right;
    }
    return left + "\\" + right;
}

std::string getParentDir(const std::string& path) {
    std::string cleanPath = trimTrailingSlash(path);
    size_t pos = cleanPath.find_last_of("\\/");
    if (pos == std::string::npos) return "";
    return cleanPath.substr(0, pos);
}

std::string getDirName(const std::string& path) {
    DWORD attrs = GetFileAttributesA(path.c_str());
    if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY)) {
        return trimTrailingSlash(path);
    }
    return getParentDir(path);
}

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

bool hasSolutionFile(const std::string& path) {
    WIN32_FIND_DATAA findData;
    HANDLE handle = FindFirstFileA(joinPath(path, "*.sln").c_str(), &findData);
    if (handle == INVALID_HANDLE_VALUE) {
        return false;
    }
    FindClose(handle);
    return true;
}

bool isProjectRoot(const std::string& path) {
    return hasSolutionFile(path) &&
        dirExists(joinPath(path, "cube")) &&
        dirExists(joinPath(path, "music"));
}

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

void initAppPaths(const char* exePath) {
    std::string exeFullPath = getFullPath(exePath);
    std::string exeDir = getDirName(exeFullPath);
    g_appRootPath = findProjectRoot(exeDir);
}

std::string getAppRootPath() {
    return g_appRootPath;
}

std::string getMusicPath(const std::string& fileName) {
    return joinPath(joinPath(g_appRootPath, "music"), fileName);
}

std::string getRecordFilePath() {
    return joinPath(joinPath(g_appRootPath, "data"), "record.csv");
}
