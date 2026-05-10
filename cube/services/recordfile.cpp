#include "recordfile.h"
#include "app_paths.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <iostream>

//确保成绩文件存在，并在首次创建时写入表头
void initRecordFile() {
    const string fileName = getRecordFilePath();
    ifstream inFile(fileName);
    if (!inFile) {
        ofstream outFile(fileName);
        outFile << "Username,Time,DateTime" << endl;
        outFile.close();
        cout << "已创建成绩文件: " << fileName << endl;
    }
    else {
        inFile.close();
    }
}

string getCurrentTimeStr() {
    time_t t = time(nullptr);
    struct tm tm;
    localtime_s(&tm, &t);

    //日期不补零，保持原 CSV 显示格式
    char buf[64];
    sprintf_s(buf, "%d/%d/%d %02d:%02d:%02d",
        tm.tm_year + 1900,
        tm.tm_mon + 1,
        tm.tm_mday,
        tm.tm_hour,
        tm.tm_min,
        tm.tm_sec);

    return string(buf);
}

void saveScore(string name, double time_sec) {
    const string fileName = getRecordFilePath();
    ofstream outFile(fileName, ios::out | ios::app);

    if (!outFile.is_open()) {
        cout << "错误：无法打开文件保存成绩" << endl;
        return;
    }

    string now = getCurrentTimeStr();
    outFile << name << "," << time_sec << "," << now << endl;

    outFile.close();
    cout << "成绩已保存: " << name << " - " << time_sec << "s" << endl;
}

vector<UserScore> getAllScores() {
    vector<UserScore> result;
    ifstream inFile(getRecordFilePath());

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
                continue;
            }
        }
    }

    inFile.close();
    return result;
}

bool compareScore(const UserScore& a, const UserScore& b) {
    return a.time < b.time;
}

vector<UserScore> getSortedScores() {
    vector<UserScore> allData = getAllScores();

    if (allData.empty()) {
        return allData;
    }

    sort(allData.begin(), allData.end(), compareScore);
    return allData;
}

vector<UserScore> getSortedTopScores(int topN) {
    vector<UserScore> allData = getSortedScores();

    vector<UserScore> result;
    int count = min((int)allData.size(), topN);
    for (int i = 0; i < count; i++) {
        result.push_back(allData[i]);
    }

    return result;
}
