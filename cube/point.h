#pragma once

#include "common.h"

class Point {
public:
	double x, y, z;
	POINT trans(const Point* offset) const; // 透视投影
	Point& operator*=(double n);
	Point operator-() const;
	friend Point operator+(const Point& a, const Point& b);
	friend Point operator-(const Point& a, const Point& b);
	friend Point operator*(const Point& a, double n);
	friend Point operator/(const Point& a, double n);
};

// 全局几何工具函数
Point cross(const Point& u, const Point& v);
double Dist(const Point& a, const Point& b);
double Cos(const Point& a, const Point& b);
double DistToWatcher(const Point& p);
