#include "rubikcube.h"
#include "cubesolver.h"
//#include <algorithm> // 放common.h中，用于 sort




// ---------------------------------------------------------
// 1. 构造函数实现
// ---------------------------------------------------------
RubikCube::RubikCube() {
	// A. 初始化位置偏移
	for (int k = 0; k < D; k++)
		for (int i = 0; i < D; i++)
			for (int j = 0; j < D; j++) {
				offset[k][i][j] = { 1.0 - k, j - 1.0, 1.0 - i };
				offset[k][i][j] *= E;
				cube[k][i][j].setOffset(offset[k][i][j]);
			}

	// B. 初始化颜色
	// 习惯：
	// R: 右面 (Right) - 红色
	// L: 左面 (Left) - 橙色
	// U: 上面 (Up) - 黄色
	// D: 下面 (Down) - 白色
	// F: 前面 (Front) - 绿色
	// B: 后面 (Back) - 蓝色
	// 0:Front 1:Right 2:Back 3:Left 4:Up 5:Down
	// 0:Front(绿色) 1:Right(红色) 2:Back(蓝色) 3:Left(橙色) 4:Up(黄色) 5:Down(白色)
	// 
	// Front (F) - Green
	for (int i = 0; i < D; i++)
		for (int j = 0; j < D; j++)
			cube[0][i][j].plane[0].setColor(RGB(0, 255, 0));

	// Right (R) - Red
	for (int i = 0; i < D; i++)
		for (int k = 0; k < D; k++)
			cube[k][i][2].plane[1].setColor(RGB(235, 0, 0));

	// Back (B) - Blue
	for (int i = 0; i < D; i++)
		for (int j = 0; j < D; j++)
			cube[2][i][j].plane[4].setColor(RGB(62, 157, 251));

	// Left (L) - Orange
	for (int i = 0; i < D; i++)
		for (int k = 0; k < D; k++)
			cube[k][i][0].plane[5].setColor(RGB(255, 128, 50));

	// Up (U) - Yellow
	for (int j = 0; j < D; j++)
		for (int k = 0; k < D; k++)
			cube[k][0][j].plane[3].setColor(RGB(255, 255, 0));

	// Down (D) - White
	for (int j = 0; j < D; j++)
		for (int k = 0; k < D; k++)
			cube[k][2][j].plane[2].setColor(RGB(235, 235, 235));

	// C. 初始化辅助映射 Map (在 cpp 中初始化更安全)
	Map = {
		&offset[1][1][0],
		&offset[0][1][1],
		&offset[1][1][2],
		&offset[2][1][1],
		&offset[1][0][1],
		&offset[1][2][1]
	};

	// D. 初始化按键映射 m
	m = {
		{'L', Left}, {'F', Front}, {'R', Right},
		{'B', Back}, {'U', Up}, {'D', Down}
	};
}

// ---------------------------------------------------------
// 2. 绘制与整体旋转
// ---------------------------------------------------------
void RubikCube::draw() {
	vector<pair<Plane*, double>> v;
	for (int k = 0; k < D; k++)
		for (int i = 0; i < D; i++)
			for (int j = 0; j < D; j++) {
				Plane* plane = cube[k][i][j].getPlanes();
				for (int n = 0; n < 6; n++)
					v.push_back({ &plane[n] , DistToWatcher(plane[n].Center()) });
			}

	// 画家算法排序
	sort(v.begin(), v.end(), [](const pair<Plane*, double>& a, const pair<Plane*, double>& b) {
		return a.second > b.second;
		});

	int size = v.size();
	for (int i = 0; i < size; i++)
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

// ---------------------------------------------------------
// 3. 单层局部旋转实现
// ---------------------------------------------------------


//原

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
				if (acos(Cos(temp, law)) / PI * 180 >= 85) continue; // 跳过不在此层的方块

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

// ---------------------------------------------------------
// 新增: 教学模式视觉初始化——仅保留中心块颜色，其他块置为深灰
// ---------------------------------------------------------
void RubikCube::setCentersColoredOnly() {
	// 深灰背景色（与画面背景区分，但视觉上未上色）
	COLORREF dark = RGB(70, 70, 70);
	// 先把所有小块的所有面设为深灰
	for (int k = 0; k < D; k++)
		for (int i = 0; i < D; i++)
			for (int j = 0; j < D; j++) {
				for (int n = 0; n < 6; n++)
					cube[k][i][j].plane[n].setColor(dark);
			}

	// 只把 6 个中心面板恢复为其标准颜色（与构造函数颜色一致）
	// Front center
	cube[0][1][1].plane[0].setColor(RGB(0, 255, 0));
	// Right center
	cube[1][1][2].plane[1].setColor(RGB(235, 0, 0));
	// Back center
	cube[2][1][1].plane[4].setColor(RGB(62, 157, 251));
	// Left center
	cube[1][1][0].plane[5].setColor(RGB(255, 128, 50));
	// Up center
	cube[1][0][1].plane[3].setColor(RGB(255, 255, 0));
	// Down center
	cube[1][2][1].plane[2].setColor(RGB(235, 235, 235));
}


// ---------------------------------------------------------
// 4. 动画与命令系统
// ---------------------------------------------------------
void RubikCube::rotate_Local(Dir dir, bool isRev) {
	if (RotateDir == Still) {
		RotateDir = dir;
		isReverse = isRev;
		// 开始新一次局部旋转，重置实例动画进度
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
		// 完成最后一步矫正
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
		// 直接改变3D魔方轴映射
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

// ---------------------------------------------------------
// 新增: 在练习模式时绘制测试坐标轴，用于标注 U D F B R L 面
// 调用时请确保在 pRubikcube->draw() 之后调用以便覆盖在顶层
// ---------------------------------------------------------
void RubikCube::drawTestAxes() {
	// cubesolver index -> Map index 映射（由代码和构造初始化推导）
	int cs_to_map[6] = { 1, 0, 4, 5, 2, 3 };

	// 要显示的字母与对应 cubesolver 索引
	vector<pair<char, int>> labels = {
		{'U', 2}, {'D', 3}, {'F', 0}, {'B', 5}, {'R', 4}, {'L', 1}
	};

	// 颜色映射（与构造函数中设置的颜色对应）
	auto colorOf = [](char c) -> COLORREF {
		switch (c) {
		case 'U': return RGB(255, 255, 0);   // Yellow
		case 'D': return RGB(235, 235, 235); // White
		case 'F': return RGB(0, 255, 0);     // Green
		case 'B': return RGB(62, 157, 251);  // Blue
		case 'R': return RGB(235, 0, 0);     // Red
		case 'L': return RGB(255, 128, 50);  // Orange
		default:  return RGB(255, 255, 255);
		}
	};

	// 屏幕投影时的辅助零点（trans 需要一个 offset 指针）
	Point zero = { 0,0,0 };

	// 魔方中心（用来算法线方向）
	Point centerCube = offset[1][1][1];

	// 设置较粗的线宽
	setlinestyle(PS_SOLID, 3, NULL, 0);

	for (auto &p : labels) {
		char ch = p.first;
		int csIdx = p.second;
		int mapIdx = cs_to_map[csIdx];

		// face center 在 world 空间
		Point faceCenter = *Map[mapIdx];

		// 法线方向近似为 (faceCenter - cubeCenter)
		Point dir = faceCenter - centerCube;
		double d = sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
		if (d < 1e-6) continue;

		// 计算起点（位于面外侧，避免轴线从魔方内部向外画）
		double startOffset = 0.35 * E; // 从面外侧开始
		Point start = faceCenter + dir * (startOffset / d);

		// axis 长度（以 world 单位为准）
		double axisLen = E * 1.6; // 更长的轴
		Point tip = faceCenter + dir * (axisLen / d);

		// 投影到屏幕坐标
		POINT ps = start.trans(&zero);
		POINT pt = tip.trans(&zero);

		// 绘制轴线与箭头/文字
		COLORREF baseCol = colorOf(ch);
		// 混合颜色使轴线和箭头更突出（与面色形成对比）
		COLORREF blended = RGB((GetRValue(baseCol) + 190) / 2, (GetGValue(baseCol) + 190) / 2, (GetBValue(baseCol) + 190) / 2);
		setlinecolor(blended);
		line(ps.x, ps.y, pt.x, pt.y);

		// 文字（偏移一点以免与线重合）
		settextcolor(blended);
		setbkmode(TRANSPARENT);
		std::string s(1, ch);
		outtextxy(pt.x + 8, pt.y + 8, s.c_str());

		// 绘制箭头（在屏幕空间构造一个小三角形）
		double dx = pt.x - ps.x;
		double dy = pt.y - ps.y;
		double dist = sqrt(dx*dx + dy*dy);
		if (dist > 1.0) {
			double ux = dx / dist;
			double uy = dy / dist;
			double arrowLen = min(24.0, dist * 0.15);
			double arrowW = arrowLen * 0.6;
			POINT base = { (LONG)(pt.x - ux * arrowLen), (LONG)(pt.y - uy * arrowLen) };
			// perpendicular
			double px = -uy, py = ux;
			POINT p1 = { pt.x, pt.y };
			POINT p2 = { (LONG)(base.x + px * arrowW), (LONG)(base.y + py * arrowW) };
			POINT p3 = { (LONG)(base.x - px * arrowW), (LONG)(base.y - py * arrowW) };
			POINT tri[3] = { p1, p2, p3 };
			setfillcolor(blended);
			fillpolygon(tri, 3);
		}
	}

	// 恢复默认线宽
	setlinestyle(PS_SOLID, 1, NULL, 0);
}

bool RubikCube::pickStickerAtScreen(int mx, int my, int &out_k, int &out_i, int &out_j, int &out_planeIndex) {
	// 寻找所有面中包含点 (mx,my) 的项，选择距离观察者最近的（DistToWatcher 值最小）
	double bestDepth = 1e12;
	int bk=-1, bi=-1, bj=-1, bp=-1;
	for (int k = 0; k < D; ++k) {
		for (int i = 0; i < D; ++i) {
			for (int j = 0; j < D; ++j) {
				Plane* planes = cube[k][i][j].getPlanes();
				for (int p = 0; p < 6; ++p) {
					if (planes[p].containsScreenPoint(mx, my, &offset[k][i][j])) {
						double depth = DistToWatcher(planes[p].Center());
						// 选择最近的（最小距离）
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





// 新增：设置可视贴纸颜色（不修改逻辑 Cube）
void RubikCube::setStickerColorVisual(int k, int i, int j, int planeIndex, COLORREF col) {
	if (k<0||k>=D||i<0||i>=D||j<0||j>=D||planeIndex<0||planeIndex>=6) return;
	Plane* planes = cube[k][i][j].getPlanes();
	planes[planeIndex].setColor(col);
}

// 新增：把贴纸映射到逻辑面与格子
//v0
/*
bool RubikCube::mapStickerToFace(int k, int i, int j, int planeIndex, int &outFace, int &outR, int &outC) {
	// plane index mapping to cubesolver face
	// plane 0: Front (F=0)
	// plane 1: Right (R=4)
	// plane 2: Down  (D=3)
	// plane 3: Up    (U=2)
	// plane 4: Back  (B=5)
	// plane 5: Left  (L=1)
	switch (planeIndex) {
	case 0: outFace = F; outR = i; outC = j; return true; // Front
	case 1: outFace = R; outR = i; outC = 2 - k; return true; // Right
	case 4: outFace = B; outR = i; outC = 2 - j; return true; // Back
	case 5: outFace = L; outR = i; outC = k; return true; // Left
	case 3: outFace = U; outR = j; outC = k; return true; // Up
	case 2: outFace = D; outR = 2 - j; outC = k; return true; // Down
	default: return false;
	}
}
*/


//v1
/*
bool RubikCube::mapStickerToFace(int k, int i, int j, int p, int& face, int& r, int& c) const {
	// p 是鼠标点击到的 plane 编号 (0~5)
	// 根据你的渲染代码，反推 r(行) 和 c(列)

	switch (p) {
	case 0: // Front 面 (前面)
		face = F;
		r = i;
		c = j;
		return true;

	case 1: // Right 面 (右面)
		face = R;
		r = i;
		// 【关键纠正】匹配渲染时的 srcCube[R][r][2 - k]
		c = 2 - k;
		return true;

	case 2: // Back 面 (后面)
		face = B;
		r = i;
		// 从背面看，物理上的右侧(j=2)其实是背面的左侧(c=0)
		c = 2 - j;
		return true;

	case 3: // Left 面 (左面)
		face = L;
		r = i;
		// 从左面看，物理上的靠后(k=2)是左面的左侧(c=0)
		c = k;
		return true;

	case 4: // Up 面 (顶面)
		face = U;
		// 顶面俯视：靠后(k=2)是第0行，靠前(k=0)是第2行
		r = 2 - k;
		c = j;
		return true;

	case 5: // Down 面 (底面)
		face = D;
		// 底面仰视：靠前(k=0)是第0行，靠后(k=2)是第2行
		r = k;
		c = j;
		return true;

	default:
		return false;
	}
}
*/




// ---------------------------------------------------------
// 【修正版 v4】模块化独立调整
// ---------------------------------------------------------
bool RubikCube::mapStickerToFace(int k, int i, int j, int planeIndex, int& outFace, int& outR, int& outC) {
	switch (planeIndex) {
		// =========================================================
		// 1. Front (F) - Green - plane 0
		// =========================================================
	case 0:
		outFace = F;
		// 先尝试这个组合，如果不对再换
		outR = i;
		outC = j;
		return true;

		// =========================================================
		// 2. Right (R) - Red - plane 1
		// =========================================================
	case 1:
		outFace = R;
		outR = i;
		outC = k;
		return true;

		// =========================================================
		// 3. Back (B) - Blue - plane 4
		// =========================================================
	case 4:
		outFace = B;
		outR = i;
		outC = 2 - j;
		return true;

		// =========================================================
		// 4. Left (L) - Orange - plane 5
		// =========================================================
	case 5:
		outFace = L;
		outR = i;
		outC = 2-k;
		return true;

		// =========================================================
		// 5. Up (U) - Yellow - plane 3
		// =========================================================
	case 3:
		outFace = U;
		outR = 2-k;
		outC = j;
		return true;

		// =========================================================
		// 6. Down (D) - White - plane 2
		// =========================================================
	case 2:
		outFace = D;
		outR = k;
		outC = j;
		return true;

	default:
		return false;
	}
}














//修卡死
//v0
/*
// ---------------------------------------------------------
// 新增: 根据逻辑数组重绘所有颜色 (教学模式专用)
// 前提: RubikCube 必须处于初始未旋转状态!
// ---------------------------------------------------------
void RubikCube::recolorFromLogicalCube(char srcCube[6][3][3]) {
	// 辅助 lambda: 把字符颜色转为 RGB
	auto charToColor = [](char c) -> COLORREF {
		switch (c) {
		case 'G': return RGB(0, 255, 0);       // Front (Green)
		case 'R': return RGB(235, 0, 0);       // Right (Red)
		case 'B': return RGB(62, 157, 251);    // Back (Blue)
		case 'O': return RGB(255, 128, 50);    // Left (Orange)
		case 'Y': return RGB(255, 255, 0);     // Up (Yellow)
		case 'W': return RGB(235, 235, 235);   // Down (White)
		default:  return RGB(70, 70, 70);       // 未上色 (深灰)
		}
		};

	// 遍历逻辑数组的 6 个面，根据 mapStickerToFace 的逆映射，设置物理贴纸颜色
	// 我们需要手动做一个反向映射：(Face, r, c) -> (k, i, j, planeIndex)

	// 1. Front (F) -> plane 0
	for (int r = 0; r < 3; r++)
		for (int c = 0; c < 3; c++)
			cube[0][r][c].plane[0].setColor(charToColor(srcCube[F][r][c]));

	// 2. Right (R) -> plane 1
	for (int r = 0; r < 3; r++)
		for (int k = 0; k < 3; k++)
			cube[k][r][2].plane[1].setColor(charToColor(srcCube[R][r][2 - k]));

	// 3. Back (B) -> plane 4
	for (int r = 0; r < 3; r++)
		for (int c = 0; c < 3; c++)
			cube[2][r][c].plane[4].setColor(charToColor(srcCube[B][r][2 - c]));

	// 4. Left (L) -> plane 5
	for (int r = 0; r < 3; r++)
		for (int k = 0; k < 3; k++)
			cube[k][r][0].plane[5].setColor(charToColor(srcCube[L][r][k]));

	// 5. Up (U) -> plane 3
	for (int c = 0; c < 3; c++)
		for (int k = 0; k < 3; k++)
			cube[k][0][c].plane[3].setColor(charToColor(srcCube[U][c][k]));

	// 6. Down (D) -> plane 2
	for (int c = 0; c < 3; c++)
		for (int k = 0; k < 3; k++)
			cube[k][2][c].plane[2].setColor(charToColor(srcCube[D][2 - c][k]));
}

*/

//v1

/*
// ---------------------------------------------------------
// 修正版：根据逻辑数组重绘所有颜色 (教学模式专用)
// 严格对应 mapStickerToFace 的逆映射
// ---------------------------------------------------------
void RubikCube::recolorFromLogicalCube(char srcCube[6][3][3]) {
	// 辅助 lambda: 把字符颜色转为 RGB
	auto charToColor = [](char c) -> COLORREF {
		switch (c) {
		case 'G': return RGB(0, 255, 0);       // Front (Green)
		case 'R': return RGB(235, 0, 0);       // Right (Red)
		case 'B': return RGB(62, 157, 251);    // Back (Blue)
		case 'O': return RGB(255, 128, 50);    // Left (Orange)
		case 'Y': return RGB(255, 255, 0);     // Up (Yellow)
		case 'W': return RGB(235, 235, 235);   // Down (White)
		default:  return RGB(70, 70, 70);       // 未上色 (深灰)
		}
		};

	// =========================================================
	// 1. Front (F) - plane 0
	// mapStickerToFace: outFace=F, outR=i, outC=j
	// =========================================================
	for (int r = 0; r < 3; r++)
		for (int c = 0; c < 3; c++)
			cube[0][r][c].plane[0].setColor(charToColor(srcCube[F][r][c]));

	// =========================================================
	// 2. Right (R) - plane 1
	// mapStickerToFace: outFace=R, outR=i, outC=2-k
	// =========================================================
	for (int r = 0; r < 3; r++)
		for (int k = 0; k < 3; k++)
			cube[k][r][2].plane[1].setColor(charToColor(srcCube[R][r][2 - k]));

	// =========================================================
	// 3. Back (B) - plane 4
	// mapStickerToFace: outFace=B, outR=i, outC=2-j
	// =========================================================
	for (int r = 0; r < 3; r++)
		for (int j = 0; j < 3; j++)
			cube[2][r][j].plane[4].setColor(charToColor(srcCube[B][r][2 - j]));

	// =========================================================
	// 4. Left (L) - plane 5
	// mapStickerToFace: outFace=L, outR=i, outC=k
	// =========================================================
	for (int r = 0; r < 3; r++)
		for (int k = 0; k < 3; k++)
			cube[k][r][0].plane[5].setColor(charToColor(srcCube[L][r][k]));

	// =========================================================
	// 5. Up (U) - plane 3
	// mapStickerToFace: outFace=U, outR=j, outC=k
	// =========================================================
	for (int j = 0; j < 3; j++)
		for (int k = 0; k < 3; k++)
			cube[k][0][j].plane[3].setColor(charToColor(srcCube[U][j][k]));

	// =========================================================
	// 6. Down (D) - plane 2 (【关键修正】这里之前写错了！)
	// mapStickerToFace: outFace=D, outR=2-j, outC=k
	// =========================================================
	for (int j = 0; j < 3; j++)
		for (int k = 0; k < 3; k++)
			cube[k][2][j].plane[2].setColor(charToColor(srcCube[D][2 - j][k]));
}
*/















// ---------------------------------------------------------
// 【修正版 v2】根据逻辑数组重绘所有颜色
// 目标：让屏幕视觉显示与终端打印的 Cube[face][row][col] 完全一致
// ---------------------------------------------------------
void RubikCube::recolorFromLogicalCube(char srcCube[6][3][3]) {
	// 辅助 lambda: 把字符颜色转为 RGB
	auto charToColor = [](char c) -> COLORREF {
		switch (c) {
		case 'G': return RGB(0, 255, 0);       // Front (Green)
		case 'R': return RGB(235, 0, 0);       // Right (Red)
		case 'B': return RGB(62, 157, 251);    // Back (Blue)
		case 'O': return RGB(255, 128, 50);    // Left (Orange)
		case 'Y': return RGB(255, 255, 0);     // Up (Yellow)
		case 'W': return RGB(235, 235, 235);   // Down (White)
		default:  return RGB(70, 70, 70);       // 未上色 (深灰)
		}
		};

	// =========================================================
	// 1. Front (F) - Green - plane 0
	// 对应 cube[0][i][j].plane[0]
	// =========================================================
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			cube[0][i][j].plane[0].setColor(charToColor(srcCube[F][i][j]));

	// =========================================================
	// 2. Right (R) - Red - plane 1
	// 对应 cube[k][i][2].plane[1]
	// 【修正】根据你的反馈，这里直接按 k 作为列，不再取反
	// =========================================================
	for (int i = 0; i < 3; i++)      // i 是行
		for (int k = 0; k < 3; k++)  // k 是列
			cube[k][i][2].plane[1].setColor(charToColor(srcCube[R][i][k]));

	// =========================================================
	// 3. Back (B) - Blue - plane 4
	// 对应 cube[2][i][j].plane[4]
	// =========================================================
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			cube[2][i][j].plane[4].setColor(charToColor(srcCube[B][i][j]));

	// =========================================================
	// 4. Left (L) - Orange - plane 5
	// 对应 cube[k][i][0].plane[5]
	// =========================================================
	for (int i = 0; i < 3; i++)
		for (int k = 0; k < 3; k++)
			cube[k][i][0].plane[5].setColor(charToColor(srcCube[L][i][k]));

	// =========================================================
	// 5. Up (U) - Yellow - plane 3
	// 对应 cube[k][0][j].plane[3]
	// 【修正】根据你的反馈 (OOO/YYY/YYY -> OYY/OYY/OYY)
	// 这里做了转置处理：视觉行 = 逻辑列，视觉列 = 逻辑行
	// =========================================================
	for (int j = 0; j < 3; j++)      // j 是逻辑行，现在作为视觉列
		for (int k = 0; k < 3; k++)  // k 是逻辑列，现在作为视觉行
			cube[k][0][j].plane[3].setColor(charToColor(srcCube[U][j][k]));

	// =========================================================
	// 6. Down (D) - White - plane 2
	// 对应 cube[k][2][j].plane[2]
	// 【修正】根据你的反馈 (WWW/WWW/RRR -> WWR/WWR/WWR)
	// 这里做了转置处理
	// =========================================================
	for (int j = 0; j < 3; j++)
		for (int k = 0; k < 3; k++)
			cube[k][2][j].plane[2].setColor(charToColor(srcCube[D][j][k]));
}


void RubikCube::resetMap() {
	// 恢复为构造函数中的初始映射
	Map = {
		&offset[1][1][0],
		&offset[0][1][1],
		&offset[1][1][2],
		&offset[2][1][1],
		&offset[1][0][1],
		&offset[1][2][1]
	};
}