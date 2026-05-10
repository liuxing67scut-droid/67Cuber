#pragma once

#include <cmath>

#include "rubikcube.h"
#include "game.h"
#include "cubesolver.h"
#include "utils.h"
#include "ui.h"
#include "music.h"
#include "timer.h"
#include "recordfile.h"
#include "user.h"

//集中保存所有按钮和跨页面面板对象
struct GameUiState {
	struct MenuButtons {
		Button guestEnter;
		Button userLogin;
	} menu;

	struct SelectButtons {
		Button cube3x3;
		Button cube2x2;
		Button back;
	} select;

	struct OverlayButtons {
		Button settings;
		Button rankList;
	} overlay;

	struct PlayCommonButtons {
		Button back;
	} playCommon;

	struct PlayModeButtons {
		Button autoMode;
		Button practiceMode;
		Button teachMode;
	} playMode;

	struct PlayAutoButtons {
		Button rand;
		Button solve;
		Button reset;
	} playAuto;

	struct PlayPracticeButtons {
		Button rand;
		Button reset;
		Button formula;
	} playPractice;

	struct PlayTeachButtons {
		Button testSolve;
		Button teachStep;
		Button reset;
	} playTeach;

	RankPanel rankPanel;
	LoginPanel loginPanel;
	SettingsPanel settingsPanel;
};

//游戏页内部模式
enum PlayMode {
	PLAY_MODE_AUTO = 0,
	PLAY_MODE_PRACTICE = 1,
	PLAY_MODE_TEACH = 2
};

//影响游戏运行和魔方逻辑的共享状态
struct GameAppState {
	GameState state = STATE_MENU;
	MOUSEMSG msg = { 0 };
	PlayMode currentMode = PLAY_MODE_AUTO;
	bool isSolving = false;
	bool isRestored = true;
	bool solvedByAlgo = false;
	bool formulaPanelVisible = false;
	SolveTimer solveTimer;
	RubikCube* pRubikcube = nullptr;
	char Cube[6][3][3] = {};
	Timer idleTimer;
	Timer autoRotateStartTimer;
	bool isAutoRotating = true;
	double autoRotateAngle = 0.0;
	double autoRotateSpeed = 0.0;
	Axis3D autoRotateAxis = { 1.0 / sqrt(3), 1.0 / sqrt(3), 1.0 / sqrt(3) };
};

//只和当前视图交互有关的状态
struct MainViewState {
	POINT org = { 0, 0 };
	POINT now = { 0, 0 };
	bool flag = false;
	bool isBeginExecute = true;
	bool hasRand = false;
	FormulaPanel formulaPanel;
	TeachColorPanel teachPanel;
	int sel_k = -1;
	int sel_i = -1;
	int sel_j = -1;
	int sel_p = -1;
	bool btnTestSolveClicked = false;
	bool teachModeHasRunOnce = false;

	MainViewState() {
		formulaPanel.setPosition(60, 470);
	}
};

//游戏页顶部模式按钮和自转参数
static const double AUTO_ROTATE_MAX_SPEED = 8.0;
static const double AUTO_ROTATE_ACCEL_TIME = 2.0;
static const int MODE_BTN_W = 180;
static const int MODE_BTN_H = 30;
static const int MODE_GAP = 20;
static const int MODE_TOTAL_W = MODE_BTN_W * 3 + MODE_GAP * 2;
static const int MODE_START_X = (Width - MODE_TOTAL_W) / 2;
static const int MODE_Y = 20;

extern GameUiState g_ui;
extern GameAppState g_app;
extern MainViewState g_viewState;
