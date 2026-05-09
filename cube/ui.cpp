#include "ui.h"
#include "music.h"
#include <ctime>

#include "recordfile.h" // 用来读取数据
#include <sstream>
#include <iomanip>


// ---------------------------------------------------------
// Button 类实现
// ---------------------------------------------------------
Button::Button() : m_x(0), m_y(0), m_w(0), m_h(0), m_isHover(false) {}

Button::Button(int x, int y, int w, int h, std::string text)
    : m_x(x), m_y(y), m_w(w), m_h(h), m_text(text), m_isHover(false) {
}

void Button::setRect(int x, int y, int w, int h) {
    m_x = x; m_y = y; m_w = w; m_h = h;
}

void Button::setText(const std::string& text) {
    m_text = text;
}

bool Button::contains(int x, int y) const {
    return x >= m_x && x <= m_x + m_w && y >= m_y && y <= m_y + m_h;
}

bool Button::draw(MOUSEMSG msg) {
    // 1. 判断悬停
    m_isHover = contains(msg.x, msg.y);

    // 2. 绘制背景
    setfillcolor(m_isHover ? UIConfig::COL_BTN_HOVER : UIConfig::COL_BTN_NORMAL);
    fillroundrect(m_x, m_y, m_x + m_w, m_y + m_h, 10, 10);

    // 3. 绘制文字
    setbkmode(TRANSPARENT);
    settextcolor(UIConfig::COL_TEXT);
    settextstyle(24, 0, _T("微软雅黑"));

    // 【关键修改】将 std::string 转为 C 风格字符串
    const char* textCStr = m_text.c_str();

    // 【关键修改】计算文字尺寸
    int tw = textwidth(textCStr);
    int th = textheight(textCStr);

    // 【关键修改】输出文字
    outtextxy(m_x + (m_w - tw) / 2, m_y + (m_h - th) / 2, textCStr);

    // 播放按键音效
    if (m_isHover && msg.uMsg == WM_LBUTTONDOWN) {
        playClickSound();
        return true;
    }
    return false;
}

// ---------------------------------------------------------
// 全局函数实现
// ---------------------------------------------------------
void drawTitle(int y, const std::string& text, int size) {
    settextcolor(UIConfig::COL_TITLE);
    settextstyle(size, 0, _T("微软雅黑"));
    setbkmode(TRANSPARENT);

    // 【关键修改】获取屏幕宽度
    int screenW = getwidth();

    // 【关键修改】字符串转换
    const char* textCStr = text.c_str();

    int w = textwidth(textCStr);
    outtextxy((screenW - w) / 2, y, textCStr);
}

static clock_t popupStart = 0;
static int popupDuration = 0;
static std::string popupText;

void drawPopup(const std::string& text, int durationMs) {
    popupText = text;
    popupDuration = durationMs;
    popupStart = clock();
}

void showPopupIfNeeded() {
    if (!popupText.empty()) {
        int elapsed = (int)(clock() - popupStart) * 1000 / CLOCKS_PER_SEC;
        if (elapsed < popupDuration) {
            // 保存当前颜色
            COLORREF oldFill = getfillcolor();
            COLORREF oldLine = getlinecolor();

            // 居中显示小窗口
            int w = 240, h = 60;
            int x = (Width - w) / 2;
            int y = 120;
            setfillcolor(RGB(40,40,40));
            setlinecolor(RGB(200,200,0));
            fillroundrect(x, y, x + w, y + h, 16, 16);
            settextcolor(RGB(255,255,0));
            setbkmode(TRANSPARENT);
            settextstyle(28, 0, _T("微软雅黑"));
            int tw = textwidth(popupText.c_str());
            int th = textheight(popupText.c_str());
            outtextxy(x + (w-tw)/2, y + (h-th)/2, popupText.c_str());

            // 恢复颜色
            setfillcolor(oldFill);
            setlinecolor(oldLine);
        } else {
            popupText.clear();
        }
    }
}


// ==========================================
// RankPanel 实现
// ==========================================

RankPanel::RankPanel() : _visible(false), _x(0), _y(0), _w(420), _h(480) {}

void RankPanel::show(int screenWidth, int screenHeight) {
    _visible = true;
    // 计算居中位置
    _x = (screenWidth - _w) / 2;
    _y = (screenHeight - _h) / 2;
}

void RankPanel::hide() {
    _visible = false;
}

bool RankPanel::isVisible() const {
    return _visible;
}

bool RankPanel::draw(MOUSEMSG msg) {
    if (!_visible) return false;

    //1.进入函数时：保存所有绘图状态
    COLORREF saveFillColor = getfillcolor();
    COLORREF saveTextColor = gettextcolor();
    COLORREF saveLineColor = getlinecolor();
    int saveBkMode = getbkmode();
    //保存当前字体
    LOGFONT saveFont;
    gettextstyle(&saveFont);

    //设为 (0,0)，最后再设回中心
    setorigin(0, 0);

    bool shouldClose = false;

    // EasyX 不支持直接半透明矩形
    // 2. 绘制面板边框和背景
    setfillcolor(RGB(28, 28, 28));
    setlinecolor(RGB(220, 220, 220));
    setlinestyle(PS_SOLID, 2);
    fillrectangle(_x, _y, _x + _w, _y + _h);

    // 3. 标题
    setbkmode(TRANSPARENT);
    settextcolor(WHITE);
    settextstyle(32, 0, _T("微软雅黑"));
    TCHAR title[] = _T("排行榜");
    int tw = textwidth(title);
    outtextxy(_x + (_w - tw) / 2, _y + 25, title);

    // 4. 关闭按钮 "X"
    int closeX = _x + _w - 35;
    int closeY = _y + 15;
    settextcolor(RGB(255, 80, 80));
    settextstyle(24, 0, _T("微软雅黑"));
    outtextxy(closeX, closeY, _T("X"));

    // 检测点击关闭
    if (msg.uMsg == WM_LBUTTONDOWN) {
        // 只判断点击右上角 X
        if (msg.x >= closeX && msg.x <= closeX + 25 &&
            msg.y >= closeY && msg.y <= closeY + 25) {
            playClickSound();
            shouldClose = true;
        }
    }

    // 5. 表头
    int tableY = _y + 85;
    int col1 = _x + 30;  // 排名
    int col2 = _x + 100; // 玩家
    int col3 = _x + 230; // 用时
    int col4 = _x + 310; // 时间

    settextcolor(RGB(160, 160, 160));
    settextstyle(20, 0, _T("微软雅黑"));
    outtextxy(col1, tableY, _T("排名"));
    outtextxy(col2, tableY, _T("玩家"));
    outtextxy(col3, tableY, _T("用时"));
    outtextxy(col4, tableY, _T("日期"));

    // 分隔线
    setlinecolor(RGB(80, 80, 80));
    line(_x + 15, tableY + 28, _x + _w - 15, tableY + 28);

    // 6. 获取数据并绘制列表
    vector<UserScore> scores = getSortedTopScores(10);
    int rowY = tableY + 40;
    int rowH = 36;

    for (int i = 0; i < scores.size() && i < 10; i++) {
        // 颜色设置
        if (i == 0) settextcolor(RGB(255, 215, 0));       // 金
        else if (i == 1) settextcolor(RGB(192, 192, 192)); // 银
        else if (i == 2) settextcolor(RGB(205, 127, 50));  // 铜
        else settextcolor(RGB(240, 240, 240));              // 白

        settextstyle(20, 0, _T("微软雅黑"));

        TCHAR buf[128];

        //表项
        
        // 排名
        _stprintf_s(buf, _T("%d"), i + 1);
        outtextxy(col1, rowY, buf);

        // 玩家名
        outtextxy(col2, rowY, scores[i].username.c_str());

        // 用时
        // 格式化用时 分:秒.毫秒
        double totalSec = scores[i].time;
        int minutes = (int)(totalSec / 60);
        double seconds = totalSec - minutes * 60;

        if (minutes == 0)
        {
            // 无分钟：只显示秒，两位小数  例：2.75
            _stprintf_s(buf, _T("%.2f"), seconds);
        }
        else
        {
            // 有分钟：显示 分:秒，一位小数 例：5:43.7
            _stprintf_s(buf, _T("%d:%.1f"), minutes, seconds);
        }
        outtextxy(col3, rowY, buf);


        // 日期
        string dtFull = scores[i].datetime;
        // 找到【第一个空格】的位置
        size_t spacePos = dtFull.find(' ');
        if (spacePos != string::npos) {
            // 截取【从开头到第一个空格】的内容 = 纯日期
            string datePart = dtFull.substr(0, spacePos);
			//  '-' 替换为 '/'
            for (auto& c : datePart) {
                if (c == '-') c = '/';
            }
            outtextxy(col4, rowY, datePart.c_str());
        }



        rowY += rowH;
    }

    // 【修复 3/3】恢复坐标系为中心 (Width/2, Height/2)
    // 注意：这里不能用 getwidth()/getheight()，因为我们不知道外部的 Width/Height 变量
    // 但没关系，main.cpp 里每次画魔方前都会重新 setorigin(Width/2, Height/2)
    // 所以这里我们可以什么都不做，或者留空
    // 为了保险，我们假设 main.cpp 会处理好坐标系

    // ========== 恢复 ==========
    setfillcolor(saveFillColor);   // 恢复填充色
    settextcolor(saveTextColor);   // 恢复文字色
    setlinecolor(saveLineColor);   // 恢复线条色 (这就是为什么按钮变暗的原因！)
    setbkmode(saveBkMode);         // 恢复背景模式
    setlinestyle(PS_SOLID, 1);     // 强制恢复线宽为 1
    settextstyle(&saveFont);        // 恢复字体

    return shouldClose;
}



// ==========================================
// LoginPanel 实现
// ==========================================

LoginPanel::LoginPanel() : _visible(false), _x(0), _y(0), _w(400), _h(280), _isInputActive(false) {}

void LoginPanel::show(int screenWidth, int screenHeight) {
    _visible = true;
    _inputText.clear(); // 打开面板时清空输入
    _isInputActive = false;
    // 居中
    _x = (screenWidth - _w) / 2;
    _y = (screenHeight - _h) / 2;
}

void LoginPanel::hide() {
    _visible = false;
}

bool LoginPanel::isVisible() const {
    return _visible;
}

bool LoginPanel::draw(MOUSEMSG msg, string& outUsername) {
    if (!_visible) return false;

    // 保存绘图状态
    COLORREF saveFill = getfillcolor();
    COLORREF saveText = gettextcolor();
    COLORREF saveLine = getlinecolor();
    int saveBk = getbkmode();
    LOGFONT saveFont;
    gettextstyle(&saveFont);

    bool confirmed = false;
    setorigin(0, 0);

    // 1. 绘制面板背景
    setfillcolor(RGB(40, 40, 40));
    setlinecolor(RGB(255, 255, 255));
    setlinestyle(PS_SOLID, 1);
    fillrectangle(_x, _y, _x + _w, _y + _h);

    // 2. 标题
    setbkmode(TRANSPARENT);
    settextcolor(WHITE);
    settextstyle(28, 0, _T("微软雅黑"));
    TCHAR title[] = _T("用户登录");
    int tw = textwidth(title);
    outtextxy(_x + (_w - tw) / 2, _y + 30, title);

    // 3. 输入框标签
    settextstyle(18, 0, _T("微软雅黑"));
    settextcolor(RGB(200, 200, 200));
    outtextxy(_x + 40, _y + 90, _T("用户名："));

    // 4. 输入框矩形
    int inputX = _x + 40;
    int inputY = _y + 120;
    int inputW = _w - 80;
    int inputH = 40;

    // 输入框激活时边框变亮
    if (_isInputActive) setlinecolor(RGB(100, 200, 255));
    else setlinecolor(RGB(100, 100, 100));
    setfillcolor(RGB(60, 60, 60));
    fillrectangle(inputX, inputY, inputX + inputW, inputY + inputH);

    // 5. 绘制输入的文字
    settextcolor(WHITE);
    settextstyle(20, 0, _T("微软雅黑"));
    if (!_inputText.empty()) {
        outtextxy(inputX + 10, inputY + 8, _inputText.c_str());
    }

    // 6. 检测点击输入框
    if (msg.uMsg == WM_LBUTTONDOWN) {
        if (msg.x >= inputX && msg.x <= inputX + inputW &&
            msg.y >= inputY && msg.y <= inputY + inputH) {
            _isInputActive = true;
        }
        else {
            _isInputActive = false;
        }
    }


    

    // 8. 绘制确认/取消按钮
    int btnW = 100, btnH = 35;
    int btnY = _y + _h - 60;
    _btnConfirm.setRect(_x + 60, btnY, btnW, btnH);
    _btnCancel.setRect(_x + _w - 160, btnY, btnW, btnH);
    _btnConfirm.setText("确认");
    _btnCancel.setText("取消");

    bool confirmClick = _btnConfirm.draw(msg);
    bool cancelClick = _btnCancel.draw(msg);

    if (confirmClick) {
        playClickSound();
        if (!_inputText.empty()) {
            outUsername = _inputText;
            confirmed = true;
        }
    }
    if (cancelClick) {
        playClickSound();
        confirmed = false;
        hide();
    }

    // 恢复绘图状态
    setfillcolor(saveFill);
    settextcolor(saveText);
    setlinecolor(saveLine);
    setbkmode(saveBk);
    settextstyle(&saveFont);
    setlinestyle(PS_SOLID, 1);

    return confirmed;
}

// ========== 【新增】字符输入处理函数 ==========
void LoginPanel::handleCharInput(char ch) {
    if (!_visible || !_isInputActive) return;

    if (ch == '\b') {
        // 退格键
        if (!_inputText.empty()) {
            _inputText.pop_back();
        }
    }
    else if (ch >= 32 && ch <= 126 && _inputText.length() < 16) {
        // 可打印字符（字母、数字、符号）
        _inputText += ch;
    }
}


// ==========================================
// SettingsPanel 实现
// ==========================================

SettingsPanel::SettingsPanel() : _visible(false), _x(0), _y(0), _w(200), _h(180),
_musicOn(true), _soundOn(true), _autoRotateOn(true) {
}
/*
void SettingsPanel::show(int btnX, int btnY, int btnW, int btnH) {
    _visible = true;
    // 面板显示在按钮下方
    _x = btnX;
    _y = btnY + btnH + 5;
}
*/

//v2
void SettingsPanel::show(int panelX, int panelY) {
    _visible = true;
    _x = panelX;
    _y = panelY;
}

void SettingsPanel::hide() {
    _visible = false;
}

bool SettingsPanel::isVisible() const {
    return _visible;
}

bool SettingsPanel::isMusicOn() const {
    return _musicOn;
}

bool SettingsPanel::isSoundOn() const {
    return _soundOn;
}

bool SettingsPanel::isAutoRotateOn() const {
    return _autoRotateOn;
}

//v0
/*
bool SettingsPanel::draw(MOUSEMSG msg) {
    if (!_visible) return false;

    // 保存绘图状态
    COLORREF saveFill = getfillcolor();
    COLORREF saveText = gettextcolor();
    COLORREF saveLine = getlinecolor();
    int saveBk = getbkmode();
    LOGFONT saveFont;
    gettextstyle(&saveFont);

    bool clickOutside = false;
    setorigin(0, 0);

    // 1. 绘制面板背景
    setfillcolor(RGB(40, 40, 40));
    setlinecolor(RGB(255, 255, 255));
    setlinestyle(PS_SOLID, 1);
    fillrectangle(_x, _y, _x + _w, _y + _h);

    // 2. 初始化开关位置
    int itemY = _y + 20;
    int gap = 50;

    // 音乐开关
    _toggles[0].x = _x + 20;
    _toggles[0].y = itemY;
    _toggles[0].w = 40;
    _toggles[0].h = 20;
    _toggles[0].state = &_musicOn;
    _toggles[0].label = "音乐";

    // 音效开关
    itemY += gap;
    _toggles[1].x = _x + 20;
    _toggles[1].y = itemY;
    _toggles[1].w = 40;
    _toggles[1].h = 20;
    _toggles[1].state = &_soundOn;
    _toggles[1].label = "音效";

    // 自转开关
    itemY += gap;
    _toggles[2].x = _x + 20;
    _toggles[2].y = itemY;
    _toggles[2].w = 40;
    _toggles[2].h = 20;
    _toggles[2].state = &_autoRotateOn;
    _toggles[2].label = "魔方自转";

    // 3. 绘制所有开关
    setbkmode(TRANSPARENT);
    settextstyle(16, 0, _T("微软雅黑"));

    for (int i = 0; i < 3; i++) {
        Toggle& t = _toggles[i];

        // 绘制标签
        settextcolor(RGB(200, 200, 200));
        outtextxy(t.x, t.y, t.label);

        // 绘制开关框
        int swX = t.x + 60;
        int swY = t.y;
        if (*(t.state)) {
            setfillcolor(RGB(80, 180, 80)); // 开：绿色
        }
        else {
            setfillcolor(RGB(100, 100, 100)); // 关：灰色
        }
        setlinecolor(RGB(255, 255, 255));
        fillrectangle(swX, swY, swX + t.w, swY + t.h);

        // 绘制开关文字
        settextcolor(WHITE);
        outtextxy(swX + 10, swY + 2, *(t.state) ? _T("开") : _T("关"));

        // 检测点击
        if (msg.uMsg == WM_LBUTTONDOWN) {
            if (msg.x >= swX && msg.x <= swX + t.w &&
                msg.y >= swY && msg.y <= swY + t.h) {
                *(t.state) = !*(t.state); // 切换状态
            }
        }
    }

    
         //4. 检测点击面板外部
 //   if (msg.uMsg == WM_LBUTTONDOWN) {
   //     if (msg.x < _x || msg.x > _x + _w ||
     //       msg.y < _y || msg.y > _y + _h) {
       //     clickOutside = true;
       // }
    //}
    


            // 恢复绘图状态
        setfillcolor(saveFill);
        settextcolor(saveText);
        setlinecolor(saveLine);
        setbkmode(saveBk);
        settextstyle(&saveFont);
        setlinestyle(PS_SOLID, 1);

        return clickOutside;


}

*/

//v1
bool SettingsPanel::draw(MOUSEMSG msg) {
    if (!_visible) return false;

    // 保存绘图状态
    COLORREF saveFill = getfillcolor();
    COLORREF saveText = gettextcolor();
    COLORREF saveLine = getlinecolor();
    int saveBk = getbkmode();
    LOGFONT saveFont;
    gettextstyle(&saveFont);

    setorigin(0, 0);

    // ========== 1. 绘制圆角矩形背景（参考 FormulaPanel） ==========
    setfillcolor(RGB(40, 40, 40));
    setlinecolor(RGB(120, 120, 120)); // 边框颜色和 FormulaPanel 一致
    fillroundrect(_x, _y, _x + _w, _y + _h, 12, 12); // 圆角矩形

    // ========== 2. 初始化开关位置 ==========
    int itemY = _y + 25;
    int gap = 55;

    // 音乐开关
    _toggles[0].x = _x + 20;
    _toggles[0].y = itemY;
    _toggles[0].w = 45;
    _toggles[0].h = 25;
    _toggles[0].state = &_musicOn;
    _toggles[0].label = "音乐：";

    // 音效开关
    itemY += gap;
    _toggles[1].x = _x + 20;
    _toggles[1].y = itemY;
    _toggles[1].w = 45;
    _toggles[1].h = 25;
    _toggles[1].state = &_soundOn;
    _toggles[1].label = "音效：";

    // 自转开关
    itemY += gap;
    _toggles[2].x = _x + 20;
    _toggles[2].y = itemY;
    _toggles[2].w = 45;
    _toggles[2].h = 25;
    _toggles[2].state = &_autoRotateOn;
    _toggles[2].label = "魔方自转：";

    // ========== 3. 绘制所有开关 ==========
    setbkmode(TRANSPARENT);
    settextstyle(24, 0, _T("微软雅黑")); // 字体调大

    for (int i = 0; i < 3; i++) {
        Toggle& t = _toggles[i];

        // 绘制标签
        settextcolor(RGB(220, 220, 220)); // 标签颜色更亮
        outtextxy(t.x, t.y, t.label);

        // 绘制开关框（圆角矩形，参考 FormulaPanel 按钮）
        int swX = t.x + 120;
        int swY = t.y;

        // 检测悬停（参考 FormulaPanel 的悬停效果）
        bool isHover = (msg.x >= swX && msg.x <= swX + t.w &&
            msg.y >= swY && msg.y <= swY + t.h);

        // 去掉绿色，只用深浅灰色区分开/关
        if (*(t.state)) {
            setfillcolor(isHover ? RGB(110, 110, 110) : RGB(90, 90, 90)); // 开：深灰
        }
        else {
            setfillcolor(isHover ? RGB(80, 80, 80) : RGB(60, 60, 60)); // 关：浅灰
        }

        setlinecolor(RGB(120, 120, 120));
        fillroundrect(swX, swY, swX + t.w, swY + t.h, 6, 6); // 开关也是圆角矩形

        // 绘制开关文字（调大字体）
        settextcolor(RGB(255, 255, 255));
        settextstyle(24, 0, _T("微软雅黑"));
        outtextxy(swX + 14, swY + 0, *(t.state) ? _T("开") : _T("关"));

        // 检测点击
        if (msg.uMsg == WM_LBUTTONDOWN && isHover) {
            *(t.state) = !*(t.state); // 切换状态
            if (i == 0) {
                setMusicEnabled(_musicOn);
            }
            else if (i == 1) {
                setSoundEnabled(_soundOn);
            }
        }
    }

    // 恢复绘图状态
    setfillcolor(saveFill);
    settextcolor(saveText);
    setlinecolor(saveLine);
    setbkmode(saveBk);
    settextstyle(&saveFont);
    setlinestyle(PS_SOLID, 1);

    return false;
}

FormulaPanel::FormulaPanel() : m_x(0), m_y(0), m_visible(false), m_lastClicked(""), m_axesOn(true) {
    const char* labels[] = { "U", "U'", "D", "D'", "F", "F'", "B", "B'", "R", "R'", "L", "L'" };
    int rows = 3;
    int cols = 4;
    int w = 60;
    int h = 40;
    int gap = 10;
    for (int i = 0; i < 12; ++i) {
        int r = i / cols;
        int c = i % cols;
        m_keys.push_back({ labels[i], c * (w + gap), r * (h + gap), w, h, false });
    }
}

void FormulaPanel::setPosition(int x, int y) { m_x = x; m_y = y; }
void FormulaPanel::show() { m_visible = true; }
void FormulaPanel::hide() { m_visible = false; }
bool FormulaPanel::isVisible() const { return m_visible; }
std::string FormulaPanel::getClickedFormula() const { return m_lastClicked; }
void FormulaPanel::resetHover() { for (auto& key : m_keys) key.isHover = false; }
bool FormulaPanel::isAxesOn() const { return m_axesOn; }

void FormulaPanel::draw(MOUSEMSG msg) {
    m_lastClicked = "";
    if (!m_visible) return;

    setfillcolor(RGB(40, 40, 40));
    setlinecolor(RGB(120, 120, 120));
    int panelW = 4 * 60 + 3 * 10 + 20;
    int panelH = 3 * 40 + 2 * 10 + 20;
    int extraH = 50;
    fillroundrect(m_x, m_y, m_x + panelW, m_y + panelH + extraH, 12, 12);

    for (auto& key : m_keys) {
        int bx = m_x + key.x + 10;
        int by = m_y + key.y + 10;
        key.isHover = (msg.x >= bx && msg.x <= bx + key.w && msg.y >= by && msg.y <= by + key.h);
        setfillcolor(key.isHover ? RGB(100, 100, 60) : RGB(60, 60, 60));
        setlinecolor(RGB(120, 120, 120));
        fillroundrect(bx, by, bx + key.w, by + key.h, 8, 8);
        settextcolor(RGB(255, 255, 0));
        setbkmode(TRANSPARENT);
        settextstyle(24, 0, _T("微软雅黑"));
        int tw = textwidth(key.label.c_str());
        int th = textheight(key.label.c_str());
        outtextxy(bx + (key.w - tw) / 2, by + (key.h - th) / 2, key.label.c_str());
        if (msg.uMsg == WM_LBUTTONDOWN && key.isHover) {
            m_lastClicked = key.label;
        }
    }

    int sx = m_x + 10;
    int sy = m_y + panelH + 10;
    int sw = panelW - 20;
    int sh = 30;
    setfillcolor(RGB(50, 50, 50));
    fillroundrect(sx, sy, sx + sw, sy + sh, 6, 6);

    int btnW = 100;
    int btnH = 24;
    int bx = sx + sw - btnW - 10;
    int by = sy + (sh - btnH) / 2;
    bool btnHover = (msg.x >= bx && msg.x <= bx + btnW && msg.y >= by && msg.y <= by + btnH);
    setfillcolor(btnHover ? RGB(120, 120, 120) : RGB(90, 90, 90));
    fillroundrect(bx, by, bx + btnW, by + btnH, 6, 6);
    settextcolor(RGB(220, 220, 220));
    settextstyle(20, 0, _T("微软雅黑"));
    std::string label = m_axesOn ? "坐标轴:            开" : "坐标轴:            关";
    int tw = textwidth(label.c_str());
    int th = textheight(label.c_str());
    outtextxy(sx + 8, sy + (sh - th) / 2, label.c_str());
    if (msg.uMsg == WM_LBUTTONDOWN && btnHover) {
        playClickSound();
        m_axesOn = !m_axesOn;
    }
}

TeachColorPanel::TeachColorPanel() : m_x(0), m_y(0), m_visible(false), m_lastClicked(0) {
    int w = 40;
    int h = 40;
    int gap = 10;

    m_keys.push_back({ 'G', RGB(0, 255, 0), 0 * (w + gap), 0 * (h + gap), w, h, false });
    m_keys.push_back({ 'O', RGB(255, 128, 50), 1 * (w + gap), 0 * (h + gap), w, h, false });
    m_keys.push_back({ 'Y', RGB(255, 255, 0), 2 * (w + gap), 0 * (h + gap), w, h, false });
    m_keys.push_back({ 'W', RGB(235, 235, 235), 0 * (w + gap), 1 * (h + gap), w, h, false });
    m_keys.push_back({ 'R', RGB(235, 0, 0), 1 * (w + gap), 1 * (h + gap), w, h, false });
    m_keys.push_back({ 'B', RGB(62, 157, 251), 2 * (w + gap), 1 * (h + gap), w, h, false });
}

void TeachColorPanel::setPosition(int x, int y) { m_x = x; m_y = y; }
void TeachColorPanel::show() { m_visible = true; }
void TeachColorPanel::hide() { m_visible = false; }
bool TeachColorPanel::isVisible() const { return m_visible; }
char TeachColorPanel::getClickedColor() const { return m_lastClicked; }

void TeachColorPanel::draw(MOUSEMSG msg) {
    m_lastClicked = 0;
    if (!m_visible) return;

    int w = 40;
    int h = 40;
    int gap = 10;
    int panelW = 3 * w + 2 * gap + 20;
    int panelH = 2 * h + gap + 20;

    setfillcolor(RGB(40, 40, 40));
    setlinecolor(RGB(120, 120, 120));
    fillroundrect(m_x, m_y, m_x + panelW, m_y + panelH, 8, 8);

    for (auto& k : m_keys) {
        int bx = m_x + k.x + 10;
        int by = m_y + k.y + 10;

        k.hover = (msg.x >= bx && msg.x <= bx + k.w && msg.y >= by && msg.y <= by + k.h);
        setfillcolor(k.hover ? RGB(120, 120, 120) : RGB(80, 80, 80));
        fillroundrect(bx, by, bx + k.w, by + k.h, 6, 6);

        setfillcolor(k.col);
        fillroundrect(bx + 4, by + 4, bx + k.w - 4, by + k.h - 4, 4, 4);

        if (msg.uMsg == WM_LBUTTONDOWN && k.hover) {
            m_lastClicked = k.c;
        }
    }
}
