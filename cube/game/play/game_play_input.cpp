#include "game_internal.h"

//绘制游戏页所有模式共用的返回按钮
static void drawSharedGameControls(GameControlState& controlState) {
	MOUSEMSG& msg = g_app.msg;
	Button& btnBack = g_ui.playCommon.back;

	setorigin(0, 0);
	controlState.btnBackClicked = btnBack.draw(msg);
}

//绘制设置、排行榜和当前用户等浮层入口
static void drawOverlayControls() {
	MOUSEMSG& msg = g_app.msg;
	Button& btnSettings = g_ui.overlay.settings;
	Button& btnRankList = g_ui.overlay.rankList;
	SettingsPanel& settingsPanel = g_ui.settingsPanel;
	RankPanel& rankPanel = g_ui.rankPanel;

	setorigin(0, 0);
	bool btnSetClick = btnSettings.draw(msg);
	bool btnRankClick = btnRankList.draw(msg);

	if (btnSetClick) {
		playClickSound();
		if (settingsPanel.isVisible()) {
			settingsPanel.hide();
		}
		else {
			rankPanel.hide();
			int btnR_X = Width - 120;
			int btnR_Y = 60;
			int btnW = 80, btnH = 30;
			int panelW = 200;
			int panelX = btnR_X + btnW - panelW;
			int panelY = btnR_Y + btnH + 5;
			settingsPanel.show(panelX, panelY);
		}
	}

	if (btnRankClick) {
		settingsPanel.hide();
		rankPanel.show(Width, Height);
	}
	drawCurrentUser(Width);
}

//处理复位和返回等所有模式共用的按钮行为
static bool handleSharedGameActions(MainViewState& viewState, const GameControlState& controlState) {
	PlayMode& currentMode = g_app.currentMode;
	GameState& state = g_app.state;
	MOUSEMSG& msg = g_app.msg;
	RubikCube*& pRubikcube = g_app.pRubikcube;

	if (controlState.btnResetClicked) {
		rebuildCubeForMode(currentMode);
		resetRuntimeAfterCubeRebuild(viewState, true);
		drawPopup("复位完成", 1500);
	}

	if (controlState.btnBackClicked) {
		delete pRubikcube;
		pRubikcube = nullptr;
		state = STATE_SELECT;
		msg = {};
		return false;
	}

	return true;
}

//处理鼠标拖拽旋转视角
bool processGameCameraInput(MainViewState& viewState) {
	RubikCube*& pRubikcube = g_app.pRubikcube;
	bool userInteracted = false;

	if (GetKey(VK_LBUTTON)) {
		if (!viewState.flag) {
			GetCursorPos(&viewState.org);
			viewState.flag = true;
		}
		userInteracted = true;
	}
	else {
		viewState.flag = false;
	}

	if (viewState.flag) {
		GetCursorPos(&viewState.now);
		pRubikcube->rotateZ(-1.0 * (viewState.now.x - viewState.org.x));
		pRubikcube->rotateY(-1.0 * (viewState.now.y - viewState.org.y));
		GetCursorPos(&viewState.org);
		userInteracted = true;
	}

	return userInteracted;
}

//按当前模式绘制对应按钮，并记录本帧点击结果
void drawGamePageControls(MainViewState& viewState, GameControlState& controlState) {
	PlayMode& currentMode = g_app.currentMode;
	drawSharedGameControls(controlState);

	if (currentMode == PLAY_MODE_AUTO) {
		drawAutoModeControls(controlState);
	}
	else if (currentMode == PLAY_MODE_PRACTICE) {
		drawPracticeModeControls(viewState, controlState);
	}
	else if (currentMode == PLAY_MODE_TEACH) {
		drawTeachModeControls(viewState, controlState);
	}

	drawOverlayControls();
}

//按当前模式分发按钮行为，再处理共用行为
bool handleGameControlActions(MainViewState& viewState, const GameControlState& controlState) {
	PlayMode& currentMode = g_app.currentMode;
	if (currentMode == PLAY_MODE_AUTO) {
		handleAutoModeActions(viewState, controlState);
	}
	else if (currentMode == PLAY_MODE_PRACTICE) {
		handlePracticeModeActions(viewState, controlState);
	}
	else if (currentMode == PLAY_MODE_TEACH) {
		handleTeachModeActions(viewState, controlState);
	}

	return handleSharedGameActions(viewState, controlState);
}
