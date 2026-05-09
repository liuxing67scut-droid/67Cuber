#include "user.h"

static string g_currentUser = "游客"; // 静态全局变量

void setCurrentUser(const string& username) {
    g_currentUser = username.empty() ? "游客" : username;
}

string getCurrentUser() {
    return g_currentUser;
}

void drawCurrentUser(int screenWidth) {
    // 1. 保存绘图状态
    COLORREF saveText = gettextcolor();
    int saveBk = getbkmode();
    LOGFONT saveFont;
    gettextstyle(&saveFont);

    // 2. 设置样式
    setbkmode(TRANSPARENT);
    settextcolor(WHITE);
    settextstyle(25, 0, _T("微软雅黑"));

    // 3. 拼接文字
    string displayStr = "ID：" + g_currentUser;

    // 4. 绘制（右上角，距离右边  px，距离顶部  px）
    outtextxy(screenWidth - 300, 25, displayStr.c_str());

    // 5. 恢复状态
    settextcolor(saveText);
    setbkmode(saveBk);
    settextstyle(&saveFont);
}

bool isGuest() {
    return (g_currentUser == "游客");
}