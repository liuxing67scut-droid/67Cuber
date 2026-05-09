
#include "cube.h"
//#include <algorithm> // 因为用到了 sort //放在common.h中

// ---------------------------------------------------------
// 1. 构造函数实现
// ---------------------------------------------------------
Cube::Cube() {
	// 初始化顶点数组 (C++11 之前可能需要在构造体内逐个赋值，这里直接用列表)
	// 注意：这里为了清晰，我们显式初始化 vertex，虽然头文件里已经给了默认值
	// 但为了确保 e=50 生效，我们在构造体里重新写一遍逻辑是最稳妥的，
	// 或者直接信任头文件中的 in-class initializer。
	// 这里直接执行绑定逻辑：

	// 重新初始化 vertex (确保 e 被正确使用，避免某些编译器对 in-class init 的顺序问题)
	vertex[0] = { e, -e,  e };
	vertex[1] = { e,  e,  e };
	vertex[2] = { e,  e, -e };
	vertex[3] = { e, -e, -e };
	vertex[4] = { -e, -e,  e };
	vertex[5] = { -e,  e,  e };
	vertex[6] = { -e,  e, -e };
	vertex[7] = { -e, -e, -e };

	// 绑定面
	plane[0].bindPoint(&vertex[0], &vertex[1], &vertex[2], &vertex[3]); // Front
	plane[1].bindPoint(&vertex[1], &vertex[5], &vertex[6], &vertex[2]); // Right
	plane[2].bindPoint(&vertex[3], &vertex[2], &vertex[6], &vertex[7]); // Down
	plane[3].bindPoint(&vertex[0], &vertex[1], &vertex[5], &vertex[4]); // Up
	plane[4].bindPoint(&vertex[4], &vertex[5], &vertex[6], &vertex[7]); // Back
	plane[5].bindPoint(&vertex[0], &vertex[3], &vertex[7], &vertex[4]); // Left
}

// ---------------------------------------------------------
// 2. 普通成员函数实现
// ---------------------------------------------------------
void Cube::setOffset(Point Offset) {
	offset = Offset;
	for (int i = 0; i < P_N; i++)
		plane[i].bindOffset(&offset);
}

void Cube::showCube() {
	// 画家消隐算法：按距离排序
	sort(plane, plane + P_N, [](const Plane& a, const Plane& b) { return a > b; });
	for (int i = 0; i < P_N; i++)
		plane[i].draw();
}

Plane* Cube::getPlanes() {
	return plane;
}

// ---------------------------------------------------------
// 3. 基础旋转 (成员函数 - 旋转自身顶点)
// ---------------------------------------------------------
void Cube::rotateX(double degree) {
	for (int i = 0; i < N; i++) {
		double y = vertex[i].y, z = vertex[i].z;
		double d = degree / 180 * PI; // z->y
		vertex[i].y = y * cos(d) + z * sin(d);
		vertex[i].z = z * cos(d) - y * sin(d);
	}
}

void Cube::rotateY(double degree) {
	for (int i = 0; i < N; i++) {
		double x = vertex[i].x, z = vertex[i].z;
		double d = degree / 180 * PI; // x->z
		vertex[i].z = z * cos(d) + x * sin(d);
		vertex[i].x = x * cos(d) - z * sin(d);
	}
}

void Cube::rotateZ(double degree) {
	for (int i = 0; i < N; i++) {
		double x = vertex[i].x, y = vertex[i].y;
		double d = degree / 180 * PI; // y->x
		vertex[i].x = x * cos(d) + y * sin(d);
		vertex[i].y = y * cos(d) - x * sin(d);
	}
}

// ---------------------------------------------------------
// 4. 本地旋转 (绕任意轴)
// ---------------------------------------------------------
void Cube::rotateX_Local(double degree, const Point A, const Point B, bool bUseAB) {
	Point a = vertex[4], b = vertex[0]; // 预先存值，否则循环中改变
	if (bUseAB) a = A, b = B;
	for (int i = 0; i < N; i++)
		vertex[i] = rotateByVector(vertex[i], a, b, degree);
}

void Cube::rotateY_Local(double degree, const Point A, const Point B, bool bUseAB) {
	Point a = vertex[0], b = vertex[1];
	if (bUseAB) a = A, b = B;
	for (int i = 0; i < N; i++)
		vertex[i] = rotateByVector(vertex[i], a, b, degree);
}

void Cube::rotateZ_Local(double degree, const Point A, const Point B, bool bUseAB) {
	Point a = vertex[2], b = vertex[1];
	if (bUseAB) a = A, b = B;
	for (int i = 0; i < N; i++)
		vertex[i] = rotateByVector(vertex[i], a, b, degree);
}

// ---------------------------------------------------------
// 5. 静态函数实现
// ---------------------------------------------------------
void Cube::rotateX(Point& p, double degree) {
	double y = p.y, z = p.z;
	double d = degree / 180 * PI;
	p.y = y * cos(d) + z * sin(d);
	p.z = z * cos(d) - y * sin(d);
}

void Cube::rotateY(Point& p, double degree) {
	double x = p.x, z = p.z;
	double d = degree / 180 * PI;
	p.z = z * cos(d) + x * sin(d);
	p.x = x * cos(d) - z * sin(d);
}

void Cube::rotateZ(Point& p, double degree) {
	double x = p.x, y = p.y;
	double d = degree / 180 * PI;
	p.x = x * cos(d) + y * sin(d);
	p.y = y * cos(d) - x * sin(d);
}

// 罗德里格斯旋转公式
Point Cube::rotateByVector(const Point& V, const Point& a, const Point& b, double T) {
	Point K, ans;
	K = a - b;
	K = K / Dist(a, b); // 单位化
	double x = K.x, y = K.y, z = K.z;
	double u = V.x, v = V.y, w = V.z;

	ans.x = u * cos(T) + (y * w - z * v) * sin(T) + x * (x * u + y * v + z * w) * (1 - cos(T));
	ans.y = v * cos(T) + (z * u - x * w) * sin(T) + y * (x * u + y * v + z * w) * (1 - cos(T));
	ans.z = w * cos(T) + (x * v - y * u) * sin(T) + z * (x * u + y * v + z * w) * (1 - cos(T));
	return ans;
}