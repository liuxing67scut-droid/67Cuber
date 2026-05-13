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
    //全局深色底色，用于没有图片背景时的基础清屏色
    const COLORREF COL_BG = RGB(37, 43, 38);
    //游玩页默认纯色背景，偏暗偏绿，降低对魔方颜色的干扰
    const COLORREF COL_PLAY_BG = RGB(45, 50, 41);

    //普通按钮底色，主要用于游戏页小按钮和通用操作按钮
    const COLORREF COL_BTN_NORMAL = RGB(214, 153, 88);
    //普通按钮悬停底色，比默认状态更亮
    const COLORREF COL_BTN_HOVER = RGB(236, 178, 107);
    //普通按钮边框色，深木色但不压成纯黑
    const COLORREF COL_BTN_BORDER = RGB(105, 61, 32);
    //普通按钮文字色，深棕色，适合浅木色按钮
    const COLORREF COL_TEXT = RGB(58, 32, 20);
    //普通按钮边框宽度，小按钮略加粗但不抢画面
    const int NORMAL_BTN_BORDER_WIDTH = 2;
    //游戏页计时器、右上角用户 ID 等 HUD 文字色
    const COLORREF COL_HUD_TEXT = RGB(245, 231, 204);
    //当前模式按钮的外层高亮边框色，用于区分自动、练习、教学模式
    const COLORREF COL_MODE_ACTIVE_BORDER = RGB(255, 224, 139);
    //当前模式按钮外层高亮边框宽度
    const int MODE_ACTIVE_BORDER_WIDTH = 3;
    //旧文本标题的备用颜色，图片标题加载失败时仍可使用
    const COLORREF COL_TITLE = RGB(255, 215, 0);

    //大按钮底色，用于封面页和选择页的主要入口按钮
    const COLORREF COL_LARGE_BTN_NORMAL = RGB(224, 154, 84);
    //大按钮悬停底色，保持木色但反馈更明显
    const COLORREF COL_LARGE_BTN_HOVER = RGB(241, 177, 102);
    //大按钮粗边框色，强调像素木牌感
    const COLORREF COL_LARGE_BTN_BORDER = RGB(105, 61, 32);
    //大按钮文字色，配合加粗字体提高可读性
    const COLORREF COL_LARGE_BTN_TEXT = RGB(55, 29, 16);
    //大按钮边框宽度，只影响封面页和选择页大按钮
    const int LARGE_BTN_BORDER_WIDTH = 3;
    //大按钮文字高度，只影响封面页和选择页大按钮
    const int LARGE_BTN_TEXT_SIZE = 32;

    //通用面板底色，用于排行榜、设置、登录、公式等弹出面板
    const COLORREF COL_PANEL_FILL = RGB(228, 171, 105);
    //面板内部浅色区域，用于输入框外层、开关行等次级块
    const COLORREF COL_PANEL_INNER = RGB(244, 198, 135);
    //通用面板边框色，统一木质边缘
    const COLORREF COL_PANEL_BORDER = RGB(75, 43, 25);
    //面板分割线颜色，用于表头下划线、滚动条轨道等
    const COLORREF COL_PANEL_LINE = RGB(139, 84, 45);
    //面板弱化文字颜色，用于表头、页脚、提示说明
    const COLORREF COL_PANEL_MUTED = RGB(104, 70, 45);
    //面板主要文字颜色
    const COLORREF COL_PANEL_TEXT = RGB(58, 32, 20);
    //关闭状态的开关底色，比开启状态更暗
    const COLORREF COL_TOGGLE_OFF = RGB(198, 135, 75);

    //排行榜第一名文字色，保留金色但压暗以适配木色面板
    const COLORREF COL_RANK_GOLD = RGB(166, 109, 10);
    //排行榜第二名文字色
    const COLORREF COL_RANK_SILVER = RGB(105, 105, 105);
    //排行榜第三名文字色
    const COLORREF COL_RANK_BRONZE = RGB(150, 82, 34);

    //短提示弹窗底色
    const COLORREF COL_POPUP_FILL = RGB(236, 178, 107);
    //短提示弹窗边框色
    const COLORREF COL_POPUP_BORDER = RGB(75, 43, 25);
    //短提示弹窗文字色
    const COLORREF COL_POPUP_TEXT = RGB(58, 32, 20);

    //登录输入框底色
    const COLORREF COL_INPUT_FILL = RGB(248, 210, 154);
    //登录输入框普通边框色
    const COLORREF COL_INPUT_BORDER = RGB(124, 76, 42);
    //登录输入框激活边框色
    const COLORREF COL_INPUT_ACTIVE = RGB(164, 96, 46);

    //关闭按钮 X 的文字和边框色
    const COLORREF COL_CLOSE_TEXT = RGB(132, 39, 28);
    //关闭按钮普通底色
    const COLORREF COL_CLOSE_FILL = RGB(244, 198, 135);
    //关闭按钮悬停底色
    const COLORREF COL_CLOSE_HOVER = RGB(255, 216, 158);
}

enum ButtonVisualStyle {
    BUTTON_STYLE_NORMAL,
    BUTTON_STYLE_LARGE
};

//通用按钮
class Button {
public:
    Button();
    Button(int x, int y, int w, int h, std::string text);

    void setRect(int x, int y, int w, int h);
    void setText(const std::string& text);
    void setVisualStyle(ButtonVisualStyle style);
    bool draw(MOUSEMSG msg);
    bool contains(int x, int y) const;

private:
    int m_x, m_y, m_w, m_h;
    std::string m_text;
    bool m_isHover;
    ButtonVisualStyle m_style;
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
    int _scrollOffset;
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

//模式玩法提示框
class ModeHelpPanel {
public:
    ModeHelpPanel();

    void showForMode(int mode);
    void hide();
    bool isVisible() const;
    bool draw(MOUSEMSG msg);

private:
    bool m_visible;
    int m_mode;
    bool m_suppressed[3];
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
