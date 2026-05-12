#pragma once

#include "common.h"

//魔方贴纸颜色集中入口，避免各模式各自维护 RGB
COLORREF cubeColorToRgb(char color);

//按逻辑面编号读取默认颜色字符，面编号来自 cubesolver
char cubeFaceColorChar(int face);

//按逻辑面编号读取默认绘制颜色
COLORREF cubeFaceColorToRgb(int face);
