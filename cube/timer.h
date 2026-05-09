#pragma once
#include <ctime>
#include <string>

class Timer {
public:
    Timer();
    void reset();
    double elapsed() const; // 返回秒
private:
    clock_t start;
};

// SolveTimer: 用于记录一次解魔方的计时（界面显示）
class SolveTimer {
public:
    SolveTimer();
    void start();    // 开始计时（如果已经在计时则继续）
    void stop();     // 停止计时（保留已用时间）
    void reset();    // 重置为0（停止状态）
    double elapsed() const; // 返回秒
    std::string format() const; // 返回 mm:ss.xx
    bool isRunning() const;
private:
    double acc; // 累计秒数
    clock_t t0; // 启动时间点
    bool running;
};
