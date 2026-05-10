#include "game_internal.h"

//清除教学模式中的贴纸选择和颜色面板
void clearTeachSelection(MainViewState& viewState) {
	RubikCube*& pRubikcube = g_app.pRubikcube;
	viewState.teachPanel.hide();
	if (pRubikcube) {
		pRubikcube->clearAllHighlights();
	}
	viewState.sel_k = -1;
	viewState.sel_i = -1;
	viewState.sel_j = -1;
	viewState.sel_p = -1;
}

//按目标模式重建魔方模型和逻辑状态
void rebuildCubeForMode(PlayMode mode) {
	RubikCube*& pRubikcube = g_app.pRubikcube;
	char(&Cube)[6][3][3] = g_app.Cube;
	if (pRubikcube) {
		delete pRubikcube;
	}
	pRubikcube = new RubikCube();

	if (mode == PLAY_MODE_TEACH) {
		for (int f = 0; f < 6; f++) {
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					Cube[f][i][j] = ' ';
				}
			}
		}
		Cube[F][1][1] = Color[F];
		Cube[L][1][1] = Color[L];
		Cube[U][1][1] = Color[U];
		Cube[D][1][1] = Color[D];
		Cube[R][1][1] = Color[R];
		Cube[B][1][1] = Color[B];
		pRubikcube->setCentersColoredOnly();
	}
	else {
		init(Cube);
	}

	pRubikcube->rotateZ(45.0);
	pRubikcube->rotateY(-30.0);
}

//重建魔方后重置计时、自转、公式面板等运行状态
void resetRuntimeAfterCubeRebuild(MainViewState& viewState, bool resetTeachModeRunOnce) {
	Axis3D& autoRotateAxis = g_app.autoRotateAxis;
	bool& isRestored = g_app.isRestored;
	bool& isSolving = g_app.isSolving;
	bool& solvedByAlgo = g_app.solvedByAlgo;
	bool& isAutoRotating = g_app.isAutoRotating;
	double& autoRotateSpeed = g_app.autoRotateSpeed;
	Timer& idleTimer = g_app.idleTimer;
	Timer& autoRotateStartTimer = g_app.autoRotateStartTimer;
	SolveTimer& solveTimer = g_app.solveTimer;
	bool& formulaPanelVisible = g_app.formulaPanelVisible;
	clearTeachSelection(viewState);
	if (resetTeachModeRunOnce) {
		viewState.teachModeHasRunOnce = false;
	}

	autoRotateAxis = randomUnitAxis();
	viewState.flag = false;
	viewState.isBeginExecute = true;
	viewState.hasRand = false;
	isRestored = true;
	isSolving = false;
	solvedByAlgo = false;
	isAutoRotating = true;
	autoRotateSpeed = 0.0;
	idleTimer.reset();
	autoRotateStartTimer.reset();
	solveTimer.reset();
	if (viewState.formulaPanel.isVisible()) {
		viewState.formulaPanel.hide();
		formulaPanelVisible = false;
	}
}

//切换游戏模式，并重建对应模式的魔方状态
void switchToMode(PlayMode mode, MainViewState& viewState, const char* popupText) {
	PlayMode& currentMode = g_app.currentMode;
	MOUSEMSG& msg = g_app.msg;
	currentMode = mode;
	msg = {};
	rebuildCubeForMode(mode);
	resetRuntimeAfterCubeRebuild(viewState, true);
	drawPopup(popupText, 1000);
}
