#pragma once

#include <string>
#include <vector>

using namespace std;

// 单条成绩记录类
class UserScore {
public:
    string username;   // 用户名 (或 "PC")
    double time;       // 还原用时 (秒)
    string datetime;   // 记录时间

    // 构造函数
    UserScore(string name, double t, string dt)
        : username(name), time(t), datetime(dt) {
    }
};

// ==========================================
// 全局函数声明
// ==========================================

// 1. 初始化：确保文件存在，如果不存在则创建空文件
void initRecordFile();

// 2. 保存一条成绩
// 参数: name - 用户名, time_sec - 用时
void saveScore(string name, double time_sec);

// 3. 读取所有成绩
// 返回: 包含所有成绩的 vector
vector<UserScore> getAllScores();

// 4. 获取当前时间字符串 (内部辅助，但也可以公开)
string getCurrentTimeStr();

// 4. 获取排序后的排行榜 (按时间从小到大，取前 topN 名)
// 参数: topN - 取前几名，默认 10
vector<UserScore> getSortedTopScores(int topN = 10);

// 5. (辅助) 比较函数，用于排序
bool compareScore(const UserScore& a, const UserScore& b);