#include "game_internal.h"

#include "app_paths.h"

//game层共享状态
GameUiState g_ui;
GameAppState g_app;
MainViewState g_viewState;

//初始化路径、窗口、音频和按钮等基础环境
void initGameApp(const char* exePath) {
	initAppPaths(exePath);
	initMusic();
	initgraph(Width, Height);
	initRecordFile();
	setbkcolor(UIConfig::COL_BG);
	BeginBatchDraw();
	initWinPos_Title(getPathName(exePath), "67 Cuber");
	initButtons();
}

//单帧调度入口，根据当前页面状态分发到具体页面
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

	if (state == STATE_MENU) {
		handleMenuPage();
	}
	else if (state == STATE_SELECT) {
		handleSelectPage(g_viewState);
	}
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

//释放图形窗口和魔方对象
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
