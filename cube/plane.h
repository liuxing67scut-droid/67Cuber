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

	// 新增：获取屏幕多边形并进行点内检测（用于拾取）
	void getScreenPolygon(POINT out[4], const Point* off) const;
	bool containsScreenPoint(int mx, int my, const Point* off) const;

	// 新增：高亮控制（仅视觉效果）
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