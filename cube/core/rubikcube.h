#pragma once

#include "cube.h"
#include "common.h"

struct StickerDragGeometry {
	POINT screenPolygon[4] = {};
	Point cubieCenter = { 0, 0, 0 };
	Point stickerCenter = { 0, 0, 0 };
	Point normal = { 0, 0, 0 };
	Point rowAxis = { 0, 0, 0 };
	Point colAxis = { 0, 0, 0 };
	int currentFace = -1;
	int currentRow = -1;
	int currentCol = -1;
};

//三阶魔方的可视模型，负责绘制、动画和贴纸映射
class RubikCube {
public:
	RubikCube();

	void draw();
	void rotateX(double degree);
	void rotateY(double degree);
	void rotateZ(double degree);

	//按当前视角映射旋转单层
	void rotateX_Local(double degree, string dir = "front");
	void rotateY_Local(double degree, string dir = "right");
	void rotateZ_Local(double degree, string dir = "up");

	//动画状态
	void rotate_Local(Dir dir, bool isRev = false);
	void updateRotate();
	bool isRotateOver();
	void setRotateStep(double Step = 0.08);

	//执行公式命令序列
	void bindCommand(const string& cmd);
	void execute();
	bool isExecuteOver();

	//教学模式只显示六个中心块
	void setCentersColoredOnly();

	//练习模式辅助方向标识
	void drawTestAxes();

	//屏幕拾取和贴纸投影
	bool pickStickerAtScreen(int mx, int my, int &out_k, int &out_i, int &out_j, int &out_planeIndex);
	void clearAllHighlights();
	bool getStickerScreenPolygon(int k, int i, int j, int planeIndex, POINT out[4]) const;
	bool getStickerDragGeometry(int k, int i, int j, int planeIndex, StickerDragGeometry& out) const;

	//只改显示颜色，不改逻辑 Cube
	void setStickerColorVisual(int k, int i, int j, int planeIndex, COLORREF col);

	//物理贴纸位置映射到逻辑面和行列
	bool mapStickerToFace(int k, int i, int j, int planeIndex, int &outFace, int &outR, int &outC);

	//按逻辑 Cube 重绘贴纸颜色
	void recolorFromLogicalCube(char srcCube[6][3][3]);

	void resetMap();

private:
	static const int D = 3;
	static const int E = 100;

	Cube cube[D][D][D];
	Point offset[D][D][D];

	Dir RotateDir = Still;
	bool isReverse = false;
	istringstream iss;

	//六个方向的当前可视轴映射
	vector<Point*> Map;
	map<char, Dir> m;

	double step = 0.08;
	double animDegree = 0.0;
};
