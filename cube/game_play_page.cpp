#include "game_internal.h"

bool handleGamePage(MainViewState& viewState) {
	GameState& state = g_app.state;
	RubikCube*& pRubikcube = g_app.pRubikcube;
	
	if (pRubikcube == nullptr) {
		state = STATE_SELECT;
		return false;
	}

	setbkcolor(RGB(35, 35, 35));
	setorigin(Width / 2, Height / 2);

	//game page页主流程：

	//拖拽视角
	bool userInteracted = processGameCameraInput(viewState);

	GameControlState controlState;
	//不同模式不同按钮
	drawGamePageControls(viewState, controlState);

	setorigin(Width / 2, Height / 2);
	//不同模式不同处理
	if (!handleGameControlActions(viewState, controlState)) {
		return false;
	}

	//显示处理
	updateAndDrawGameScene(viewState, userInteracted);
	handleGameRuntimeUi(viewState);
	
	return true;
}