#pragma once

#include "game_state.h"

//按钮点击结果
struct GameControlState {
	bool btnRandClicked = false;
	bool btnSolveClicked = false;
	bool btnResetClicked = false;
	bool btnFormulaClicked = false;
	bool btnTeachStepClicked = false;
	bool btnBackClicked = false;
};

//初始化
void initButtons();
void syncUiPanelsForCurrentState(MainViewState& viewState);

//页面入口
void handleMenuPage();
void handleSelectPage(MainViewState& viewState);
bool handleGamePage(MainViewState& viewState);

//局状态
void clearTeachSelection(MainViewState& viewState);
void rebuildCubeForMode(PlayMode mode);
void resetRuntimeAfterCubeRebuild(MainViewState& viewState, bool resetTeachModeRunOnce);
void switchToMode(PlayMode mode, MainViewState& viewState, const char* popupText);

//游戏页共通流程
bool processGameCameraInput(MainViewState& viewState);
void drawGamePageControls(MainViewState& viewState, GameControlState& controlState);
bool handleGameControlActions(MainViewState& viewState, const GameControlState& controlState);
void updateAndDrawGameScene(MainViewState& viewState, bool userInteracted);
void handleGameRuntimeUi(MainViewState& viewState);

//自动模式
void drawAutoModeControls(GameControlState& controlState);
void handleAutoModeActions(MainViewState& viewState, const GameControlState& controlState);

//练习模式
void drawPracticeModeControls(MainViewState& viewState, GameControlState& controlState);
void handlePracticeModeActions(MainViewState& viewState, const GameControlState& controlState);
void handlePracticeModeCompletion(MainViewState& viewState);
void handlePracticeFormulaExecution(MainViewState& viewState);

//教学模式
void drawTeachModeControls(MainViewState& viewState, GameControlState& controlState);
void handleTeachModeActions(MainViewState& viewState, const GameControlState& controlState);
void handleTeachModePickInput(MainViewState& viewState);
void handleTeachPanelInteraction(MainViewState& viewState);
