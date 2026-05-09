#pragma once

#include "common.h"

// 全局变量声明 (定义在 cubesolver.cpp 中)
extern const int F, B, L, R, U, D;
extern const char Color[6];
extern string Command;
extern int step;
extern bool isSrand;


void init(char cube[][3][3]);
void random(char cube[][3][3], int N);
// 练习模式专用随机：不包含整块转动（turn/Fturn），避免改变视角映射
void random_practice(char cube[][3][3], int N);
void setColor(unsigned short ForeColor, unsigned short BackGroundColor);
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

void DownCross(char cube[][3][3]);
void DownMid(char cube[][3][3]);
void DownCorner(char cube[][3][3]);

void MidEdge(char cube[][3][3]);

void TopCross(char cube[][3][3]);
void TopCorner(char cube[][3][3]);
void TopLayerCorner(char cube[][3][3]);
void TopEdge(char cube[][3][3]);

void Solute(char cube[][3][3]);//!!Bug:turn 之后 Fturn方向改变 同理Fturn 之后 turn方向改变!!!!!!!!!!!!!!!!!!!!!!!!!!!




// 新增：验证着色是否合法（返回 false 并通过 reason 说明原因）
bool validateColoring(const char cube[6][3][3], string &reason);

//补充
bool isCubeSolved(const char Cube[6][3][3]);

