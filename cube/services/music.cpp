#include "music.h"
#include "app_paths.h"

#include <windows.h>
#include <mmsystem.h>
#include <string>

#pragma comment(lib, "winmm.lib")

static bool g_musicEnabled = true;
static bool g_soundEnabled = true;
static bool g_musicPlaying = false;

//使用 MCI 循环播放背景音乐
static void playBackgroundMusic() {
    std::string openCmd = "open \"" + getMusicPath("minecraft.mp3") + "\" type mpegvideo alias bgm";
    mciSendStringA("close bgm", NULL, 0, NULL);
    mciSendStringA(openCmd.c_str(), NULL, 0, NULL);
    mciSendStringA("play bgm repeat", NULL, 0, NULL);
    g_musicPlaying = true;
}

bool isMusicEnabled() {
    return g_musicEnabled;
}

bool isSoundEnabled() {
    return g_soundEnabled;
}

//设置背景音乐开关，并立即同步播放状态
void setMusicEnabled(bool enabled) {
    g_musicEnabled = enabled;
    if (g_musicEnabled) {
        if (!g_musicPlaying) {
            playBackgroundMusic();
        }
    }
    else if (g_musicPlaying) {
        stopMusic();
    }
}

void setSoundEnabled(bool enabled) {
    g_soundEnabled = enabled;
}

//启动时根据开关状态播放背景音乐
void initMusic() {
    if (g_musicEnabled) {
        playBackgroundMusic();
    }
}

//停止并关闭背景音乐别名
void stopMusic() {
    mciSendStringA("stop bgm", NULL, 0, NULL);
    mciSendStringA("close bgm", NULL, 0, NULL);
    g_musicPlaying = false;
}

//播放按钮点击音效
void playClickSound() {
    if (!g_soundEnabled) return;

    mciSendStringA("close click", NULL, 0, NULL);
    mciSendStringA(("open \"" + getMusicPath("click.mp3") + "\" type mpegvideo alias click").c_str(), NULL, 0, NULL);
    mciSendStringA("play click from 0", NULL, 0, NULL);
}

//播放魔方转动音效
void playCubeSound() {
    if (!g_soundEnabled) return;

    mciSendStringA("close cube", NULL, 0, NULL);
    std::string openCmd = "open \"" + getMusicPath("cube.mp3") + "\" type mpegvideo alias cube";
    mciSendStringA(openCmd.c_str(), NULL, 0, NULL);
    mciSendStringA("play cube from 0", NULL, 0, NULL);
}
