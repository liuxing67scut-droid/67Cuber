#pragma once
#include <ctime>
#include <string>

class Timer {
public:
    Timer();
    void reset();
    double elapsed() const;
private:
    clock_t start;
};

//用于记录一次解魔方的计时
class SolveTimer {
public:
    SolveTimer();
    void start();
    void stop();
    void reset();
    double elapsed() const;
    std::string format() const;
    bool isRunning() const;
private:
    double acc;
    clock_t t0;
    bool running;
};
