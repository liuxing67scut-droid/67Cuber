#include "utils.h"
#include <random> // 【注意】确保包含这个头文件
#include <cmath>  // 【注意】确保包含 sqrt
#include <ctime>  // 【注意】确保包含 time

void initWinPos_Title(string Title, string newTitle) {
	HWND hwnd = NULL;
	while (!hwnd)
		hwnd = FindWindowA(NULL, Title.c_str()), Sleep(1);//FindWindowW Error
	int Screen_W = GetSystemMetrics(SM_CXSCREEN);//Screen_X
	int Screen_H = GetSystemMetrics(SM_CYSCREEN);//Screen_Y
	MoveWindow(hwnd, Screen_W / 2 - Width / 2, Screen_H / 2 - Height / 2, Width, Height, true);
	if (newTitle != "")
		SetWindowTextA(hwnd, newTitle.c_str());
}



string getPathName(string path) {
	int pos = path.find_last_of('\\');
	if (pos != string::npos)path = path.substr(pos + 1);
	pos = path.find_last_of('.');
	return path.substr(0, pos);
}


//魔方自转速度变化曲线
double smoothS(double t, double T) {
	if (t <= 0) return 0.0;
	if (t >= T) return 1.0;
	double x = t / T;
	return x * x * (3 - 2 * x);
}

// 生成随机单位轴的函数实现
Axis3D randomUnitAxis() {
	static std::mt19937 rng((unsigned)time(NULL));
	std::normal_distribution<double> dist(0.0, 1.0);
	double x = dist(rng), y = dist(rng), z = dist(rng);
	double len = sqrt(x * x + y * y + z * z);
	if (len < 1e-6) return { 1,0,0 };
	return { x / len, y / len, z / len };
}