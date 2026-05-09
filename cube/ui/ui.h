#pragma once
#include "common.h"

// 界面状态枚举
enum GameState {
    STATE_MENU,       // 开始菜单
    STATE_SELECT,     // 模式选择
    STATE_GAME_3x3,   // 3阶魔方运行中
    STATE_GAME_2x2,   // 2阶魔方 (预留)
    STATE_EXIT        // 退出
};

// 颜色宏
namespace UIConfig {
    const COLORREF COL_BG = RGB(35, 35, 35);
    const COLORREF COL_BTN_NORMAL = RGB(60, 60, 60);
    const COLORREF COL_BTN_HOVER = RGB(100, 100, 100);
    const COLORREF COL_TEXT = RGB(230, 230, 230);
    const COLORREF COL_TITLE = RGB(255, 215, 0);
}

// 按钮类
class Button {
public:
    Button();
    Button(int x, int y, int w, int h, std::string text);

    // 设置位置和文字
    void setRect(int x, int y, int w, int h);
    void setText(const std::string& text);

    // 绘制按钮，传入鼠标消息，返回是否被点击
    bool draw(MOUSEMSG msg);

    // 手动检查是否包含点
    bool contains(int x, int y) const;

private:
    int m_x, m_y, m_w, m_h;
    std::string m_text;
    bool m_isHover;
};

// 全局UI绘制函数声明
void drawTitle(int y, const std::string& text, int size = 48);
// 新增：弹窗提示
void drawPopup(const std::string& text, int durationMs = 1500);
// 弹窗显示（每帧调用）
void showPopupIfNeeded();



// ==========================================
// 新增：排行榜面板类
// ==========================================
class RankPanel {
public:
    RankPanel();

    // 设置面板居中显示
    void show(int screenWidth, int screenHeight);

    // 隐藏面板
    void hide();

    // 是否可见
    bool isVisible() const;

    // 绘制面板 (包含点击检测逻辑)
    // 返回: true 如果点击了关闭按钮
    bool draw(MOUSEMSG msg);

    //获取
    int getX() const { return _x; }
    int getY() const { return _y; }
    int getW() const { return _w; }
    int getH() const { return _h; }

private:
    bool _visible;
    int _x, _y, _w, _h;
};


class LoginPanel {
public:
    LoginPanel();

    void show(int screenWidth, int screenHeight);
    void hide();
    bool isVisible() const;

    // 绘制面板 + 处理输入
    // 返回值：true 表示点击了“确认”，false 表示点击了“取消”或无操作
    bool draw(MOUSEMSG msg, string& outUsername);

    // ========== 【新增】处理字符输入（从图形窗口消息获取） ==========
    void handleCharInput(char ch);

private:
    bool _visible;
    int _x, _y, _w, _h;

    // 输入框状态
    string _inputText;      // 输入的用户名
    bool _isInputActive;    // 输入框是否被激活（正在输入）

    // 按钮
    Button _btnConfirm;     // 确认
    Button _btnCancel;      // 取消
};

// ==========================================
// 设置面板类
// ==========================================
class SettingsPanel {
public:
    SettingsPanel();

    // 在指定按钮下方显示面板
    //void show(int btnX, int btnY, int btnW, int btnH);
    void show(int panelX, int panelY);
    void hide();
    bool isVisible() const;

    // 绘制面板 + 处理点击
    // 返回值：true 表示点击了面板外部，需要关闭
    bool draw(MOUSEMSG msg);

    // 获取设置状态
    bool isMusicOn() const;
    bool isSoundOn() const;
    bool isAutoRotateOn() const;

private:
    bool _visible;
    int _x, _y, _w, _h;

    // 设置状态
    bool _musicOn;
    bool _soundOn;
    bool _autoRotateOn;

    // 开关按钮（简单的矩形点击区域）
    struct Toggle {
        int x, y, w, h;
        bool* state;
        const char* label;
    };
    Toggle _toggles[3];
};





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
