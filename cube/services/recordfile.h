#pragma once

#include <string>
#include <vector>

using namespace std;

//单条成绩记录
class UserScore {
public:
    string username;
    double time;
    string datetime;

    UserScore(string name, double t, string dt)
        : username(name), time(t), datetime(dt) {
    }
};

//成绩文件操作
void initRecordFile();
void saveScore(string name, double time_sec);
vector<UserScore> getAllScores();
string getCurrentTimeStr();

//排行榜按用时升序返回前 topN 名
vector<UserScore> getSortedTopScores(int topN = 10);
bool compareScore(const UserScore& a, const UserScore& b);
