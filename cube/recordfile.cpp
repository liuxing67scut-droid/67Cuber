
#include "recordfile.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <iostream>

#include <algorithm>//sort需要

// 文件名 全局变量
const string FILENAME = "record.csv";

// ---------------------------------------------------------
// 0. 初始化文件
// ---------------------------------------------------------
void initRecordFile() {
    // 尝试以只读方式打开
    ifstream inFile(FILENAME);
    if (!inFile) {
        // 文件不存在，创建一个空文件
        ofstream outFile(FILENAME);
        //写入表头
        outFile << "Username,Time,DateTime" << endl;
        outFile.close();
        cout << "已创建成绩文件: " << FILENAME << endl;
    }
    else {
        inFile.close();
    }
}

// ---------------------------------------------------------
// 1. 获取当前时间
// ---------------------------------------------------------
string getCurrentTimeStr() {
    time_t t = time(nullptr);
    struct tm tm;
    localtime_s(&tm, &t);

    // 手动拼接：年/月/日（不补零） + 时间
    // tm_year: 从1900开始  |  tm_mon: 0~11  |  tm_mday:1~31
    char buf[64];
    sprintf_s(buf, "%d/%d/%d %02d:%02d:%02d",
        tm.tm_year + 1900,   // 年
        tm.tm_mon + 1,       // 月（不补零：3 → 3，不是03）
        tm.tm_mday,          // 日（不补零：8 → 8，不是08）
        tm.tm_hour,          // 时
        tm.tm_min,           // 分
        tm.tm_sec);          // 秒

    return string(buf);
}

// ---------------------------------------------------------
// 2. 保存成绩
// ---------------------------------------------------------
void saveScore(string name, double time_sec) {
    // 打开文件：ios::app 表示追加
    ofstream outFile(FILENAME, ios::out | ios::app);

    if (!outFile.is_open()) {
        cout << "错误：无法打开文件保存成绩" << endl;
        return;
    }

    string now = getCurrentTimeStr();

    // 写入 CSV
    outFile << name << "," << time_sec << "," << now << endl;

    outFile.close();
    cout << "成绩已保存: " << name << " - " << time_sec << "s" << endl;
}

// ---------------------------------------------------------
// 3. 读取所有成绩
// ---------------------------------------------------------
vector<UserScore> getAllScores() {
    vector<UserScore> result;
    ifstream inFile(FILENAME);

    if (!inFile.is_open()) {
        return result;
    }

    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string name, timeStr, dateStr;

        if (getline(ss, name, ',') &&
            getline(ss, timeStr, ',') &&
            getline(ss, dateStr, ',')) {

            try {
                double t = stod(timeStr);
                result.emplace_back(name, t, dateStr);
            }
            catch (...) {
                continue; // 跳过坏数据
            }
        }
    }

    inFile.close();
    return result;
}

// ---------------------------------------------------------
//4.辅助比较函数
// ---------------------------------------------------------
bool compareScore(const UserScore& a, const UserScore& b) {
    // 如果 a 的时间 < b 的时间，a 应该排在前面，返回 true
    return a.time < b.time;
}

// ---------------------------------------------------------
// 5.获取排序后的前 N 名
// ---------------------------------------------------------
vector<UserScore> getSortedTopScores(int topN) {
    // 1. 先读取所有数据
    vector<UserScore> allData = getAllScores();

    // 2. 如果没有数据，直接返回空
    if (allData.empty()) {
        return allData;
    }

    // 3. 使用 std::sort 排序
    // 需要包含 <algorithm> 头文件，如果报错请在 cpp 开头加上 #include <algorithm>
    sort(allData.begin(), allData.end(), compareScore);

    // 4. 截取前 topN 名
    vector<UserScore> result;
    int count = min((int)allData.size(), topN);
    for (int i = 0; i < count; i++) {
        result.push_back(allData[i]);
    }

    return result;
}