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

#define Width 1440
#define Height 900
#define PI 3.1415926535898	//魔方旋转角度相关
#define GetKey(X) (GetAsyncKeyState(X) & 0x8000)

const int Watcher_x = 1000, Back_wall_x = -100;

enum Dir :int {
	Left, Front, Right, Back, Up, Down, Still
};