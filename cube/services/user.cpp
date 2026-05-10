#include "user.h"

static string g_currentUser = "游客";

void setCurrentUser(const string& username) {
    g_currentUser = username.empty() ? "游客" : username;
}

string getCurrentUser() {
    return g_currentUser;
}

//绘制右上角用户名，结束后恢复 EasyX 绘图状态
void drawCurrentUser(int screenWidth) {
    COLORREF saveText = gettextcolor();
    int saveBk = getbkmode();
    LOGFONT saveFont;
    gettextstyle(&saveFont);

    setbkmode(TRANSPARENT);
    settextcolor(WHITE);
    settextstyle(25, 0, _T("微软雅黑"));

    string displayStr = "ID：" + g_currentUser;
    outtextxy(screenWidth - 300, 25, displayStr.c_str());

    settextcolor(saveText);
    setbkmode(saveBk);
    settextstyle(&saveFont);
}

bool isGuest() {
    return (g_currentUser == "游客");
}
