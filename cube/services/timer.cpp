#include "timer.h"

Timer::Timer() { reset(); }
void Timer::reset() { start = clock(); }
double Timer::elapsed() const { return (double)(clock() - start) / CLOCKS_PER_SEC; }

// -------------------- SolveTimer --------------------
SolveTimer::SolveTimer() : acc(0.0), t0(0), running(false) {}

void SolveTimer::start() {
    if (!running) {
        running = true;
        t0 = clock();
    }
}

void SolveTimer::stop() {
    if (running) {
        double dt = (double)(clock() - t0) / CLOCKS_PER_SEC;
        acc += dt;
        running = false;
    }
}

void SolveTimer::reset() {
    acc = 0.0;
    running = false;
    t0 = 0;
}

double SolveTimer::elapsed() const {
    if (running) {
        double dt = (double)(clock() - t0) / CLOCKS_PER_SEC;
        return acc + dt;
    }
    return acc;
}

bool SolveTimer::isRunning() const { return running; }

std::string SolveTimer::format() const {
    double t = elapsed();
    int minutes = (int)(t / 60);
    double rem = t - minutes * 60;
    int seconds = (int)rem;
    int centi = (int)((rem - seconds) * 100);
    char buf[64];
    sprintf_s(buf, sizeof(buf), "%02d:%02d.%02d", minutes, seconds, centi);
    return std::string(buf);
}
