#include "game_state.h"

#include "ui_menu_cover.h"

//菜单页入口
void handleMenuPage() {
	GameState& state = g_app.state;
	MOUSEMSG& msg = g_app.msg;
	Button& btnGuestEnter = g_ui.menu.guestEnter;
	Button& btnUserLogin = g_ui.menu.userLogin;
	LoginPanel& loginPanel = g_ui.loginPanel;

	setorigin(0, 0);
	drawMenuCover();

	bool guestClick = btnGuestEnter.draw(msg);
	bool loginClick = btnUserLogin.draw(msg);

	if (guestClick) {
		playClickSound();
		setCurrentUser("游客");
		state = STATE_SELECT;
	}

	if (loginClick) {
		playClickSound();
		loginPanel.show(Width, Height);
	}

	if (loginClick) {
		playClickSound();

		char inputName[64] = { 0 };
		if (InputBox(inputName, 64, "请输入用户名", "用户登录", "", false)) {
			if (strlen(inputName) > 0) {
				setCurrentUser(string(inputName));
				state = STATE_SELECT;
			}
		}
	}

	FlushBatchDraw();
}
