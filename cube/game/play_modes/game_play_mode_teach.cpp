#include "game_internal.h"

#include <iostream>

void drawTeachModeControls(MainViewState& viewState, GameControlState& controlState) {
	MOUSEMSG& msg = g_app.msg;
	Button& btnTestSolve = g_ui.playTeach.testSolve;
	Button& btnTeachStep = g_ui.playTeach.teachStep;
	Button& btnReset = g_ui.playTeach.reset;

	viewState.btnTestSolveClicked = btnTestSolve.draw(msg);
	controlState.btnTeachStepClicked = btnTeachStep.draw(msg);
	controlState.btnResetClicked = btnReset.draw(msg);
}

void handleTeachModeActions(MainViewState& viewState, const GameControlState& controlState) {
	bool& isSolving = g_app.isSolving;
	bool& isRestored = g_app.isRestored;
	bool& solvedByAlgo = g_app.solvedByAlgo;
	SolveTimer& solveTimer = g_app.solveTimer;
	RubikCube*& pRubikcube = g_app.pRubikcube;
	char(&Cube)[6][3][3] = g_app.Cube;
	bool& isAutoRotating = g_app.isAutoRotating;
	double& autoRotateSpeed = g_app.autoRotateSpeed;
	bool& teachModeHasRunOnce = viewState.teachModeHasRunOnce;

	if (viewState.btnTestSolveClicked) {
		if (isSolving || !pRubikcube->isExecuteOver()) {
		}
		else if (teachModeHasRunOnce) {
			drawPopup("Please reset first", 1500);
		}
		else {
			bool incomplete = false;
			for (int f = 0; f < 6 && !incomplete; ++f) {
				for (int r = 0; r < 3 && !incomplete; ++r) {
					for (int c = 0; c < 3; ++c) {
						if (Cube[f][r][c] == ' ') {
							incomplete = true;
							break;
						}
					}
				}
			}

			if (incomplete) {
				drawPopup("Coloring incomplete", 1500);
			}
			else if (isCubeSolved(Cube)) {
				drawPopup("Already solved", 1500);
				isRestored = true;
			}
			else {
				Command = "";
				std::cout << Command << std::endl;

				std::string reason;
				if (!validateColoring(Cube, reason)) {
					drawPopup(reason.c_str(), 2000);
				}
				else {
					Solute(Cube);
					std::cout << Command << std::endl;

					pRubikcube->setRotateStep(0.05);
					pRubikcube->bindCommand(Command);

					teachModeHasRunOnce = true;
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

	if (controlState.btnTeachStepClicked) {
		drawPopup("Teach mode TBD", 1500);
	}
}

void handleTeachModePickInput(MainViewState& viewState) {
	PlayMode& currentMode = g_app.currentMode;
	MOUSEMSG& msg = g_app.msg;
	RubikCube*& pRubikcube = g_app.pRubikcube;
	TeachColorPanel& teachPanel = viewState.teachPanel;
	int& sel_k = viewState.sel_k;
	int& sel_i = viewState.sel_i;
	int& sel_j = viewState.sel_j;
	int& sel_p = viewState.sel_p;

	if (currentMode == PLAY_MODE_TEACH && msg.uMsg == WM_RBUTTONDOWN) {
		int mx = msg.x - Width / 2;
		int my = msg.y - Height / 2;
		int kk, ii, jj, pp;
		if (pRubikcube->pickStickerAtScreen(mx, my, kk, ii, jj, pp)) {
			bool isCenter = false;
			if ((kk == 0 && ii == 1 && jj == 1 && pp == 0) ||
				(kk == 1 && ii == 1 && jj == 2 && pp == 1) ||
				(kk == 2 && ii == 1 && jj == 1 && pp == 4) ||
				(kk == 1 && ii == 1 && jj == 0 && pp == 5) ||
				(kk == 1 && ii == 0 && jj == 1 && pp == 3) ||
				(kk == 1 && ii == 2 && jj == 1 && pp == 2)) {
				isCenter = true;
			}
			if (isCenter) {
				drawPopup("Center locked", 1000);
				pRubikcube->clearAllHighlights();
			}
			else {
				sel_k = kk;
				sel_i = ii;
				sel_j = jj;
				sel_p = pp;

				int colorpanelW = 160;
				int colorpanelH = 110;
				int colorpanel_px = msg.x;
				int colorpanel_py = msg.y;

				if (colorpanel_px + colorpanelW > getwidth()) {
					colorpanel_px = getwidth() - colorpanelW;
				}
				if (colorpanel_py + colorpanelH > getheight()) {
					colorpanel_py = getheight() - colorpanelH;
				}

				teachPanel.setPosition(colorpanel_px, colorpanel_py);
				teachPanel.show();
			}
		}
		msg.uMsg = 0;
	}
}

void handleTeachPanelInteraction(MainViewState& viewState) {
	MOUSEMSG& msg = g_app.msg;
	RubikCube*& pRubikcube = g_app.pRubikcube;
	char(&Cube)[6][3][3] = g_app.Cube;
	TeachColorPanel& teachPanel = viewState.teachPanel;
	int& sel_k = viewState.sel_k;
	int& sel_i = viewState.sel_i;
	int& sel_j = viewState.sel_j;
	int& sel_p = viewState.sel_p;
	bool& teachModeHasRunOnce = viewState.teachModeHasRunOnce;

	if (teachPanel.isVisible()) {
		teachPanel.draw(msg);
		char chosen = teachPanel.getClickedColor();

		if (chosen != 0) {
			if (teachModeHasRunOnce) {
				drawPopup("Please reset first", 1500);
				clearTeachSelection(viewState);
			}
			else if (sel_k != -1 && pRubikcube && pRubikcube->isExecuteOver()) {
				auto colorCharToRGB = [](char c) -> COLORREF {
					switch (c) {
					case 'G': return RGB(0, 255, 0);
					case 'O': return RGB(255, 128, 50);
					case 'Y': return RGB(255, 255, 0);
					case 'W': return RGB(235, 235, 235);
					case 'R': return RGB(235, 0, 0);
					case 'B': return RGB(62, 157, 251);
					default: return RGB(70, 70, 70);
					}
				};
				COLORREF col = colorCharToRGB(chosen);

				pRubikcube->setStickerColorVisual(sel_k, sel_i, sel_j, sel_p, col);

				int face, rr, cc;
				if (pRubikcube->mapStickerToFace(sel_k, sel_i, sel_j, sel_p, face, rr, cc)) {
					Cube[face][rr][cc] = chosen;
				}

				clearTeachSelection(viewState);
			}
		}
		else if (msg.uMsg == WM_LBUTTONDOWN) {
			clearTeachSelection(viewState);
		}

		if (msg.uMsg == WM_LBUTTONDOWN) {
			msg.uMsg = 0;
		}
	}
}
