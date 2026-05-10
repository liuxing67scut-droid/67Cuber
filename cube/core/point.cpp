#include "point.h"

//按观察点做透视投影，EasyX 屏幕坐标 y 轴向下
POINT Point::trans(const Point* offset) const {
	double X, Y, Z;
	X = x + offset->x, Y = y + offset->y, Z = z + offset->z;
	Y = Y * (Watcher_x - Back_wall_x) / (Watcher_x - X);
	Z = Z * (Watcher_x - Back_wall_x) / (Watcher_x - X);
	return { int(Y), int(-Z) };
}

Point& Point::operator*=(double n) {
	x *= n, y *= n, z *= n;
	return *this;
}

Point Point::operator-() const {
	return { -x, -y, -z };
}

Point operator+(const Point& a, const Point& b) {
	return { a.x + b.x, a.y + b.y, a.z + b.z };
}

Point operator-(const Point& a, const Point& b) {
	return { a.x - b.x, a.y - b.y, a.z - b.z };
}

Point operator*(const Point& a, double n) {
	return { a.x * n, a.y * n, a.z * n };
}

Point operator/(const Point& a, double n) {
	return { a.x / n, a.y / n, a.z / n };
}

Point cross(const Point& u, const Point& v) {
	return { u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x };
}

double Dist(const Point& a, const Point& b) {
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
}

double Cos(const Point& a, const Point& b) {
	Point origin = { 0,0,0 };
	double alpha = (a.x * b.x + a.y * b.y + a.z * b.z) / (Dist(a, origin) * Dist(b, origin));
	//限制边界，避免浮点误差影响角度计算
	return (alpha > 0.99) ? 0.99 : (alpha < -0.99) ? -0.99 : alpha;
}

double DistToWatcher(const Point& p) {
	Point watcher = { Watcher_x, 0, 0 };
	return Dist(watcher, p);
}
