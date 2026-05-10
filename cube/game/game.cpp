#include "game_internal.h"

#include "app_paths.h"

//game层共享状态
GameUiState g_ui;
GameAppState g_app;
MainViewState g_viewState;

void initGameApp(const char* exePath) {
	initAppPaths(exePath);
	//初始化窗口和基础环境
	initMusic();
	initgraph(Width, Height);
	initRecordFile();
	setbkcolor(UIConfig::COL_BG);
	BeginBatchDraw();
	initWinPos_Title(getPathName(exePath), "67 Cuber");
	initButtons();
}

void runGameApp() {
	GameState& state = g_app.state;
	MOUSEMSG& msg = g_app.msg;
	if (MouseHit()) {
		msg = GetMouseMsg();
	}

	cleardevice();

	//每帧先同步跨页面面板状态
	g_viewState.btnTestSolveClicked = false;
	syncUiPanelsForCurrentState(g_viewState);

	//按页面分发：

	//开始菜单页
	if (state == STATE_MENU) {
		handleMenuPage();
	}
	//选择魔方页
	else if (state == STATE_SELECT) {
		handleSelectPage(g_viewState);
	}
	//3阶魔方页
	else if (state == STATE_GAME_3x3) {
		if (!handleGamePage(g_viewState)) {
			return;
		}
	}

	FlushBatchDraw();
	Sleep(1);
	if (msg.uMsg == WM_LBUTTONDOWN) {
		msg.uMsg = 0;
	}
}

void closeGameApp() {
	RubikCube*& pRubikcube = g_app.pRubikcube;
	if (pRubikcube) {
		delete pRubikcube;
		pRubikcube = nullptr;
	}
	EndBatchDraw();
	closegraph();
	_getch();
}
