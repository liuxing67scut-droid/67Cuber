#include "plane.h"

namespace {
const COLORREF CUBE_EDGE_COLOR = RGB(72, 50, 32);
const int CUBE_EDGE_WIDTH = 2;
}

Plane::Plane(const Point* a, const Point* b, const Point* c, const Point* d) {
	bindPoint(a, b, c, d);
}

void Plane::bindPoint(const Point* a, const Point* b, const Point* c, const Point* d) {
	p[0] = a, p[1] = b, p[2] = c, p[3] = d;
}

void Plane::bindOffset(const Point* off) {
	offset = off;
}

void Plane::setColor(COLORREF col) {
	color = col;
}

Point Plane::Center() const {
	//对角线中点加小方块偏移
	Point center = (*p[0] + *p[2]) / 2;
	return center + *offset;
}

//缓存投影后的屏幕四边形，供绘制和拾取共用
void Plane::getScreenPolygon(POINT out[4], const Point* off) const {
	out[0] = p[0]->trans(off);
	out[1] = p[1]->trans(off);
	out[2] = p[2]->trans(off);
	out[3] = p[3]->trans(off);
}

static bool pointInPolygon(int x, int y, const POINT poly[], int n) {
	//射线法判断点是否在多边形内
	bool c = false;
	for (int i = 0, j = n - 1; i < n; j = i++) {
		int xi = poly[i].x, yi = poly[i].y;
		int xj = poly[j].x, yj = poly[j].y;
		bool intersect = ((yi > y) != (yj > y)) && (x < (xj - xi) * (y - yi) / (double)(yj - yi) + xi);
		if (intersect) c = !c;
	}
	return c;
}

bool Plane::containsScreenPoint(int mx, int my, const Point* off) const {
	if (!off) return false;
	POINT poly[4];
	getScreenPolygon(poly, off);
	return pointInPolygon(mx, my, poly, 4);
}

void Plane::setHighlight(bool h) {
	highlight = h;
}

void Plane::draw() const {
	setfillcolor(color);
	setlinecolor(CUBE_EDGE_COLOR);
	setlinestyle(PS_SOLID, CUBE_EDGE_WIDTH, NULL, 0);

	POINT point[4] = {
		p[0]->trans(offset),
		p[1]->trans(offset),
		p[2]->trans(offset),
		p[3]->trans(offset)
	};

	fillpolygon(point, 4);

	if (highlight) {
		//高亮选中的面
		setlinestyle(PS_SOLID, 3, NULL, 0);
		setlinecolor(RGB(255, 255, 0));
		for (int i = 0; i < 4; i++) {
			int ni = (i + 1) % 4;
			line(point[i].x, point[i].y, point[ni].x, point[ni].y);
		}
		setlinecolor(CUBE_EDGE_COLOR);
		setlinestyle(PS_SOLID, 1, NULL, 0);
	}

	setlinestyle(PS_SOLID, 1, NULL, 0);
}

bool operator>(const Plane& a, const Plane& b) {
	Point C_a = a.Center();
	Point C_b = b.Center();
	//距离观察点越远越先绘制
	return DistToWatcher(C_a) > DistToWatcher(C_b);
}
