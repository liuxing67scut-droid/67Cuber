#pragma once

#include "common.h"

//求解模块共享状态，定义在 cubesolver.cpp
extern const int F, B, L, R, U, D;
extern const char Color[6];
extern string Command;
extern int step;
extern bool isSrand;

//基础状态和随机打乱
void init(char cube[][3][3]);
void random(char cube[][3][3], int N);
//练习模式随机只转单面，避免改变视角映射
void random_practice(char cube[][3][3], int N);
void setColor(unsigned short ForeColor, unsigned short BackGroundColor);

//基础转动
void rotate(char cube[][3][3], int mode, int direct);
void front(char cube[][3][3], int mode);
void left(char cube[][3][3], int mode);
void right(char cube[][3][3], int mode);
void back(char cube[][3][3], int mode);
void up(char cube[][3][3], int mode);
void down(char cube[][3][3], int mode);
void move(char cube[][3][3], const char* com);
void turn(char cube[][3][3]);
void Fturn(char cube[][3][3]);

//分层求解步骤
void DownCross(char cube[][3][3]);
void DownMid(char cube[][3][3]);
void DownCorner(char cube[][3][3]);

void MidEdge(char cube[][3][3]);

void TopCross(char cube[][3][3]);
void TopCorner(char cube[][3][3]);
void TopLayerCorner(char cube[][3][3]);
void TopEdge(char cube[][3][3]);

//生成还原命令并更新 cube 状态
//求解过程包含整块转动，调整 turn/Fturn 时需同步验证动画映射
void Solute(char cube[][3][3]);

//教学模式录入结果的基本合法性检查
bool validateColoring(const char cube[6][3][3], string &reason);

//判断当前逻辑魔方是否已经复原
bool isCubeSolved(const char Cube[6][3][3]);

