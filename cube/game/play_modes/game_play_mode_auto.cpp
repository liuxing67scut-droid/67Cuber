#include "game_internal.h"

#include <iostream>

//绘制自动模式的打乱、还原、复位按钮
void drawAutoModeControls(GameControlState& controlState) {
	MOUSEMSG& msg = g_app.msg;
	Button& btnRand = g_ui.playAuto.rand;
	Button& btnSolve = g_ui.playAuto.solve;
	Button& btnReset = g_ui.playAuto.reset;

	controlState.btnRandClicked = btnRand.draw(msg);
	controlState.btnSolveClicked = btnSolve.draw(msg);
	controlState.btnResetClicked = btnReset.draw(msg);
}

//处理自动打乱和算法还原
void handleAutoModeActions(MainViewState& viewState, const GameControlState& controlState) {
	bool& isSolving = g_app.isSolving;
	bool& isRestored = g_app.isRestored;
	bool& solvedByAlgo = g_app.solvedByAlgo;
	SolveTimer& solveTimer = g_app.solveTimer;
	RubikCube*& pRubikcube = g_app.pRubikcube;
	char(&Cube)[6][3][3] = g_app.Cube;
	bool& isAutoRotating = g_app.isAutoRotating;
	double& autoRotateSpeed = g_app.autoRotateSpeed;
	bool& isBeginExecute = viewState.isBeginExecute;
	bool& hasRand = viewState.hasRand;

	if (controlState.btnRandClicked) {
		if (!isSolving && pRubikcube->isExecuteOver()) {
			Command = "";
			random(Cube, 200);
			std::cout << Command << std::endl;

			pRubikcube->setRotateStep(PI / 2);
			pRubikcube->bindCommand(Command);

			isBeginExecute = true;
			hasRand = true;
			isRestored = false;
			isAutoRotating = false;
			autoRotateSpeed = 0.0;
		}
	}

	if (controlState.btnSolveClicked) {
		if (pRubikcube->isExecuteOver()) {
			if (isCubeSolved(Cube)) {
				drawPopup("已复位", 1500);
				isRestored = true;
			}
			else {
				Command = "";
				Solute(Cube);
				std::cout << Command << std::endl;

				pRubikcube->setRotateStep(0.05);
				pRubikcube->bindCommand(Command);

				isSolving = true;
				solvedByAlgo = true;
				isAutoRotating = false;
				autoRotateSpeed = 0.0;
				solveTimer.reset();
				solveTimer.start();
			}
		}
	}
}
