#pragma once

#include <string>
#include <graphics.h> // 用于绘图
using namespace std;

// 设置当前用户
void setCurrentUser(const string& username);

// 获取当前用户
string getCurrentUser();

// 在右上角绘制当前用户名
void drawCurrentUser(int screenWidth);

// 判断当前用户是否为游客
bool isGuest();