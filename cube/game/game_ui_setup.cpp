#include "game_internal.h"

static void initMenuButtons() {
	Button& btnGuestEnter = g_ui.menu.guestEnter;
	Button& btnUserLogin = g_ui.menu.userLogin;

	btnGuestEnter.setRect((Width - 200) / 2, 300, 200, 60);
	btnGuestEnter.setText("游客进入");

	btnUserLogin.setRect((Width - 200) / 2, 400, 200, 60);
	btnUserLogin.setText("用户登录");
}

static void initSelectButtons() {
	Button& btn3x3 = g_ui.select.cube3x3;
	Button& btn2x2 = g_ui.select.cube2x2;
	Button& btnBack = g_ui.select.back;

	btn3x3.setRect((Width - 200) / 2, 300, 200, 60);
	btn3x3.setText("3 阶魔方");

	btn2x2.setRect((Width - 200) / 2, 400, 200, 60);
	btn2x2.setText("敬请期待");

	btnBack.setRect(20, 20, 80, 30);
	btnBack.setText("< 返回");
}

static void initOverlayButtons() {
	Button& btnRankList = g_ui.overlay.rankList;
	Button& btnSettings = g_ui.overlay.settings;

	int btnR_X = Width - 120;
	int btnR_Y = 20;
	int btnW = 80;
	int btnH = 30;

	btnRankList.setRect(btnR_X, btnR_Y, btnW, btnH);
	btnRankList.setText("排行榜");
	btnSettings.setRect(btnR_X, btnR_Y + btnH + 10, btnW, btnH);
	btnSettings.setText("设置");
}

static void initPlayButtons() {
	Button& btnBack = g_ui.playCommon.back;

	Button& btnModeAuto = g_ui.playMode.autoMode;
	Button& btnModePractice = g_ui.playMode.practiceMode;
	Button& btnModeTeach = g_ui.playMode.teachMode;

	Button& btnAutoRand = g_ui.playAuto.rand;
	Button& btnAutoSolve = g_ui.playAuto.solve;
	Button& btnAutoReset = g_ui.playAuto.reset;

	Button& btnPracticeRand = g_ui.playPractice.rand;
	Button& btnPracticeReset = g_ui.playPractice.reset;
	Button& btnPracticeFormula = g_ui.playPractice.formula;

	Button& btnTeachTestSolve = g_ui.playTeach.testSolve;
	Button& btnTeachStep = g_ui.playTeach.teachStep;
	Button& btnTeachReset = g_ui.playTeach.reset;

	btnBack.setRect(20, 20, 80, 30);
	btnBack.setText("< 返回");

	btnAutoRand.setRect(60, 200, 120, 50);
	btnAutoRand.setText("随机打乱");
	btnAutoSolve.setRect(60, 270, 120, 50);
	btnAutoSolve.setText("算法还原");
	btnAutoReset.setRect(60, 340, 120, 50);
	btnAutoReset.setText("一键复位");

	btnPracticeRand.setRect(60, 200, 120, 50);
	btnPracticeRand.setText("随机打乱");
	btnPracticeReset.setRect(60, 270, 120, 50);
	btnPracticeReset.setText("一键复位");
	btnPracticeFormula.setRect(60, 340, 120, 50);
	btnPracticeFormula.setText("公式控制");

	btnTeachTestSolve.setRect(60, 200, 120, 50);
	btnTeachTestSolve.setText("人机对战");
	btnTeachStep.setRect(60, 270, 120, 50);
	btnTeachStep.setText("步骤教学");
	btnTeachReset.setRect(60, 340, 120, 50);
	btnTeachReset.setText("一键复位");

	btnModeAuto.setRect(MODE_START_X, MODE_Y, MODE_BTN_W, MODE_BTN_H);
	btnModeAuto.setText("自动模式");
	btnModePractice.setRect(MODE_START_X + (MODE_BTN_W + MODE_GAP), MODE_Y, MODE_BTN_W, MODE_BTN_H);
	btnModePractice.setText("练习模式");
	btnModeTeach.setRect(MODE_START_X + (MODE_BTN_W + MODE_GAP) * 2, MODE_Y, MODE_BTN_W, MODE_BTN_H);
	btnModeTeach.setText("教学模式");
}

void initButtons() {
	initMenuButtons();
	initSelectButtons();
	initOverlayButtons();
	initPlayButtons();
}

void syncUiPanelsForCurrentState(MainViewState& viewState) {
	GameState& state = g_app.state;
	FormulaPanel& formulaPanel = viewState.formulaPanel;
	TeachColorPanel& teachPanel = viewState.teachPanel;
	bool& formulaPanelVisible = g_app.formulaPanelVisible;

	if (state != STATE_GAME_3x3) {
		if (formulaPanel.isVisible()) {
			formulaPanel.hide();
			formulaPanelVisible = false;
			formulaPanel.resetHover();
		}

		if (teachPanel.isVisible()) {
			teachPanel.hide();
		}

		viewState.sel_k = -1;
		viewState.sel_i = -1;
		viewState.sel_j = -1;
		viewState.sel_p = -1;

		if (g_ui.settingsPanel.isVisible()) {
			g_ui.settingsPanel.hide();
		}
		if (g_ui.rankPanel.isVisible()) {
			g_ui.rankPanel.hide();
		}
	}
}