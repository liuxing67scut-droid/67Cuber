#include "rubikcube.h"
#include "cubesolver.h"
#include "cube_colors.h"

namespace {
double pointLength(const Point& p) {
	return sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
}

Point normalizePoint(const Point& p) {
	double len = pointLength(p);
	if (len < 1e-6) {
		return { 0, 0, 0 };
	}

	return p / len;
}

double dotPoint(const Point& a, const Point& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

int classifyLayerByProjection(double projection) {
	const double threshold = 50.0;
	if (projection < -threshold) {
		return 0;
	}
	if (projection > threshold) {
		return 2;
	}
	return 1;
}

int rowPositiveFaceOf(int face) {
	if (face == U) {
		return F;
	}
	if (face == D) {
		return B;
	}
	return D;
}

int colPositiveFaceOf(int face) {
	if (face == B) {
		return L;
	}
	if (face == R) {
		return B;
	}
	if (face == L) {
		return F;
	}
	return R;
}
}

RubikCube::RubikCube() {
	//初始化小方块位置
	for (int k = 0; k < D; k++)
		for (int i = 0; i < D; i++)
			for (int j = 0; j < D; j++) {
				offset[k][i][j] = { 1.0 - k, j - 1.0, 1.0 - i };
				offset[k][i][j] *= E;
				cube[k][i][j].setOffset(offset[k][i][j]);
			}

	//面颜色约定：F蓝 R红 B绿 L橙 U黄 D白
	for (int i = 0; i < D; i++)
		for (int j = 0; j < D; j++)
			cube[0][i][j].plane[0].setColor(cubeFaceColorToRgb(F));

	for (int i = 0; i < D; i++)
		for (int k = 0; k < D; k++)
			cube[k][i][2].plane[1].setColor(cubeFaceColorToRgb(R));

	for (int i = 0; i < D; i++)
		for (int j = 0; j < D; j++)
			cube[2][i][j].plane[4].setColor(cubeFaceColorToRgb(B));

	for (int i = 0; i < D; i++)
		for (int k = 0; k < D; k++)
			cube[k][i][0].plane[5].setColor(cubeFaceColorToRgb(L));

	for (int j = 0; j < D; j++)
		for (int k = 0; k < D; k++)
			cube[k][0][j].plane[3].setColor(cubeFaceColorToRgb(U));

	for (int j = 0; j < D; j++)
		for (int k = 0; k < D; k++)
			cube[k][2][j].plane[2].setColor(cubeFaceColorToRgb(D));

	//当前可视方向到中心块偏移的映射
	Map = {
		&offset[1][1][0],
		&offset[0][1][1],
		&offset[1][1][2],
		&offset[2][1][1],
		&offset[1][0][1],
		&offset[1][2][1]
	};

	//公式字母到旋转方向的映射
	m = {
		{'L', Left}, {'F', Front}, {'R', Right},
		{'B', Back}, {'U', Up}, {'D', Down}
	};
}

void RubikCube::draw() {
	vector<pair<Plane*, double>> v;
	for (int k = 0; k < D; k++)
		for (int i = 0; i < D; i++)
			for (int j = 0; j < D; j++) {
				Plane* plane = cube[k][i][j].getPlanes();
				for (int n = 0; n < 6; n++)
					v.push_back({ &plane[n] , DistToWatcher(plane[n].Center()) });
			}

	//画家算法按距离从远到近绘制
	sort(v.begin(), v.end(), [](const pair<Plane*, double>& a, const pair<Plane*, double>& b) {
		return a.second > b.second;
		});

	size_t size = v.size();
	for (size_t i = 0; i < size; i++)
		v[i].first->draw();
}

void RubikCube::rotateX(double degree) {
	for (int k = 0; k < D; k++)
		for (int i = 0; i < D; i++)
			for (int j = 0; j < D; j++) {
				Cube::rotateX(offset[k][i][j], degree);
				cube[k][i][j].setOffset(offset[k][i][j]);
				cube[k][i][j].rotateX(degree);
			}
}

void RubikCube::rotateY(double degree) {
	for (int k = 0; k < D; k++)
		for (int i = 0; i < D; i++)
			for (int j = 0; j < D; j++) {
				Cube::rotateY(offset[k][i][j], degree);
				cube[k][i][j].setOffset(offset[k][i][j]);
				cube[k][i][j].rotateY(degree);
			}
}

void RubikCube::rotateZ(double degree) {
	for (int k = 0; k < D; k++)
		for (int i = 0; i < D; i++)
			for (int j = 0; j < D; j++) {
				Cube::rotateZ(offset[k][i][j], degree);
				cube[k][i][j].setOffset(offset[k][i][j]);
				cube[k][i][j].rotateZ(degree);
			}
}



void RubikCube::rotateX_Local(double degree, string dir) {
	const Point& A = offset[1][1][1];
	const Point& B = *Map[1];
	Point law = B - A;

	if (dir == "back") {
		law = -law;
		degree = -degree;
	}

	for (int k = 0; k < D; k++)
		for (int i = 0; i < D; i++)
			for (int j = 0; j < D; j++) {
				Point temp = offset[k][i][j] - offset[1][1][1];
				if (acos(Cos(temp, law)) / PI * 180 >= 85) continue; //跳过不在目标层的小方块

				offset[k][i][j] = Cube::rotateByVector(offset[k][i][j], A, B, degree);
				cube[k][i][j].setOffset(offset[k][i][j]);
				cube[k][i][j].rotateX_Local(degree, A, B, true);
			}
}

void RubikCube::rotateY_Local(double degree, string dir) {
	const Point& A = offset[1][1][1];
	const Point& B = *Map[2];
	Point law = B - A;

	if (dir == "left") {
		law = -law;
		degree = -degree;
	}

	for (int k = 0; k < D; k++)
		for (int i = 0; i < D; i++)
			for (int j = 0; j < D; j++) {
				Point temp = offset[k][i][j] - offset[1][1][1];
				if (acos(Cos(temp, law)) / PI * 180 >= 85) continue;

				offset[k][i][j] = Cube::rotateByVector(offset[k][i][j], A, B, degree);
				cube[k][i][j].setOffset(offset[k][i][j]);
				cube[k][i][j].rotateY_Local(degree, A, B, true);
			}
}

void RubikCube::rotateZ_Local(double degree, string dir) {
	const Point& A = offset[1][1][1];
	const Point& B = *Map[4];
	Point law = B - A;

	if (dir == "down") {
		law = -law;
		degree = -degree;
	}

	for (int k = 0; k < D; k++)
		for (int i = 0; i < D; i++)
			for (int j = 0; j < D; j++) {
				Point temp = offset[k][i][j] - offset[1][1][1];
				if (acos(Cos(temp, law)) / PI * 180 >= 85) continue;

				offset[k][i][j] = Cube::rotateByVector(offset[k][i][j], A, B, degree);
				cube[k][i][j].setOffset(offset[k][i][j]);
				cube[k][i][j].rotateZ_Local(degree, A, B, true);
			}
}

//教学模式初始状态只显示中心色
void RubikCube::setCentersColoredOnly() {
	//未上色贴纸使用深灰
	COLORREF dark = RGB(70, 70, 70);
	for (int k = 0; k < D; k++)
		for (int i = 0; i < D; i++)
			for (int j = 0; j < D; j++) {
				for (int n = 0; n < 6; n++)
					cube[k][i][j].plane[n].setColor(dark);
			}

	//恢复六个中心块的标准色
	cube[0][1][1].plane[0].setColor(cubeFaceColorToRgb(F));
	cube[1][1][2].plane[1].setColor(cubeFaceColorToRgb(R));
	cube[2][1][1].plane[4].setColor(cubeFaceColorToRgb(B));
	cube[1][1][0].plane[5].setColor(cubeFaceColorToRgb(L));
	cube[1][0][1].plane[3].setColor(cubeFaceColorToRgb(U));
	cube[1][2][1].plane[2].setColor(cubeFaceColorToRgb(D));
}


void RubikCube::rotate_Local(Dir dir, bool isRev) {
	if (RotateDir == Still) {
		RotateDir = dir;
		isReverse = isRev;
		//开始一次新的局部旋转
		animDegree = 0;
	}
}

void RubikCube::updateRotate() {
	if (RotateDir == Still) return;

	void (RubikCube:: * func)(double, string) = nullptr;
	string dir;

	switch (RotateDir) {
	case Front: func = &RubikCube::rotateX_Local; dir = "front"; break;
	case Back:  func = &RubikCube::rotateX_Local; dir = "back"; break;
	case Right: func = &RubikCube::rotateY_Local; dir = "right"; break;
	case Left:  func = &RubikCube::rotateY_Local; dir = "left"; break;
	case Up:    func = &RubikCube::rotateZ_Local; dir = "up"; break;
	case Down:  func = &RubikCube::rotateZ_Local; dir = "down"; break;
	}

	if (func == nullptr) return;

	int rev = isReverse ? -1 : 1;

	if (animDegree < PI / 2) {
		animDegree += step;
		(this->*func)(rev * step, dir);
	}
	else {
		//补齐最后一小段角度，避免累计误差
		(this->*func)(rev * (PI / 2 - animDegree), dir);
		animDegree = 0;
		RotateDir = Still;
	}
}

bool RubikCube::isRotateOver() {
	return RotateDir == Still;
}

void RubikCube::setRotateStep(double Step) {
	step = Step;
}

void RubikCube::bindCommand(const string& cmd) {
	iss.clear();
	iss.str(cmd);
}

void RubikCube::execute() {
	if (!iss.good() || !isRotateOver()) return;

	string cmd;
	iss >> cmd;

	if (cmd == "→") {
		//整体转动只改变可视轴映射
		Point* t = Map[5];
		Map[5] = Map[2];
		Map[2] = Map[4];
		Map[4] = Map[0];
		Map[0] = t;
	}
	else if (cmd == "↓") {
		Point* t = Map[3];
		Map[3] = Map[5];
		Map[5] = Map[1];
		Map[1] = Map[4];
		Map[4] = t;
	}
	else {
		if (m.find(cmd[0]) != m.end()) {
			rotate_Local(m[cmd[0]], cmd[1] == '\'' ? true : false);
		}
	}
}

bool RubikCube::isExecuteOver() {
	return !iss.good() && isRotateOver();
}

//练习模式绘制 U/D/F/B/R/L 方向标识
void RubikCube::drawTestAxes() {
	//cubesolver 面编号到可视轴 Map 的映射
	int cs_to_map[6] = { 1, 0, 4, 5, 2, 3 };

	vector<pair<char, int>> labels = {
		{'U', 2}, {'D', 3}, {'F', 0}, {'B', 5}, {'R', 4}, {'L', 1}
	};

	Point zero = { 0,0,0 };
	Point centerCube = offset[1][1][1];

	//方向标识使用较粗线条
	setlinestyle(PS_SOLID, 3, NULL, 0);

	for (auto &p : labels) {
		char ch = p.first;
		int csIdx = p.second;
		int mapIdx = cs_to_map[csIdx];

		Point faceCenter = *Map[mapIdx];
		//面中心到魔方中心的方向近似为法线
		Point dir = faceCenter - centerCube;
		double d = sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
		if (d < 1e-6) continue;

		//从面外侧开始绘制，避免线条穿过魔方内部
		double startOffset = 0.35 * E;
		Point start = faceCenter + dir * (startOffset / d);

		double axisLen = E * 1.6;
		Point tip = faceCenter + dir * (axisLen / d);

		POINT ps = start.trans(&zero);
		POINT pt = tip.trans(&zero);

		COLORREF baseCol = cubeFaceColorToRgb(csIdx);
		//提亮颜色，避免与面色混在一起
		COLORREF blended = RGB((GetRValue(baseCol) + 190) / 2, (GetGValue(baseCol) + 190) / 2, (GetBValue(baseCol) + 190) / 2);
		setlinecolor(blended);
		line(ps.x, ps.y, pt.x, pt.y);

		//文字略微偏移，避免压在线上
		setbkmode(TRANSPARENT);
		std::string s(1, ch);
		int textX = pt.x + 8;
		int textY = pt.y + 8;
		settextcolor(RGB(58, 32, 20));
		outtextxy(textX - 1, textY, s.c_str());
		outtextxy(textX + 1, textY, s.c_str());
		outtextxy(textX, textY - 1, s.c_str());
		outtextxy(textX, textY + 1, s.c_str());
		settextcolor(blended);
		outtextxy(textX, textY, s.c_str());

		//屏幕空间构造箭头三角形
		double dx = pt.x - ps.x;
		double dy = pt.y - ps.y;
		double dist = sqrt(dx*dx + dy*dy);
		if (dist > 1.0) {
			double ux = dx / dist;
			double uy = dy / dist;
			double arrowLen = min(24.0, dist * 0.15);
			double arrowW = arrowLen * 0.6;
			POINT base = { (LONG)(pt.x - ux * arrowLen), (LONG)(pt.y - uy * arrowLen) };
			//垂直方向
			double px = -uy, py = ux;
			POINT p1 = { pt.x, pt.y };
			POINT p2 = { (LONG)(base.x + px * arrowW), (LONG)(base.y + py * arrowW) };
			POINT p3 = { (LONG)(base.x - px * arrowW), (LONG)(base.y - py * arrowW) };
			POINT tri[3] = { p1, p2, p3 };
			setfillcolor(blended);
			fillpolygon(tri, 3);
		}
	}

	//恢复默认线宽
	setlinestyle(PS_SOLID, 1, NULL, 0);
}

bool RubikCube::pickStickerAtScreen(int mx, int my, int &out_k, int &out_i, int &out_j, int &out_planeIndex) {
	//多个面命中时选择离观察者最近的贴纸
	double bestDepth = 1e12;
	int bk = -1, bi = -1, bj = -1, bp = -1;
	for (int k = 0; k < D; ++k) {
		for (int i = 0; i < D; ++i) {
			for (int j = 0; j < D; ++j) {
				Plane* planes = cube[k][i][j].getPlanes();
				for (int p = 0; p < 6; ++p) {
					if (planes[p].containsScreenPoint(mx, my, &offset[k][i][j])) {
						double depth = DistToWatcher(planes[p].Center());
						//保留最前面的命中面
						if (depth < bestDepth) {
							bestDepth = depth;
							bk = k; bi = i; bj = j; bp = p;
						}
					}
				}
			}
		}
	}
	if (bk != -1) {
		clearAllHighlights();
		Plane* planes = cube[bk][bi][bj].getPlanes();
		planes[bp].setHighlight(true);
		out_k = bk; out_i = bi; out_j = bj; out_planeIndex = bp;
		return true;
	}
	return false;
}

void RubikCube::clearAllHighlights() {
	for (int k = 0; k < D; ++k)
		for (int i = 0; i < D; ++i)
			for (int j = 0; j < D; ++j) {
				Plane* planes = cube[k][i][j].getPlanes();
				for (int p = 0; p < 6; ++p)
					planes[p].setHighlight(false);
			}
}

bool RubikCube::getStickerScreenPolygon(int k, int i, int j, int planeIndex, POINT out[4]) const {
	if (k < 0 || k >= D || i < 0 || i >= D || j < 0 || j >= D || planeIndex < 0 || planeIndex >= 6 || out == nullptr) {
		return false;
	}

	cube[k][i][j].plane[planeIndex].getScreenPolygon(out, &offset[k][i][j]);
	return true;
}

bool RubikCube::getStickerDragGeometry(int k, int i, int j, int planeIndex, StickerDragGeometry& out) const {
	if (k < 0 || k >= D || i < 0 || i >= D || j < 0 || j >= D || planeIndex < 0 || planeIndex >= 6) {
		return false;
	}

	const Point centerCube = offset[1][1][1];
	auto directionOfFace = [&](int face) -> Point {
		int mapIndex = -1;
		if (face == F) {
			mapIndex = 1;
		}
		else if (face == L) {
			mapIndex = 0;
		}
		else if (face == U) {
			mapIndex = 4;
		}
		else if (face == D) {
			mapIndex = 5;
		}
		else if (face == R) {
			mapIndex = 2;
		}
		else if (face == B) {
			mapIndex = 3;
		}

		if (mapIndex < 0) {
			return { 0, 0, 0 };
		}
		return normalizePoint(*Map[mapIndex] - centerCube);
	};

	cube[k][i][j].plane[planeIndex].getScreenPolygon(out.screenPolygon, &offset[k][i][j]);
	out.cubieCenter = offset[k][i][j];
	out.stickerCenter = cube[k][i][j].plane[planeIndex].Center();
	out.normal = normalizePoint(out.stickerCenter - out.cubieCenter);

	const int faces[6] = { F, L, U, D, R, B };
	double bestScore = -1e9;
	int bestFace = -1;
	for (int n = 0; n < 6; ++n) {
		Point faceDir = directionOfFace(faces[n]);
		double score = dotPoint(out.normal, faceDir);
		if (score > bestScore) {
			bestScore = score;
			bestFace = faces[n];
		}
	}

	if (bestFace == -1) {
		return false;
	}

	out.currentFace = bestFace;
	out.rowAxis = directionOfFace(rowPositiveFaceOf(bestFace));
	out.colAxis = directionOfFace(colPositiveFaceOf(bestFace));

	Point rel = out.cubieCenter - centerCube;
	out.currentRow = classifyLayerByProjection(dotPoint(rel, out.rowAxis));
	out.currentCol = classifyLayerByProjection(dotPoint(rel, out.colAxis));

	return true;
}

//只改变可视贴纸颜色
void RubikCube::setStickerColorVisual(int k, int i, int j, int planeIndex, COLORREF col) {
	if (k < 0 || k >= D || i < 0 || i >= D || j < 0 || j >= D || planeIndex < 0 || planeIndex >= 6) return;
	Plane* planes = cube[k][i][j].getPlanes();
	planes[planeIndex].setColor(col);
}

//物理贴纸映射到逻辑 Cube 的面和行列
bool RubikCube::mapStickerToFace(int k, int i, int j, int planeIndex, int& outFace, int& outR, int& outC) {
	switch (planeIndex) {
	case 0:
		outFace = F;
		outR = i;
		outC = j;
		return true;

	case 1:
		outFace = R;
		outR = i;
		outC = k;
		return true;

	case 4:
		outFace = B;
		outR = i;
		outC = 2 - j;
		return true;

	case 5:
		outFace = L;
		outR = i;
		outC = 2 - k;
		return true;

	case 3:
		outFace = U;
		outR = 2 - k;
		outC = j;
		return true;

	case 2:
		outFace = D;
		outR = k;
		outC = j;
		return true;

	default:
		return false;
	}
}

//按逻辑 Cube 重绘可视贴纸颜色
void RubikCube::recolorFromLogicalCube(char srcCube[6][3][3]) {
	//前面：cube[0][i][j].plane[0]
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			cube[0][i][j].plane[0].setColor(cubeColorToRgb(srcCube[F][i][j]));

	//右面：cube[k][i][2].plane[1]
	for (int i = 0; i < 3; i++)
		for (int k = 0; k < 3; k++)
			cube[k][i][2].plane[1].setColor(cubeColorToRgb(srcCube[R][i][k]));

	//后面：cube[2][i][j].plane[4]
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			cube[2][i][j].plane[4].setColor(cubeColorToRgb(srcCube[B][i][j]));

	//左面：cube[k][i][0].plane[5]
	for (int i = 0; i < 3; i++)
		for (int k = 0; k < 3; k++)
			cube[k][i][0].plane[5].setColor(cubeColorToRgb(srcCube[L][i][k]));

	//顶面显示需要转置逻辑行列
	for (int j = 0; j < 3; j++)
		for (int k = 0; k < 3; k++)
			cube[k][0][j].plane[3].setColor(cubeColorToRgb(srcCube[U][j][k]));

	//底面显示需要转置逻辑行列
	for (int j = 0; j < 3; j++)
		for (int k = 0; k < 3; k++)
			cube[k][2][j].plane[2].setColor(cubeColorToRgb(srcCube[D][j][k]));
}

void RubikCube::resetMap() {
	//恢复初始可视轴映射
	Map = {
		&offset[1][1][0],
		&offset[0][1][1],
		&offset[1][1][2],
		&offset[2][1][1],
		&offset[1][0][1],
		&offset[1][2][1]
	};
}
