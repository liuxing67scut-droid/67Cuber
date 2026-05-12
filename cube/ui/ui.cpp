#include "ui.h"
#include "cube_colors.h"
#include "music.h"
#include <ctime>

#include "recordfile.h"
#include <sstream>
#include <iomanip>

Button::Button() : m_x(0), m_y(0), m_w(0), m_h(0), m_isHover(false), m_style(BUTTON_STYLE_NORMAL) {}

Button::Button(int x, int y, int w, int h, std::string text)
    : m_x(x), m_y(y), m_w(w), m_h(h), m_text(text), m_isHover(false), m_style(BUTTON_STYLE_NORMAL) {
}

void Button::setRect(int x, int y, int w, int h) {
    m_x = x; m_y = y; m_w = w; m_h = h;
}

void Button::setText(const std::string& text) {
    m_text = text;
}

void Button::setVisualStyle(ButtonVisualStyle style) {
    m_style = style;
}

bool Button::contains(int x, int y) const {
    return x >= m_x && x <= m_x + m_w && y >= m_y && y <= m_y + m_h;
}

//绘制按钮并返回本帧是否点击
bool Button::draw(MOUSEMSG msg) {
    m_isHover = contains(msg.x, msg.y);

    bool isLargeStyle = (m_style == BUTTON_STYLE_LARGE);
    COLORREF fillColor = isLargeStyle
        ? (m_isHover ? UIConfig::COL_LARGE_BTN_HOVER : UIConfig::COL_LARGE_BTN_NORMAL)
        : (m_isHover ? UIConfig::COL_BTN_HOVER : UIConfig::COL_BTN_NORMAL);
    COLORREF borderColor = isLargeStyle ? UIConfig::COL_LARGE_BTN_BORDER : UIConfig::COL_BTN_BORDER;
    COLORREF textColor = isLargeStyle ? UIConfig::COL_LARGE_BTN_TEXT : UIConfig::COL_TEXT;
    int borderWidth = isLargeStyle ? UIConfig::LARGE_BTN_BORDER_WIDTH : UIConfig::NORMAL_BTN_BORDER_WIDTH;
    int textSize = isLargeStyle ? UIConfig::LARGE_BTN_TEXT_SIZE : 24;

    setfillcolor(fillColor);
    setlinecolor(borderColor);
    setlinestyle(PS_SOLID, borderWidth);
    fillroundrect(m_x, m_y, m_x + m_w, m_y + m_h, 10, 10);

    setbkmode(TRANSPARENT);
    settextcolor(textColor);

    if (isLargeStyle) {
        LOGFONT font = { 0 };
        font.lfHeight = textSize;
        font.lfWeight = FW_BOLD;
        _tcscpy_s(font.lfFaceName, _T("微软雅黑"));
        settextstyle(&font);
    }
    else {
        settextstyle(textSize, 0, _T("微软雅黑"));
    }

    const char* textCStr = m_text.c_str();

    int tw = textwidth(textCStr);
    int th = textheight(textCStr);

    outtextxy(m_x + (m_w - tw) / 2, m_y + (m_h - th) / 2, textCStr);
    setlinestyle(PS_SOLID, 1);

    if (m_isHover && msg.uMsg == WM_LBUTTONDOWN) {
        playClickSound();
        return true;
    }
    return false;
}

//绘制居中标题
void drawTitle(int y, const std::string& text, int size) {
    settextcolor(UIConfig::COL_TITLE);
    settextstyle(size, 0, _T("微软雅黑"));
    setbkmode(TRANSPARENT);

    int screenW = getwidth();

    const char* textCStr = text.c_str();

    int w = textwidth(textCStr);
    outtextxy((screenW - w) / 2, y, textCStr);
}

static clock_t popupStart = 0;
static int popupDuration = 0;
static std::string popupText;

//记录需要短暂显示的弹窗文本
void drawPopup(const std::string& text, int durationMs) {
    popupText = text;
    popupDuration = durationMs;
    popupStart = clock();
}

//按持续时间绘制弹窗提示
void showPopupIfNeeded() {
    if (!popupText.empty()) {
        int elapsed = (int)(clock() - popupStart) * 1000 / CLOCKS_PER_SEC;
        if (elapsed < popupDuration) {
            COLORREF oldFill = getfillcolor();
            COLORREF oldLine = getlinecolor();

            int w = 240, h = 60;
            int x = (Width - w) / 2;
            int y = 120;
            setfillcolor(UIConfig::COL_POPUP_FILL);
            setlinecolor(UIConfig::COL_POPUP_BORDER);
            fillroundrect(x, y, x + w, y + h, 16, 16);
            settextcolor(UIConfig::COL_POPUP_TEXT);
            setbkmode(TRANSPARENT);
            settextstyle(28, 0, _T("微软雅黑"));
            int tw = textwidth(popupText.c_str());
            int th = textheight(popupText.c_str());
            outtextxy(x + (w-tw)/2, y + (h-th)/2, popupText.c_str());

            setfillcolor(oldFill);
            setlinecolor(oldLine);
        } else {
            popupText.clear();
        }
    }
}


RankPanel::RankPanel() : _visible(false), _x(0), _y(0), _w(460), _h(540), _scrollOffset(0) {}

void RankPanel::show(int screenWidth, int screenHeight) {
    _visible = true;
    _scrollOffset = 0;
    _x = (screenWidth - _w) / 2;
    _y = (screenHeight - _h) / 2;
}

void RankPanel::hide() {
    _visible = false;
}

bool RankPanel::isVisible() const {
    return _visible;
}

//绘制排行榜面板，并返回是否点击关闭按钮
bool RankPanel::draw(MOUSEMSG msg) {
    if (!_visible) return false;

    COLORREF saveFillColor = getfillcolor();
    COLORREF saveTextColor = gettextcolor();
    COLORREF saveLineColor = getlinecolor();
    int saveBkMode = getbkmode();
    LOGFONT saveFont;
    gettextstyle(&saveFont);

    //面板按屏幕坐标绘制
    setorigin(0, 0);

    bool shouldClose = false;

    const int panelPadding = 32;
    setfillcolor(UIConfig::COL_PANEL_FILL);
    setlinecolor(UIConfig::COL_PANEL_BORDER);
    setlinestyle(PS_SOLID, 1);
    fillroundrect(_x, _y, _x + _w, _y + _h, 14, 14);

    setbkmode(TRANSPARENT);
    settextcolor(UIConfig::COL_PANEL_TEXT);
    settextstyle(30, 0, _T("微软雅黑"));
    TCHAR title[] = _T("排行榜");
    int tw = textwidth(title);
    outtextxy(_x + (_w - tw) / 2, _y + 30, title);

    int closeSize = 28;
    int closeX = _x + _w - panelPadding - closeSize;
    int closeY = _y + 22;
    bool closeHover = msg.x >= closeX && msg.x <= closeX + closeSize &&
        msg.y >= closeY && msg.y <= closeY + closeSize;

    setfillcolor(closeHover ? UIConfig::COL_CLOSE_HOVER : UIConfig::COL_CLOSE_FILL);
    setlinecolor(UIConfig::COL_CLOSE_TEXT);
    fillroundrect(closeX, closeY, closeX + closeSize, closeY + closeSize, 8, 8);

    settextcolor(UIConfig::COL_CLOSE_TEXT);
    settextstyle(20, 0, _T("微软雅黑"));
    TCHAR closeText[] = _T("X");
    int closeTextW = textwidth(closeText);
    int closeTextH = textheight(closeText);
    outtextxy(closeX + (closeSize - closeTextW) / 2, closeY + (closeSize - closeTextH) / 2, closeText);

    if (msg.uMsg == WM_LBUTTONDOWN) {
        if (closeHover) {
            playClickSound();
            shouldClose = true;
        }
    }

    vector<UserScore> scores = getSortedScores();
    const int visibleRows = 10;
    const int scrollStep = 3;
    int maxOffset = (int)scores.size() - visibleRows;
    if (maxOffset < 0) {
        maxOffset = 0;
    }

    if (msg.uMsg == WM_MOUSEWHEEL && maxOffset > 0) {
        if (msg.wheel > 0) {
            _scrollOffset -= scrollStep;
        }
        else if (msg.wheel < 0) {
            _scrollOffset += scrollStep;
        }

        if (_scrollOffset < 0) {
            _scrollOffset = 0;
        }
        if (_scrollOffset > maxOffset) {
            _scrollOffset = maxOffset;
        }
    }

    int tableY = _y + 98;
    int col1 = _x + panelPadding + 5;
    int col2 = _x + 115;
    int col3 = _x + 275;
    int col4 = _x + 350;

    settextcolor(UIConfig::COL_PANEL_MUTED);
    settextstyle(20, 0, _T("微软雅黑"));
    outtextxy(col1, tableY, _T("排名"));
    outtextxy(col2, tableY, _T("玩家"));
    outtextxy(col3, tableY, _T("用时"));
    outtextxy(col4, tableY, _T("日期"));

    setlinecolor(UIConfig::COL_PANEL_LINE);
    line(_x + panelPadding, tableY + 28, _x + _w - panelPadding, tableY + 28);

    int rowY = tableY + 40;
    int rowH = 36;
    int endIndex = _scrollOffset + visibleRows;
    if (endIndex > (int)scores.size()) {
        endIndex = (int)scores.size();
    }

    for (int i = _scrollOffset; i < endIndex; i++) {
        if (i == 0) settextcolor(UIConfig::COL_RANK_GOLD);
        else if (i == 1) settextcolor(UIConfig::COL_RANK_SILVER);
        else if (i == 2) settextcolor(UIConfig::COL_RANK_BRONZE);
        else settextcolor(UIConfig::COL_PANEL_TEXT);

        settextstyle(20, 0, _T("微软雅黑"));

        TCHAR buf[128];

        _stprintf_s(buf, _T("%d"), i + 1);
        outtextxy(col1, rowY, buf);

        outtextxy(col2, rowY, scores[i].username.c_str());

        double totalSec = scores[i].time;
        int minutes = (int)(totalSec / 60);
        double seconds = totalSec - minutes * 60;

        if (minutes == 0)
        {
            _stprintf_s(buf, _T("%.2f"), seconds);
        }
        else
        {
            _stprintf_s(buf, _T("%d:%.1f"), minutes, seconds);
        }
        outtextxy(col3, rowY, buf);

        string dtFull = scores[i].datetime;
        size_t spacePos = dtFull.find(' ');
        if (spacePos != string::npos) {
            string datePart = dtFull.substr(0, spacePos);
            for (auto& c : datePart) {
                if (c == '-') c = '/';
            }
            outtextxy(col4, rowY, datePart.c_str());
        }
        rowY += rowH;
    }

    settextstyle(18, 0, _T("微软雅黑"));
    settextcolor(UIConfig::COL_PANEL_MUTED);
    TCHAR pageText[64];
    if (scores.empty()) {
        _stprintf_s(pageText, _T("暂无成绩"));
    }
    else if (maxOffset == 0) {
        _stprintf_s(pageText, _T("共 %d 条成绩"), (int)scores.size());
    }
    else {
        _stprintf_s(pageText, _T("%d-%d / %d  滚轮查看更多"),
            _scrollOffset + 1, endIndex, (int)scores.size());
    }
    outtextxy(_x + panelPadding, _y + _h - 36, pageText);

    if (maxOffset > 0) {
        int barX = _x + _w - 18;
        int barTop = tableY + 40;
        int barBottom = _y + _h - 52;
        int trackH = barBottom - barTop;
        int thumbH = trackH * visibleRows / (int)scores.size();
        if (thumbH < 28) {
            thumbH = 28;
        }
        int thumbY = barTop + (_scrollOffset * (trackH - thumbH) / maxOffset);

        setlinecolor(UIConfig::COL_PANEL_LINE);
        line(barX, barTop, barX, barBottom);
        setfillcolor(UIConfig::COL_PANEL_MUTED);
        fillroundrect(barX - 3, thumbY, barX + 3, thumbY + thumbH, 4, 4);
    }

    setfillcolor(saveFillColor);
    settextcolor(saveTextColor);
    setlinecolor(saveLineColor);
    setbkmode(saveBkMode);
    setlinestyle(PS_SOLID, 1);
    settextstyle(&saveFont);

    return shouldClose;
}
LoginPanel::LoginPanel() : _visible(false), _x(0), _y(0), _w(400), _h(280), _isInputActive(false) {}

void LoginPanel::show(int screenWidth, int screenHeight) {
    _visible = true;
    _inputText.clear();
    _isInputActive = false;
    _x = (screenWidth - _w) / 2;
    _y = (screenHeight - _h) / 2;
}

void LoginPanel::hide() {
    _visible = false;
}

bool LoginPanel::isVisible() const {
    return _visible;
}

//绘制登录面板并处理确认、取消和输入框点击
bool LoginPanel::draw(MOUSEMSG msg, string& outUsername) {
    if (!_visible) return false;

    COLORREF saveFill = getfillcolor();
    COLORREF saveText = gettextcolor();
    COLORREF saveLine = getlinecolor();
    int saveBk = getbkmode();
    LOGFONT saveFont;
    gettextstyle(&saveFont);

    bool confirmed = false;
    setorigin(0, 0);

    setfillcolor(UIConfig::COL_PANEL_FILL);
    setlinecolor(UIConfig::COL_PANEL_BORDER);
    setlinestyle(PS_SOLID, 1);
    fillroundrect(_x, _y, _x + _w, _y + _h, 14, 14);

    setbkmode(TRANSPARENT);
    settextcolor(UIConfig::COL_PANEL_TEXT);
    settextstyle(28, 0, _T("微软雅黑"));
    TCHAR title[] = _T("用户登录");
    int tw = textwidth(title);
    outtextxy(_x + (_w - tw) / 2, _y + 30, title);

    settextstyle(18, 0, _T("微软雅黑"));
    settextcolor(UIConfig::COL_PANEL_TEXT);
    outtextxy(_x + 40, _y + 90, _T("用户名："));

    int inputX = _x + 40;
    int inputY = _y + 120;
    int inputW = _w - 80;
    int inputH = 40;

    if (_isInputActive) setlinecolor(UIConfig::COL_INPUT_ACTIVE);
    else setlinecolor(UIConfig::COL_INPUT_BORDER);
    setfillcolor(UIConfig::COL_INPUT_FILL);
    fillroundrect(inputX, inputY, inputX + inputW, inputY + inputH, 8, 8);

    settextcolor(UIConfig::COL_PANEL_TEXT);
    settextstyle(20, 0, _T("微软雅黑"));
    if (!_inputText.empty()) {
        outtextxy(inputX + 10, inputY + 8, _inputText.c_str());
    }

    if (msg.uMsg == WM_LBUTTONDOWN) {
        if (msg.x >= inputX && msg.x <= inputX + inputW &&
            msg.y >= inputY && msg.y <= inputY + inputH) {
            _isInputActive = true;
        }
        else {
            _isInputActive = false;
        }
    }
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

    setfillcolor(saveFill);
    settextcolor(saveText);
    setlinecolor(saveLine);
    setbkmode(saveBk);
    settextstyle(&saveFont);
    setlinestyle(PS_SOLID, 1);

    return confirmed;
}

//处理登录输入框的字符输入
void LoginPanel::handleCharInput(char ch) {
    if (!_visible || !_isInputActive) return;

    if (ch == '\b') {
        if (!_inputText.empty()) {
            _inputText.pop_back();
        }
    }
    else if (ch >= 32 && ch <= 126 && _inputText.length() < 16) {
        _inputText += ch;
    }
}

SettingsPanel::SettingsPanel() : _visible(false), _x(0), _y(0), _w(200), _h(180),
_musicOn(true), _soundOn(true), _autoRotateOn(true) {
}

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

//绘制设置面板并同步音乐、音效开关
bool SettingsPanel::draw(MOUSEMSG msg) {
    if (!_visible) return false;

    COLORREF saveFill = getfillcolor();
    COLORREF saveText = gettextcolor();
    COLORREF saveLine = getlinecolor();
    int saveBk = getbkmode();
    LOGFONT saveFont;
    gettextstyle(&saveFont);

    setorigin(0, 0);

    setfillcolor(UIConfig::COL_PANEL_FILL);
    setlinecolor(UIConfig::COL_PANEL_BORDER);
    fillroundrect(_x, _y, _x + _w, _y + _h, 12, 12);

    int itemY = _y + 25;
    int gap = 55;

    _toggles[0].x = _x + 20;
    _toggles[0].y = itemY;
    _toggles[0].w = 45;
    _toggles[0].h = 25;
    _toggles[0].state = &_musicOn;
    _toggles[0].label = "音乐：";

    itemY += gap;
    _toggles[1].x = _x + 20;
    _toggles[1].y = itemY;
    _toggles[1].w = 45;
    _toggles[1].h = 25;
    _toggles[1].state = &_soundOn;
    _toggles[1].label = "音效：";

    itemY += gap;
    _toggles[2].x = _x + 20;
    _toggles[2].y = itemY;
    _toggles[2].w = 45;
    _toggles[2].h = 25;
    _toggles[2].state = &_autoRotateOn;
    _toggles[2].label = "魔方自转：";

    setbkmode(TRANSPARENT);
    settextstyle(24, 0, _T("微软雅黑"));

    for (int i = 0; i < 3; i++) {
        Toggle& t = _toggles[i];

        settextcolor(UIConfig::COL_PANEL_TEXT);
        outtextxy(t.x, t.y, t.label);

        int swX = t.x + 120;
        int swY = t.y;

        bool isHover = (msg.x >= swX && msg.x <= swX + t.w &&
            msg.y >= swY && msg.y <= swY + t.h);

        if (*(t.state)) {
            setfillcolor(isHover ? UIConfig::COL_BTN_HOVER : UIConfig::COL_BTN_NORMAL);
        }
        else {
            setfillcolor(isHover ? UIConfig::COL_PANEL_INNER : UIConfig::COL_TOGGLE_OFF);
        }

        setlinecolor(UIConfig::COL_PANEL_BORDER);
        fillroundrect(swX, swY, swX + t.w, swY + t.h, 6, 6);

        settextcolor(UIConfig::COL_PANEL_TEXT);
        settextstyle(24, 0, _T("微软雅黑"));
        outtextxy(swX + 14, swY + 0, *(t.state) ? _T("开") : _T("关"));

        if (msg.uMsg == WM_LBUTTONDOWN && isHover) {
            *(t.state) = !*(t.state);
            if (i == 0) {
                setMusicEnabled(_musicOn);
            }
            else if (i == 1) {
                setSoundEnabled(_soundOn);
            }
            if (_soundOn) {
                playClickSound();
            }
        }
    }

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

//绘制公式按钮和坐标轴开关
void FormulaPanel::draw(MOUSEMSG msg) {
    m_lastClicked = "";
    if (!m_visible) return;

    setfillcolor(UIConfig::COL_PANEL_FILL);
    setlinecolor(UIConfig::COL_PANEL_BORDER);
    int panelW = 4 * 60 + 3 * 10 + 20;
    int panelH = 3 * 40 + 2 * 10 + 20;
    int extraH = 50;
    fillroundrect(m_x, m_y, m_x + panelW, m_y + panelH + extraH, 12, 12);

    for (auto& key : m_keys) {
        int bx = m_x + key.x + 10;
        int by = m_y + key.y + 10;
        key.isHover = (msg.x >= bx && msg.x <= bx + key.w && msg.y >= by && msg.y <= by + key.h);
        setfillcolor(key.isHover ? UIConfig::COL_BTN_HOVER : UIConfig::COL_BTN_NORMAL);
        setlinecolor(UIConfig::COL_PANEL_BORDER);
        fillroundrect(bx, by, bx + key.w, by + key.h, 8, 8);
        settextcolor(UIConfig::COL_TEXT);
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
    setfillcolor(UIConfig::COL_PANEL_INNER);
    setlinecolor(UIConfig::COL_PANEL_BORDER);
    fillroundrect(sx, sy, sx + sw, sy + sh, 6, 6);

    int btnW = 100;
    int btnH = 24;
    int bx = sx + sw - btnW - 10;
    int by = sy + (sh - btnH) / 2;
    bool btnHover = (msg.x >= bx && msg.x <= bx + btnW && msg.y >= by && msg.y <= by + btnH);
    setfillcolor(btnHover ? UIConfig::COL_BTN_HOVER : UIConfig::COL_BTN_NORMAL);
    setlinecolor(UIConfig::COL_PANEL_BORDER);
    fillroundrect(bx, by, bx + btnW, by + btnH, 6, 6);
    settextcolor(UIConfig::COL_PANEL_TEXT);
    settextstyle(20, 0, _T("微软雅黑"));
    const char* axisLabel = "坐标轴:";
    int labelH = textheight(axisLabel);
    outtextxy(sx + 8, sy + (sh - labelH) / 2, axisLabel);

    const char* btnText = m_axesOn ? "开" : "关";
    int btnTextW = textwidth(btnText);
    int btnTextH = textheight(btnText);
    outtextxy(bx + (btnW - btnTextW) / 2, by + (btnH - btnTextH) / 2, btnText);
    if (msg.uMsg == WM_LBUTTONDOWN && btnHover) {
        playClickSound();
        m_axesOn = !m_axesOn;
    }
}

TeachColorPanel::TeachColorPanel() : m_x(0), m_y(0), m_visible(false), m_lastClicked(0) {
    int w = 40;
    int h = 40;
    int gap = 10;

    m_keys.push_back({ 'G', cubeColorToRgb('G'), 0 * (w + gap), 0 * (h + gap), w, h, false });
    m_keys.push_back({ 'O', cubeColorToRgb('O'), 1 * (w + gap), 0 * (h + gap), w, h, false });
    m_keys.push_back({ 'Y', cubeColorToRgb('Y'), 2 * (w + gap), 0 * (h + gap), w, h, false });
    m_keys.push_back({ 'W', cubeColorToRgb('W'), 0 * (w + gap), 1 * (h + gap), w, h, false });
    m_keys.push_back({ 'R', cubeColorToRgb('R'), 1 * (w + gap), 1 * (h + gap), w, h, false });
    m_keys.push_back({ 'B', cubeColorToRgb('B'), 2 * (w + gap), 1 * (h + gap), w, h, false });
}

void TeachColorPanel::setPosition(int x, int y) { m_x = x; m_y = y; }
void TeachColorPanel::show() { m_visible = true; }
void TeachColorPanel::hide() { m_visible = false; }
bool TeachColorPanel::isVisible() const { return m_visible; }
char TeachColorPanel::getClickedColor() const { return m_lastClicked; }

//绘制教学模式颜色选择面板
void TeachColorPanel::draw(MOUSEMSG msg) {
    m_lastClicked = 0;
    if (!m_visible) return;

    int w = 40;
    int h = 40;
    int gap = 10;
    int panelW = 3 * w + 2 * gap + 20;
    int panelH = 2 * h + gap + 20;

    setfillcolor(UIConfig::COL_PANEL_FILL);
    setlinecolor(UIConfig::COL_PANEL_BORDER);
    fillroundrect(m_x, m_y, m_x + panelW, m_y + panelH, 8, 8);

    for (auto& k : m_keys) {
        int bx = m_x + k.x + 10;
        int by = m_y + k.y + 10;

        k.hover = (msg.x >= bx && msg.x <= bx + k.w && msg.y >= by && msg.y <= by + k.h);
        setfillcolor(k.hover ? UIConfig::COL_BTN_HOVER : UIConfig::COL_BTN_NORMAL);
        setlinecolor(UIConfig::COL_PANEL_BORDER);
        fillroundrect(bx, by, bx + k.w, by + k.h, 6, 6);

        setfillcolor(k.col);
        fillroundrect(bx + 4, by + 4, bx + k.w - 4, by + k.h - 4, 4, 4);

        if (msg.uMsg == WM_LBUTTONDOWN && k.hover) {
            m_lastClicked = k.c;
        }
    }
}
