#pragma once
#include "common.h"

void initWinPos_Title(string Title, string newTitle = "");

string getPathName(string path);

//速度曲线
double smoothS(double t, double T);

//3D轴结构体
struct Axis3D {
    double x, y, z;
};

//生成随机单位轴
Axis3D randomUnitAxis();
