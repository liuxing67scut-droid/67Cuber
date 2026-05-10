#pragma once

#include "plane.h"

//单个小方块，维护局部顶点和六个可绘制面
class Cube {
public:
	Cube();

	void setOffset(Point Offset);
	void showCube();
	Plane* getPlanes();

	//绕自身局部轴或指定轴旋转
	void rotateX(double degree);
	void rotateY(double degree);
	void rotateZ(double degree);
	void rotateX_Local(double degree, const Point A = { 0,0,0 }, const Point B = { 0,0,0 }, bool bUseAB = false);
	void rotateY_Local(double degree, const Point A = { 0,0,0 }, const Point B = { 0,0,0 }, bool bUseAB = false);
	void rotateZ_Local(double degree, const Point A = { 0,0,0 }, const Point B = { 0,0,0 }, bool bUseAB = false);

	//直接旋转点坐标
	static void rotateX(Point& p, double degree);
	static void rotateY(Point& p, double degree);
	static void rotateZ(Point& p, double degree);
	static Point rotateByVector(const Point& V, const Point& a, const Point& b, double T);

	friend class RubikCube;

private:
	const double e = 50;
	static const int N = 8;   //8个顶点
	static const int P_N = 6; //6个面

	Point offset = { 0,0,0 };
	Point vertex[N];
	Plane plane[P_N];
};
