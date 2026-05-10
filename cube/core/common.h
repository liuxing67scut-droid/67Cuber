#pragma once

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:6031)
#pragma warning(disable:26451)
#pragma warning(disable:26495)
#pragma warning(disable:26812)
#pragma comment(lib,"user32.lib")

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <graphics.h>
#include <cmath>
#include <string.h>
#include <vector>
#include <algorithm>
#include <utility>
#include <string>
#include <time.h>
#include <sstream>
#include <map>
#include <Windows.h>

using namespace std;

//窗口和基础数学常量
#define Width 1440
#define Height 900
#define PI 3.1415926535898
#define GetKey(X) (GetAsyncKeyState(X) & 0x8000)

//透视投影使用的观察点和远端参考面
const int Watcher_x = 1000, Back_wall_x = -100;

//魔方旋转方向
enum Dir :int {
	Left, Front, Right, Back, Up, Down, Still
};
