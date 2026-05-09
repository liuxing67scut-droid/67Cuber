
#include "point.h" // 必须包含自己的头文件

// ---------------------------------------------------------
// 1. 实现 Point 类的成员函数
// ---------------------------------------------------------

// 透视变换函数
POINT Point::trans(const Point* offset) const {
	double X, Y, Z;
	X = x + offset->x, Y = y + offset->y, Z = z + offset->z;
	Y = Y * (Watcher_x - Back_wall_x) / (Watcher_x - X);
	Z = Z * (Watcher_x - Back_wall_x) / (Watcher_x - X);
	return { int(Y), int(-Z) };
}

// 重载 *= 运算符
Point& Point::operator*=(double n) {
	x *= n, y *= n, z *= n;
	return *this;
}

// 重载负号运算符 (例如 -p)
Point Point::operator-() const {
	return { -x, -y, -z };
}

// ---------------------------------------------------------
// 2. 实现友元函数 (operator+, -, *, /)
// ---------------------------------------------------------

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

// ---------------------------------------------------------
// 3. 实现全局几何工具函数
// ---------------------------------------------------------

Point cross(const Point& u, const Point& v) {
	return { u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x };
}

double Dist(const Point& a, const Point& b) {
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
}

double Cos(const Point& a, const Point& b) {
	Point origin = { 0,0,0 };
	double alpha = (a.x * b.x + a.y * b.y + a.z * b.z) / (Dist(a, origin) * Dist(b, origin));
	return (alpha > 0.99) ? 0.99 : (alpha < -0.99) ? -0.99 : alpha;
}

double DistToWatcher(const Point& p) {
	Point watcher = { Watcher_x, 0, 0 };
	return Dist(watcher, p);
}