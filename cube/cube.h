
#pragma once

#include "plane.h" // Cube 包含 Plane，所以需要包含

class Cube {
public:
	// 1. 构造函数
	Cube();

	// 2. 普通成员函数
	void setOffset(Point Offset);
	void showCube();
	Plane* getPlanes();

	// 3. 本地旋转 (非静态)
	// 注意：默认参数只能在声明里写，cpp里不能写
	void rotateX(double degree);
	void rotateY(double degree);
	void rotateZ(double degree);
	void rotateX_Local(double degree, const Point A = { 0,0,0 }, const Point B = { 0,0,0 }, bool bUseAB = false);
	void rotateY_Local(double degree, const Point A = { 0,0,0 }, const Point B = { 0,0,0 }, bool bUseAB = false);
	void rotateZ_Local(double degree, const Point A = { 0,0,0 }, const Point B = { 0,0,0 }, bool bUseAB = false);

	// 4. 静态工具函数 (可以直接通过类名调用)
	static void rotateX(Point& p, double degree);
	static void rotateY(Point& p, double degree);
	static void rotateZ(Point& p, double degree);
	static Point rotateByVector(const Point& V, const Point& a, const Point& b, double T);

	// 5. 友元声明
	friend class RubikCube;

private:
	// 私有成员
	const double e = 50;
	static const int N = 8;   // 8个顶点
	static const int P_N = 6; // 6个面

	Point offset = { 0,0,0 };
	Point vertex[N]; // 顶点数组
	Plane plane[P_N]; // 面数组
};
