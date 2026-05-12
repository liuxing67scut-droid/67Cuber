#include "game_internal.h"

#include <iostream>

//算法执行完成后停止计时并保存成绩
static void handleAlgorithmSolveCompletion() {
	bool& isSolving = g_app.isSolving;
	bool& isRestored = g_app.isRestored;
	bool& solvedByAlgo = g_app.solvedByAlgo;
	SolveTimer& solveTimer = g_app.solveTimer;
	RubikCube*& pRubikcube = g_app.pRubikcube;

	if (isSolving && pRubikcube->isExecuteOver()) {
		if (solvedByAlgo) {
			solveTimer.stop();
			drawPopup("还原成功！", 1500);
			double pcRecord = solveTimer.elapsed();
			saveScore("PC", pcRecord);

			std::vector<UserScore> top10 = getSortedTopScores();
			std::cout << "=== Top " << top10.size() << " ===" << std::endl;
			for (int top_i = 0; top_i < top10.size(); top_i++) {
				std::cout << (top_i + 1) << ". " << top10[top_i].username
					<< " - " << top10[top_i].time << "s" << std::endl;
			}

			solvedByAlgo = false;
		}
		isSolving = false;
		isRestored = true;
	}
}

//绘制计时器和弹窗提示
static void drawGameHud() {
	SolveTimer& solveTimer = g_app.solveTimer;

	setorigin(0, 0);
	std::string timeStr = solveTimer.format();
	settextcolor(UIConfig::COL_HUD_TEXT);
	setbkmode(TRANSPARENT);
	settextstyle(36, 0, _T("Microsoft YaHei"));
	outtextxy((Width - textwidth(timeStr.c_str())) / 2, 80, timeStr.c_str());
	showPopupIfNeeded();
}

//处理顶部模式切换按钮
static void handleModeTabButtons(MainViewState& viewState) {
	PlayMode& currentMode = g_app.currentMode;
	MOUSEMSG& msg = g_app.msg;
	Button& btnModeAuto = g_ui.playMode.autoMode;
	Button& btnModePractice = g_ui.playMode.practiceMode;
	Button& btnModeTeach = g_ui.playMode.teachMode;

	bool autoClicked = btnModeAuto.draw(msg);
	bool practiceClicked = btnModePractice.draw(msg);
	bool teachClicked = btnModeTeach.draw(msg);

	if (autoClicked && currentMode != PLAY_MODE_AUTO) {
		switchToMode(PLAY_MODE_AUTO, viewState, "切换至自动模式");
	}

	if (practiceClicked && currentMode != PLAY_MODE_PRACTICE) {
		switchToMode(PLAY_MODE_PRACTICE, viewState, "切换至练习模式");
	}

	if (teachClicked && currentMode != PLAY_MODE_TEACH) {
		switchToMode(PLAY_MODE_TEACH, viewState, "切换至教学模式");
	}
}

static void drawModeTabHighlightAt(int x, int y) {
	const int padding = 4;
	setlinecolor(UIConfig::COL_MODE_ACTIVE_BORDER);
	setlinestyle(PS_SOLID, UIConfig::MODE_ACTIVE_BORDER_WIDTH);
	roundrect(x - padding, y - padding, x + MODE_BTN_W + padding, y + MODE_BTN_H + padding, 12, 12);
	setlinestyle(PS_SOLID, 1);
}

//高亮当前游戏模式
static void drawCurrentModeTabHighlight() {
	PlayMode& currentMode = g_app.currentMode;

	if (currentMode == PLAY_MODE_AUTO) {
		drawModeTabHighlightAt(MODE_START_X, MODE_Y);
	}
	if (currentMode == PLAY_MODE_PRACTICE) {
		drawModeTabHighlightAt(MODE_START_X + (MODE_BTN_W + MODE_GAP), MODE_Y);
	}
	if (currentMode == PLAY_MODE_TEACH) {
		drawModeTabHighlightAt(MODE_START_X + (MODE_BTN_W + MODE_GAP) * 2, MODE_Y);
	}
}

//处理设置面板和排行榜面板
static void handleOverlayPanels() {
	MOUSEMSG& msg = g_app.msg;
	SettingsPanel& settingsPanel = g_ui.settingsPanel;
	RankPanel& rankPanel = g_ui.rankPanel;

	if (settingsPanel.isVisible()) {
		settingsPanel.draw(msg);
		if (msg.uMsg == WM_LBUTTONDOWN) {
			msg.uMsg = 0;
		}
	}

	if (rankPanel.isVisible()) {
		if (rankPanel.draw(msg)) {
			rankPanel.hide();
		}
		if (msg.uMsg == WM_LBUTTONDOWN || msg.uMsg == WM_MOUSEWHEEL) {
			msg.uMsg = 0;
		}
	}
}

//更新自转、执行动画并绘制魔方场景
void updateAndDrawGameScene(MainViewState& viewState, bool userInteracted) {
	PlayMode& currentMode = g_app.currentMode;
	RubikCube*& pRubikcube = g_app.pRubikcube;
	TeachColorPanel& teachPanel = viewState.teachPanel;
	bool& isAutoRotating = g_app.isAutoRotating;
	double& autoRotateSpeed = g_app.autoRotateSpeed;
	Timer& idleTimer = g_app.idleTimer;
	Timer& autoRotateStartTimer = g_app.autoRotateStartTimer;
	Axis3D& autoRotateAxis = g_app.autoRotateAxis;
	double& autoRotateAngle = g_app.autoRotateAngle;
	FormulaPanel& formulaPanel = viewState.formulaPanel;
	SettingsPanel& settingsPanel = g_ui.settingsPanel;

	bool isPickingColor = (currentMode == PLAY_MODE_TEACH && teachPanel.isVisible());
	bool isPracticeRightDragging = (currentMode == PLAY_MODE_PRACTICE && viewState.practiceRightDragActive);
	bool autoRotateEnabled = settingsPanel.isAutoRotateOn();

	if (userInteracted || isPracticeRightDragging || !pRubikcube->isExecuteOver() || isPickingColor || !autoRotateEnabled) {
		isAutoRotating = false;
		autoRotateSpeed = 0.0;
		idleTimer.reset();
	}
	else if (!isAutoRotating && idleTimer.elapsed() > 5.0 && autoRotateEnabled) {
		isAutoRotating = true;
		autoRotateSpeed = 0.0;
		autoRotateStartTimer.reset();
		autoRotateAxis = randomUnitAxis();
	}

	if (isAutoRotating) {
		double t = autoRotateStartTimer.elapsed();
		autoRotateSpeed = (t < AUTO_ROTATE_ACCEL_TIME)
			? (AUTO_ROTATE_MAX_SPEED * smoothS(t, AUTO_ROTATE_ACCEL_TIME))
			: AUTO_ROTATE_MAX_SPEED;
		autoRotateAngle += autoRotateSpeed;
		double rad = autoRotateSpeed * PI / 180.0;
		pRubikcube->rotateX(rad * autoRotateAxis.x);
		pRubikcube->rotateY(rad * autoRotateAxis.y);
		pRubikcube->rotateZ(rad * autoRotateAxis.z);
	}

	pRubikcube->execute();
	pRubikcube->updateRotate();
	pRubikcube->draw();

	if (currentMode == PLAY_MODE_PRACTICE && formulaPanel.isVisible() && formulaPanel.isAxesOn()) {
		pRubikcube->drawTestAxes();
	}
}

//场景绘制后处理模式面板、完成判定和浮层
void handleGameRuntimeUi(MainViewState& viewState) {
	handleTeachModePickInput(viewState);
	handlePracticeRightDragRotation(viewState);
	handlePracticeModeCompletion(viewState);
	handleAlgorithmSolveCompletion();
	drawPracticeRightDragPreview(viewState);
	drawGameHud();
	handlePracticeFormulaExecution(viewState);
	handleTeachPanelInteraction(viewState);
	handleModeTabButtons(viewState);
	drawCurrentModeTabHighlight();
	handleOverlayPanels();
}
