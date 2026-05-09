#pragma once

#include "cube.h"
#include "common.h" // 包含 Dir 枚举


class RubikCube {
public:
	// 1. 构造函数
	RubikCube();

	// 2. 核心功能
	void draw();
	void rotateX(double degree);
	void rotateY(double degree);
	void rotateZ(double degree);

	// 3. 单层旋转 (注意：默认参数只在声明处写)
	void rotateX_Local(double degree, string dir = "front");
	void rotateY_Local(double degree, string dir = "right");
	void rotateZ_Local(double degree, string dir = "up");

	// 4. 动画控制
	void rotate_Local(Dir dir, bool isRev = false);
	void updateRotate();
	bool isRotateOver();
	void setRotateStep(double Step = 0.08);

	// 5. 命令执行
	void bindCommand(const string& cmd);
	void execute();
	bool isExecuteOver();





	// 教学模式辅助：只显示六个中心块的颜色，其他块置为深灰（视觉效果）
	void setCentersColoredOnly();

	// 测试：绘制坐标轴并标注面字母（用于练习模式帮助）
	void drawTestAxes();

	// 新增：拾取与高亮接口，用于教学模式右键选择贴纸
	// 鼠标坐标为相对于魔方渲染原点（通常为 Width/2,Height/2）的屏幕坐标
	bool pickStickerAtScreen(int mx, int my, int &out_k, int &out_i, int &out_j, int &out_planeIndex);
	void clearAllHighlights();

	// 新增：设置可视贴纸颜色（不修改逻辑 Cube）
	void setStickerColorVisual(int k, int i, int j, int planeIndex, COLORREF col);
	// 新增：把贴纸映射到逻辑面与格子
	//v0
	bool mapStickerToFace(int k, int i, int j, int planeIndex, int &outFace, int &outR, int &outC);
	//v1
	//bool mapStickerToFace(int k, int i, int j, int p, int& face, int& r, int& c) const;



	//修“卡死”
	// 新增：根据逻辑数组 Cube 重绘所有颜色（仅在魔方未旋转时有效）
	void recolorFromLogicalCube(char srcCube[6][3][3]);

	// 新增：重置 Map 映射为初始状态（教学模式测试还原前必须调用）
	void resetMap();

private:
	// 私有常量
	static const int D = 3;
	static const int E = 100;

	// 成员变量
	Cube cube[D][D][D];
	Point offset[D][D][D];

	Dir RotateDir = Still;
	bool isReverse = false;
	istringstream iss;

	// 注意：这里只声明容器，具体初始化放在 .cpp 构造函数中，避免复杂的类内初始化问题
	vector<Point*> Map;
	map<char, Dir> m;

	

	double step = 0.08;
	// 每个实例的当前动画进度（弧度），替换原先 updateRotate 中的 static 局部变量
	double animDegree = 0.0;
};

















