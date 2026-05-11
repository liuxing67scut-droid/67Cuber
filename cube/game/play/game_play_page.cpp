#include "game_internal.h"

#include "ui_play_cover.h"

//游戏页单帧主流程，按输入、按钮、场景、浮层顺序调度
bool handleGamePage(MainViewState& viewState) {
	GameState& state = g_app.state;
	RubikCube*& pRubikcube = g_app.pRubikcube;
	
	if (pRubikcube == nullptr) {
		state = STATE_SELECT;
		return false;
	}

	setorigin(0, 0);
	drawPlayBackground();
	setorigin(Width / 2, Height / 2);

	bool userInteracted = processGameCameraInput(viewState);

	GameControlState controlState;
	drawGamePageControls(viewState, controlState);

	setorigin(Width / 2, Height / 2);
	if (!handleGameControlActions(viewState, controlState)) {
		return false;
	}

	updateAndDrawGameScene(viewState, userInteracted);
	handleGameRuntimeUi(viewState);
	
	return true;
}
