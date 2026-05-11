#include "game_state.h"

#include "ui_select_cover.h"

#include <iostream>

//选择魔方页，负责进入3阶游戏并初始化默认运行状态
void handleSelectPage(MainViewState& viewState) {
	PlayMode& currentMode = g_app.currentMode;
	GameState& state = g_app.state;
	MOUSEMSG& msg = g_app.msg;
	RubikCube*& pRubikcube = g_app.pRubikcube;
	bool& formulaPanelVisible = g_app.formulaPanelVisible;
	double& autoRotateAngle = g_app.autoRotateAngle;
	bool& isAutoRotating = g_app.isAutoRotating;
	double& autoRotateSpeed = g_app.autoRotateSpeed;
	Timer& idleTimer = g_app.idleTimer;
	Timer& autoRotateStartTimer = g_app.autoRotateStartTimer;
	Axis3D& autoRotateAxis = g_app.autoRotateAxis;
	SolveTimer& solveTimer = g_app.solveTimer;
	char(&Cube)[6][3][3] = g_app.Cube;
	FormulaPanel& formulaPanel = viewState.formulaPanel;
	Button& btn3x3 = g_ui.select.cube3x3;
	Button& btn2x2 = g_ui.select.cube2x2;
	Button& btnBack = g_ui.select.back;

	setorigin(0, 0);
	drawSelectCover();

	if (btn3x3.draw(msg)) {
		std::cout << "3x3" << std::endl;

		if (pRubikcube == nullptr) {
			pRubikcube = new RubikCube();
			init(Cube);
			viewState.flag = false;
			viewState.isBeginExecute = true;
			viewState.hasRand = false;
			pRubikcube->rotateZ(45.0);
			pRubikcube->rotateY(-30.0);
			autoRotateAngle = 0.0;
			isAutoRotating = true;
			autoRotateSpeed = 0.0;
			idleTimer.reset();
			autoRotateStartTimer.reset();
			autoRotateAxis = randomUnitAxis();
			solveTimer.reset();
		}
		
		currentMode = PLAY_MODE_AUTO;
		
		if (formulaPanel.isVisible()) {
			formulaPanel.hide();
			formulaPanelVisible = false;
		}
		state = STATE_GAME_3x3;
		msg = {};
	}

	//2阶入口暂留
	btn2x2.draw(msg);
	if (btnBack.draw(msg)) {
		state = STATE_MENU;
		msg = {};
	}
}
