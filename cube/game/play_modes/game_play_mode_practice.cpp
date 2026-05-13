#include "game_internal.h"

#include <cmath>
#include <iostream>

struct DragVec2 {
	double x = 0.0;
	double y = 0.0;
};

struct PracticeDragMoveDebug {
	bool ok = false;
	char axis = '?';
	char sign = '?';
	bool useRow = true;
	double dragLength = 0.0;
	double scoreA = 0.0;
	double scoreB = 0.0;
	char move = '?';
	bool rev = false;
	const char* reason = "";
};

static void resetPracticeRightDrag(MainViewState& viewState) {
	viewState.practiceRightDragActive = false;
	viewState.practiceRightDragPreviewVisible = false;
	viewState.practiceCurrentFace = -1;
	viewState.practiceCurrentRow = -1;
	viewState.practiceCurrentCol = -1;
	viewState.practiceCurrentRowAxis = { 0, 0 };
	viewState.practiceCurrentColAxis = { 0, 0 };

	if (g_app.pRubikcube) {
		g_app.pRubikcube->clearAllHighlights();
	}
}

static POINT toWindowPoint(const POINT& p) {
	return { p.x + Width / 2, p.y + Height / 2 };
}

static void drawCenteredLine(double cx, double cy, double vx, double vy, double halfLen) {
	double len = std::sqrt(vx * vx + vy * vy);
	if (len < 1.0) {
		return;
	}

	double ux = vx / len;
	double uy = vy / len;
	line(
		(LONG)(cx - ux * halfLen),
		(LONG)(cy - uy * halfLen),
		(LONG)(cx + ux * halfLen),
		(LONG)(cy + uy * halfLen)
	);
}

static double vecLength(DragVec2 v) {
	return std::sqrt(v.x * v.x + v.y * v.y);
}

static bool normalizeVec(DragVec2& v) {
	double len = vecLength(v);
	if (len < 1.0) {
		return false;
	}

	v.x /= len;
	v.y /= len;
	return true;
}

static double dotVec(DragVec2 a, DragVec2 b) {
	return a.x * b.x + a.y * b.y;
}

static DragVec2 pointToVec(POINT p) {
	return { (double)p.x, (double)p.y };
}

static POINT projectAxisToScreenVector(const Point& center, const Point& axis) {
	Point zero = { 0, 0, 0 };
	POINT p0 = center.trans(&zero);
	POINT p1 = (center + axis * 100.0).trans(&zero);
	return { p1.x - p0.x, p1.y - p0.y };
}

static char faceName(int face) {
	if (face == F) return 'F';
	if (face == B) return 'B';
	if (face == L) return 'L';
	if (face == R) return 'R';
	if (face == U) return 'U';
	if (face == D) return 'D';
	return '?';
}

static bool mapDragRowToMove(int face, int row, int& outMove) {
	if (row == 1) {
		return false;
	}

	if (face == F || face == B || face == R || face == L) {
		outMove = (row == 0) ? U : D;
		return true;
	}

	if (face == U) {
		outMove = (row == 0) ? B : F;
		return true;
	}

	if (face == D) {
		outMove = (row == 0) ? F : B;
		return true;
	}

	return false;
}

static bool mapDragColToMove(int face, int col, int& outMove) {
	if (col == 1) {
		return false;
	}

	if (face == F || face == U || face == D) {
		outMove = (col == 0) ? L : R;
		return true;
	}

	if (face == B) {
		outMove = (col == 0) ? R : L;
		return true;
	}

	if (face == R) {
		outMove = (col == 0) ? F : B;
		return true;
	}

	if (face == L) {
		outMove = (col == 0) ? B : F;
		return true;
	}

	return false;
}

static bool isPracticeDragReverse(int face, int move, char sign) {
	bool axisPositive = (sign == '+');
	bool positiveMeansPrime = false;
	bool calibrated = true;

	if (face == F) {
		positiveMeansPrime = (move == U || move == R);
	}
	else if (face == U) {
		positiveMeansPrime = (move == B || move == L);
	}
	else if (face == R) {
		positiveMeansPrime = (move == U || move == B);
	}
	else if (face == B) {
		positiveMeansPrime = (move == D || move == L);
	}
	else if (face == L) {
		positiveMeansPrime = (move == D || move == F);
	}
	else if (face == D) {
		positiveMeansPrime = (move == F || move == R);
	}
	else {
		calibrated = false;
	}

	if (!calibrated) {
		return sign == '-';
	}

	return axisPositive ? positiveMeansPrime : !positiveMeansPrime;
}

static char correctedCurrentDragSign(int currentFace, bool useRow, char sign) {
	bool invert = false;

	if (currentFace == U && !useRow) {
		invert = true;
	}
	else if ((currentFace == B || currentFace == L) && useRow) {
		invert = true;
	}

	if (!invert) {
		return sign;
	}
	return sign == '+' ? '-' : '+';
}

static bool inferPracticeCurrentDragMove(
	int currentFace,
	int currentRow,
	int currentCol,
	POINT rowAxisScreen,
	POINT colAxisScreen,
	int startX,
	int startY,
	int endX,
	int endY,
	PracticeDragMoveDebug& out
) {
	const double minDragLength = 25.0;
	DragVec2 drag = { (double)(endX - startX), (double)(endY - startY) };
	out.dragLength = vecLength(drag);
	if (out.dragLength < minDragLength) {
		out.reason = "drag too short";
		return false;
	}

	if (!normalizeVec(drag)) {
		out.reason = "bad drag vector";
		return false;
	}

	DragVec2 rowAxis = pointToVec(rowAxisScreen);
	DragVec2 colAxis = pointToVec(colAxisScreen);
	if (!normalizeVec(rowAxis) || !normalizeVec(colAxis)) {
		out.reason = "bad current axis";
		return false;
	}

	double dotRow = dotVec(drag, rowAxis);
	double dotCol = dotVec(drag, colAxis);
	out.scoreA = std::fabs(dotRow);
	out.scoreB = std::fabs(dotCol);

	bool closerToRowAxis = (out.scoreA >= out.scoreB);
	//拖拽沿行方向移动时，实际是在选择某一列；沿列方向移动时，实际是在选择某一行。
	out.useRow = !closerToRowAxis;
	out.axis = closerToRowAxis ? 'R' : 'C';
	char axisSign = (closerToRowAxis ? dotRow : dotCol) >= 0.0 ? '+' : '-';
	out.sign = correctedCurrentDragSign(currentFace, out.useRow, axisSign);

	int move = -1;
	bool mapped = out.useRow
		? mapDragRowToMove(currentFace, currentRow, move)
		: mapDragColToMove(currentFace, currentCol, move);
	if (!mapped) {
		out.reason = out.useRow ? "middle row" : "middle col";
		return false;
	}

	out.ok = true;
	out.move = faceName(move);
	out.rev = isPracticeDragReverse(currentFace, move, out.sign);
	return true;
}

static void printPracticeCurrentDragResult(const MainViewState& viewState, const PracticeDragMoveDebug& result) {
	std::cout << "[PracticeDragCurrent] currentFace=" << faceName(viewState.practiceCurrentFace)
		<< " currentRow=" << viewState.practiceCurrentRow
		<< " currentCol=" << viewState.practiceCurrentCol
		<< " axis=" << result.axis
		<< " sign=" << result.sign
		<< " selector=" << (result.useRow ? "row" : "col")
		<< " len=" << result.dragLength
		<< " scoreRow=" << result.scoreA
		<< " scoreCol=" << result.scoreB;

	if (result.ok) {
		std::cout << " candidate=" << result.move << (result.rev ? "'" : "") << std::endl;
	}
	else {
		std::cout << " no outer move";
		if (result.reason[0] != '\0') {
			std::cout << " reason=" << result.reason;
		}
		std::cout << std::endl;
	}
}

static void printPracticeDragGeometry(const StickerDragGeometry& geometry) {
	std::cout << "[PracticeDragGeometry] currentFace=" << faceName(geometry.currentFace)
		<< " currentRow=" << geometry.currentRow
		<< " currentCol=" << geometry.currentCol
		<< std::endl;
}

static bool executePracticeMove(MainViewState& viewState, char move, bool rev) {
	if (g_app.currentMode != PLAY_MODE_PRACTICE) {
		return false;
	}

	bool& isSolving = g_app.isSolving;
	SolveTimer& solveTimer = g_app.solveTimer;
	RubikCube*& pRubikcube = g_app.pRubikcube;
	char(&Cube)[6][3][3] = g_app.Cube;
	bool& isAutoRotating = g_app.isAutoRotating;
	double& autoRotateSpeed = g_app.autoRotateSpeed;
	bool& isRestored = g_app.isRestored;
	bool& hasRand = viewState.hasRand;

	if (!pRubikcube || !pRubikcube->isExecuteOver() || isSolving) {
		return false;
	}

	int mode = rev ? -1 : 1;
	switch (move) {
	case 'U': up(Cube, mode); break;
	case 'D': down(Cube, mode); break;
	case 'F': front(Cube, mode); break;
	case 'B': back(Cube, mode); break;
	case 'R': right(Cube, mode); break;
	case 'L': left(Cube, mode); break;
	default:
		return false;
	}

	std::string cmd;
	cmd += move;
	if (rev) {
		cmd += "'";
	}

	Command = cmd;
	playCubeSound();
	isAutoRotating = false;
	autoRotateSpeed = 0.0;
	isRestored = false;
	pRubikcube->setRotateStep(0.1);
	pRubikcube->bindCommand(Command);

	if (hasRand && !solveTimer.isRunning()) {
		solveTimer.reset();
		solveTimer.start();
	}

	return true;
}

static void finishPracticeRightDrag(MainViewState& viewState, int endX, int endY) {
	viewState.practiceDragCurrentX = endX;
	viewState.practiceDragCurrentY = endY;

	PracticeDragMoveDebug currentResult;
	inferPracticeCurrentDragMove(
		viewState.practiceCurrentFace,
		viewState.practiceCurrentRow,
		viewState.practiceCurrentCol,
		viewState.practiceCurrentRowAxis,
		viewState.practiceCurrentColAxis,
		viewState.practiceDragStartX,
		viewState.practiceDragStartY,
		viewState.practiceDragCurrentX,
		viewState.practiceDragCurrentY,
		currentResult
	);
	printPracticeCurrentDragResult(viewState, currentResult);

	if (currentResult.ok) {
		executePracticeMove(viewState, currentResult.move, currentResult.rev);
	}
	resetPracticeRightDrag(viewState);
}

static POINT getCurrentMouseClientPos() {
	POINT pt;
	GetCursorPos(&pt);
	HWND hwnd = GetHWnd();
	ScreenToClient(hwnd, &pt);
	return pt;
}

//绘制练习模式按钮，并控制公式面板显示
void drawPracticeModeControls(MainViewState& viewState, GameControlState& controlState) {
	MOUSEMSG& msg = g_app.msg;
	FormulaPanel& formulaPanel = viewState.formulaPanel;
	bool& formulaPanelVisible = g_app.formulaPanelVisible;
	Button& btnRand = g_ui.playPractice.rand;
	Button& btnReset = g_ui.playPractice.reset;
	Button& btnFormula = g_ui.playPractice.formula;

	controlState.btnRandClicked = btnRand.draw(msg);
	controlState.btnResetClicked = btnReset.draw(msg);
	controlState.btnFormulaClicked = btnFormula.draw(msg);
	formulaPanel.setPosition(60, 400);

	if (controlState.btnFormulaClicked) {
		playClickSound();
		formulaPanelVisible = !formulaPanelVisible;
		formulaPanelVisible ? formulaPanel.show() : formulaPanel.hide();
	}
}

//处理练习模式随机打乱
void handlePracticeModeActions(MainViewState& viewState, const GameControlState& controlState) {
	bool& isSolving = g_app.isSolving;
	bool& isRestored = g_app.isRestored;
	SolveTimer& solveTimer = g_app.solveTimer;
	RubikCube*& pRubikcube = g_app.pRubikcube;
	char(&Cube)[6][3][3] = g_app.Cube;
	bool& isAutoRotating = g_app.isAutoRotating;
	double& autoRotateSpeed = g_app.autoRotateSpeed;
	bool& isBeginExecute = viewState.isBeginExecute;
	bool& hasRand = viewState.hasRand;

	if (controlState.btnRandClicked) {
		if (!isSolving && pRubikcube->isExecuteOver()) {
			Command = "";
			random_practice(Cube, 120);
			std::cout << Command << std::endl;
			solveTimer.reset();

			pRubikcube->setRotateStep(PI / 2);
			pRubikcube->bindCommand(Command);

			isBeginExecute = true;
			hasRand = true;
			isRestored = false;
			isAutoRotating = false;
			autoRotateSpeed = 0.0;
		}
	}
}

//练习模式右键拖拽外层旋转
void handlePracticeRightDragRotation(MainViewState& viewState) {
	PlayMode& currentMode = g_app.currentMode;
	MOUSEMSG& msg = g_app.msg;
	RubikCube*& pRubikcube = g_app.pRubikcube;
	bool& isSolving = g_app.isSolving;
	bool& isAutoRotating = g_app.isAutoRotating;
	double& autoRotateSpeed = g_app.autoRotateSpeed;

	if (currentMode != PLAY_MODE_PRACTICE) {
		if (viewState.practiceRightDragActive || viewState.practiceRightDragPreviewVisible) {
			resetPracticeRightDrag(viewState);
		}
		return;
	}

	if (g_ui.rankPanel.isVisible() || !pRubikcube || isSolving || !pRubikcube->isExecuteOver()) {
		if (viewState.practiceRightDragActive || viewState.practiceRightDragPreviewVisible) {
			resetPracticeRightDrag(viewState);
		}
		return;
	}

	if (viewState.practiceRightDragActive) {
		POINT pt = getCurrentMouseClientPos();
		viewState.practiceDragCurrentX = pt.x;
		viewState.practiceDragCurrentY = pt.y;
	}

	if (viewState.practiceRightDragActive && msg.uMsg != WM_RBUTTONUP && !GetKey(VK_RBUTTON)) {
		finishPracticeRightDrag(viewState, viewState.practiceDragCurrentX, viewState.practiceDragCurrentY);
		return;
	}

	if (msg.uMsg == WM_RBUTTONDOWN) {
		int mx = msg.x - Width / 2;
		int my = msg.y - Height / 2;
		int kk, ii, jj, pp;

		if (pRubikcube->pickStickerAtScreen(mx, my, kk, ii, jj, pp)) {
			StickerDragGeometry geometry;
			if (pRubikcube->getStickerDragGeometry(kk, ii, jj, pp, geometry)) {
				viewState.practiceRightDragActive = true;
				viewState.practiceRightDragPreviewVisible = true;
				viewState.practiceDragStartX = msg.x;
				viewState.practiceDragStartY = msg.y;
				viewState.practiceDragCurrentX = msg.x;
				viewState.practiceDragCurrentY = msg.y;
				viewState.practiceCurrentFace = geometry.currentFace;
				viewState.practiceCurrentRow = geometry.currentRow;
				viewState.practiceCurrentCol = geometry.currentCol;
				viewState.practiceCurrentRowAxis = projectAxisToScreenVector(geometry.stickerCenter, geometry.rowAxis);
				viewState.practiceCurrentColAxis = projectAxisToScreenVector(geometry.stickerCenter, geometry.colAxis);
				for (int n = 0; n < 4; ++n) {
					viewState.practiceDragPolygon[n] = geometry.screenPolygon[n];
				}

				isAutoRotating = false;
				autoRotateSpeed = 0.0;
				printPracticeDragGeometry(geometry);
			}
			else {
				resetPracticeRightDrag(viewState);
			}
		}
		else if (viewState.practiceRightDragActive || viewState.practiceRightDragPreviewVisible) {
			resetPracticeRightDrag(viewState);
		}

		msg.uMsg = 0;
		return;
	}

	if (msg.uMsg == WM_MOUSEMOVE && viewState.practiceRightDragActive) {
		viewState.practiceDragCurrentX = msg.x;
		viewState.practiceDragCurrentY = msg.y;
		msg.uMsg = 0;
		return;
	}

	if (msg.uMsg == WM_RBUTTONUP && viewState.practiceRightDragActive) {
		finishPracticeRightDrag(viewState, msg.x, msg.y);
		msg.uMsg = 0;
	}
}

//绘制右键拖拽的局部参考线，后续旋转判定会基于这组方向
void drawPracticeRightDragPreview(MainViewState& viewState) {
	if (g_app.currentMode != PLAY_MODE_PRACTICE || !viewState.practiceRightDragPreviewVisible) {
		return;
	}

	POINT p[4];
	for (int n = 0; n < 4; ++n) {
		p[n] = toWindowPoint(viewState.practiceDragPolygon[n]);
	}

	double cx = 0.0;
	double cy = 0.0;
	for (int n = 0; n < 4; ++n) {
		cx += p[n].x;
		cy += p[n].y;
	}
	cx /= 4.0;
	cy /= 4.0;

	double ax = (p[1].x - p[0].x + p[2].x - p[3].x) * 0.5;
	double ay = (p[1].y - p[0].y + p[2].y - p[3].y) * 0.5;
	double bx = (p[3].x - p[0].x + p[2].x - p[1].x) * 0.5;
	double by = (p[3].y - p[0].y + p[2].y - p[1].y) * 0.5;

	setorigin(0, 0);
	setlinecolor(RGB(255, 230, 150));
	setlinestyle(PS_SOLID, 2);
	drawCenteredLine(cx, cy, ax, ay, 62.0);
	drawCenteredLine(cx, cy, bx, by, 62.0);

	double dx = viewState.practiceDragCurrentX - viewState.practiceDragStartX;
	double dy = viewState.practiceDragCurrentY - viewState.practiceDragStartY;
	if (std::sqrt(dx * dx + dy * dy) > 4.0) {
		setlinecolor(RGB(255, 245, 210));
		line(viewState.practiceDragStartX, viewState.practiceDragStartY, viewState.practiceDragCurrentX, viewState.practiceDragCurrentY);
	}

	setfillcolor(RGB(255, 230, 150));
	solidcircle((int)cx, (int)cy, 4);
	setlinestyle(PS_SOLID, 1);
}

//检测手动复原完成并保存用户成绩
void handlePracticeModeCompletion(MainViewState& viewState) {
	PlayMode& currentMode = g_app.currentMode;
	bool& isSolving = g_app.isSolving;
	SolveTimer& solveTimer = g_app.solveTimer;
	RubikCube*& pRubikcube = g_app.pRubikcube;
	char(&Cube)[6][3][3] = g_app.Cube;
	bool& isRestored = g_app.isRestored;
	bool& hasRand = viewState.hasRand;

	if (currentMode == PLAY_MODE_PRACTICE && !isSolving && pRubikcube->isExecuteOver() && solveTimer.isRunning()) {
		if (isCubeSolved(Cube)) {
			solveTimer.stop();
			drawPopup("手动还原成功！", 1500);
			if (!isGuest()) {
				double userRecord = solveTimer.elapsed();
				saveScore(getCurrentUser().c_str(), userRecord);
			}

			isRestored = true;
			hasRand = false;
		}
	}
}

//处理公式面板点击，并同步逻辑魔方和动画命令
void handlePracticeFormulaExecution(MainViewState& viewState) {
	PlayMode& currentMode = g_app.currentMode;
	MOUSEMSG& msg = g_app.msg;
	FormulaPanel& formulaPanel = viewState.formulaPanel;

	if (currentMode == PLAY_MODE_PRACTICE) {
		formulaPanel.draw(msg);
		std::string formula = formulaPanel.getClickedFormula();
		if (!formula.empty()) {
			char move = formula[0];
			bool rev = (formula.size() > 1 && formula[1] == '\'');
			executePracticeMove(viewState, move, rev);
		}
	}
}
