#include "game_internal.h"

static void drawSharedGameControls(GameControlState& controlState) {
	MOUSEMSG& msg = g_app.msg;
	Button& btnBack = g_ui.playCommon.back;

	setorigin(0, 0);
	controlState.btnBackClicked = btnBack.draw(msg);
}

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
		rankPanel.show(Width, Height);
	}
	drawCurrentUser(Width);
}

static bool handleSharedGameActions(MainViewState& viewState, const GameControlState& controlState) {
	PlayMode& currentMode = g_app.currentMode;
	GameState& state = g_app.state;
	MOUSEMSG& msg = g_app.msg;
	RubikCube*& pRubikcube = g_app.pRubikcube;

	if (controlState.btnResetClicked) {
		rebuildCubeForMode(currentMode);
		resetRuntimeAfterCubeRebuild(viewState, true);
		drawPopup("Reset complete", 1500);
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
