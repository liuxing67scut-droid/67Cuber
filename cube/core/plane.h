#pragma once
#include "point.h"

class Plane {
public:
	Plane() = default;
	Plane(const Point* a, const Point* b, const Point* c, const Point* d);
	void bindPoint(const Point* a, const Point* b, const Point* c, const Point* d);
	void bindOffset(const Point* off);
	void setColor(COLORREF col);
	Point Center() const;
	void draw() const;

	//屏幕拾取用的投影四边形
	void getScreenPolygon(POINT out[4], const Point* off) const;
	bool containsScreenPoint(int mx, int my, const Point* off) const;

	//选中面高亮
	void setHighlight(bool h);

	friend class Cube;
	friend bool operator>(const Plane& a, const Plane& b);
private:
	const Point* p[4];
	const Point* offset;
	COLORREF color = RGB(50, 50, 50);
	bool highlight = false;
};
bool operator>(const Plane& a, const Plane& b);
