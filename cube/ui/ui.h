#pragma once
#include "common.h"

//界面状态
enum GameState {
    STATE_MENU,
    STATE_SELECT,
    STATE_GAME_3x3,
    STATE_GAME_2x2,
    STATE_EXIT
};

namespace UIConfig {
    const COLORREF COL_BG = RGB(35, 35, 35);
    const COLORREF COL_BTN_NORMAL = RGB(60, 60, 60);
    const COLORREF COL_BTN_HOVER = RGB(100, 100, 100);
    const COLORREF COL_TEXT = RGB(230, 230, 230);
    const COLORREF COL_TITLE = RGB(255, 215, 0);
}

//通用按钮
class Button {
public:
    Button();
    Button(int x, int y, int w, int h, std::string text);

    void setRect(int x, int y, int w, int h);
    void setText(const std::string& text);
    bool draw(MOUSEMSG msg);
    bool contains(int x, int y) const;

private:
    int m_x, m_y, m_w, m_h;
    std::string m_text;
    bool m_isHover;
};

//基础 UI 绘制
void drawTitle(int y, const std::string& text, int size = 48);
void drawPopup(const std::string& text, int durationMs = 1500);
void showPopupIfNeeded();

//排行榜面板
class RankPanel {
public:
    RankPanel();

    void show(int screenWidth, int screenHeight);
    void hide();
    bool isVisible() const;
    bool draw(MOUSEMSG msg);

    int getX() const { return _x; }
    int getY() const { return _y; }
    int getW() const { return _w; }
    int getH() const { return _h; }

private:
    bool _visible;
    int _x, _y, _w, _h;
};

//登录面板
class LoginPanel {
public:
    LoginPanel();

    void show(int screenWidth, int screenHeight);
    void hide();
    bool isVisible() const;
    bool draw(MOUSEMSG msg, string& outUsername);
    void handleCharInput(char ch);

private:
    bool _visible;
    int _x, _y, _w, _h;

    string _inputText;
    bool _isInputActive;

    Button _btnConfirm;
    Button _btnCancel;
};

//设置面板
class SettingsPanel {
public:
    SettingsPanel();

    void show(int panelX, int panelY);
    void hide();
    bool isVisible() const;
    bool draw(MOUSEMSG msg);

    bool isMusicOn() const;
    bool isSoundOn() const;
    bool isAutoRotateOn() const;

private:
    bool _visible;
    int _x, _y, _w, _h;

    bool _musicOn;
    bool _soundOn;
    bool _autoRotateOn;

    struct Toggle {
        int x, y, w, h;
        bool* state;
        const char* label;
    };
    Toggle _toggles[3];
};

//公式面板
struct FormulaKey {
    std::string label;
    int x, y, w, h;
    bool isHover = false;
};

class FormulaPanel {
public:
    FormulaPanel();
    void setPosition(int x, int y);
    void draw(MOUSEMSG msg);
    bool isVisible() const;
    void show();
    void hide();
    std::string getClickedFormula() const;
    void resetHover();
    bool isAxesOn() const;

private:
    int m_x, m_y;
    bool m_visible;
    std::vector<FormulaKey> m_keys;
    std::string m_lastClicked;
    bool m_axesOn;
};

//教学模式颜色面板
class TeachColorPanel {
public:
    TeachColorPanel();
    void setPosition(int x, int y);
    void show();
    void hide();
    bool isVisible() const;
    void draw(MOUSEMSG msg);
    char getClickedColor() const;

private:
    int m_x, m_y;
    bool m_visible;
    char m_lastClicked;

    struct CKey {
        char c;
        COLORREF col;
        int x, y, w, h;
        bool hover;
    };

    std::vector<CKey> m_keys;
};
