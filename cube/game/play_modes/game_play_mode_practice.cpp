#include "game_internal.h"

#include <iostream>

void drawPracticeModeControls(MainViewState& viewState, GameControlState& controlState) {
	MOUSEMSG& msg = g_app.msg;
	FormulaPanel& formulaPanel = viewState.formulaPanel;
	bool& formulaPanelVisible = g_app.formulaPanelVisible;
	Button& btnRand = g_ui.playPractice.rand;
	Button& btnReset = g_ui.playPractice.reset;
	Button& btnFormula = g_ui.playPractice.formula;

	controlState.btnRandClicked = btnRand.draw(msg);
	controlState.btnResetClicked = btnReset.draw(msg);
	controlState.btnFormulaClicked = btnFormula.draw(msg);
	formulaPanel.setPosition(60, 400);

	if (controlState.btnFormulaClicked) {
		playClickSound();
		formulaPanelVisible = !formulaPanelVisible;
		formulaPanelVisible ? formulaPanel.show() : formulaPanel.hide();
	}
}

void handlePracticeModeActions(MainViewState& viewState, const GameControlState& controlState) {
	bool& isSolving = g_app.isSolving;
	bool& isRestored = g_app.isRestored;
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
			random_practice(Cube, 120);
			std::cout << Command << std::endl;
			solveTimer.reset();

			pRubikcube->setRotateStep(PI / 2);
			pRubikcube->bindCommand(Command);

			isBeginExecute = true;
			hasRand = true;
			isRestored = false;
			isAutoRotating = false;
			autoRotateSpeed = 0.0;
		}
	}
}

void handlePracticeModeCompletion(MainViewState& viewState) {
	PlayMode& currentMode = g_app.currentMode;
	bool& isSolving = g_app.isSolving;
	SolveTimer& solveTimer = g_app.solveTimer;
	RubikCube*& pRubikcube = g_app.pRubikcube;
	char(&Cube)[6][3][3] = g_app.Cube;
	bool& isRestored = g_app.isRestored;
	bool& hasRand = viewState.hasRand;

	if (currentMode == PLAY_MODE_PRACTICE && !isSolving && pRubikcube->isExecuteOver() && solveTimer.isRunning()) {
		if (isCubeSolved(Cube)) {
			solveTimer.stop();
			drawPopup("Manual solve!", 1500);
			if (!isGuest()) {
				double userRecord = solveTimer.elapsed();
				saveScore(getCurrentUser().c_str(), userRecord);
			}

			isRestored = true;
			hasRand = false;
		}
	}
}

void handlePracticeFormulaExecution(MainViewState& viewState) {
	PlayMode& currentMode = g_app.currentMode;
	MOUSEMSG& msg = g_app.msg;
	bool& isSolving = g_app.isSolving;
	SolveTimer& solveTimer = g_app.solveTimer;
	RubikCube*& pRubikcube = g_app.pRubikcube;
	char(&Cube)[6][3][3] = g_app.Cube;
	bool& isAutoRotating = g_app.isAutoRotating;
	double& autoRotateSpeed = g_app.autoRotateSpeed;
	bool& isRestored = g_app.isRestored;
	bool& hasRand = viewState.hasRand;
	FormulaPanel& formulaPanel = viewState.formulaPanel;

	if (currentMode == PLAY_MODE_PRACTICE) {
		formulaPanel.draw(msg);
		std::string formula = formulaPanel.getClickedFormula();
		if (!formula.empty() && pRubikcube && pRubikcube->isExecuteOver() && !isSolving) {
			isAutoRotating = false;
			autoRotateSpeed = 0.0;
			char move = formula[0];
			bool rev = (formula.size() > 1 && formula[1] == '\'');
			int mode = rev ? -1 : 1;
			switch (move) {
			case 'U': up(Cube, mode); break;
			case 'D': down(Cube, mode); break;
			case 'F': front(Cube, mode); break;
			case 'B': back(Cube, mode); break;
			case 'R': right(Cube, mode); break;
			case 'L': left(Cube, mode); break;
			}
			std::string cmd;
			cmd += move;
			if (rev) {
				cmd += "'";
			}
			Command = cmd;
			playCubeSound();
			isRestored = false;
			pRubikcube->setRotateStep(0.1);
			pRubikcube->bindCommand(Command);
			if (hasRand && !solveTimer.isRunning()) {
				solveTimer.reset();
				solveTimer.start();
			}
		}
	}
}
