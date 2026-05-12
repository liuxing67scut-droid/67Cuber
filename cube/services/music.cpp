#include "music.h"
#include "app_paths.h"

#include <windows.h>
#include <mmsystem.h>
#include <string>

#pragma comment(lib, "winmm.lib")

static bool g_musicEnabled = true;
static bool g_soundEnabled = true;
static bool g_musicOpened = false;
static bool g_musicPlaying = false;

//使用 MCI 循环播放背景音乐
static void playBackgroundMusic() {
    std::string musicPath = getMusicPath("minecraft.mp3");
    std::string openCmd = "open \"" + musicPath + "\" type mpegvideo alias bgm";

    if (!g_musicOpened) {
        MCIERROR openResult = mciSendStringA(openCmd.c_str(), NULL, 0, NULL);
        g_musicOpened = (openResult == 0);
    }

    if (g_musicOpened) {
        MCIERROR playResult = mciSendStringA("play bgm repeat", NULL, 0, NULL);
        g_musicPlaying = (playResult == 0);
    }
}

static void pauseBackgroundMusic() {
    if (!g_musicOpened) {
        g_musicPlaying = false;
        return;
    }

    MCIERROR pauseResult = mciSendStringA("pause bgm", NULL, 0, NULL);
    if (pauseResult == 0) {
        g_musicPlaying = false;
    }
}

static void resumeBackgroundMusic() {
    if (!g_musicOpened) {
        playBackgroundMusic();
        return;
    }

    MCIERROR resumeResult = mciSendStringA("resume bgm", NULL, 0, NULL);
    if (resumeResult != 0) {
        resumeResult = mciSendStringA("play bgm repeat", NULL, 0, NULL);
    }
    g_musicPlaying = (resumeResult == 0);
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
            resumeBackgroundMusic();
        }
    }
    else if (g_musicPlaying) {
        pauseBackgroundMusic();
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
    if (g_musicOpened) {
        mciSendStringA("stop bgm", NULL, 0, NULL);
        mciSendStringA("close bgm", NULL, 0, NULL);
    }
    g_musicOpened = false;
    g_musicPlaying = false;
}

//播放按钮点击音效
void playClickSound() {
    std::string clickPath = getMusicPath("click.mp3");
    if (!g_soundEnabled) return;

    std::string openCmd = "open \"" + clickPath + "\" type mpegvideo alias click";
    mciSendStringA("close click", NULL, 0, NULL);
    mciSendStringA(openCmd.c_str(), NULL, 0, NULL);
    mciSendStringA("play click from 0", NULL, 0, NULL);
}

//播放魔方转动音效
void playCubeSound() {
    std::string cubePath = getMusicPath("cube.mp3");
    if (!g_soundEnabled) return;

    std::string openCmd = "open \"" + cubePath + "\" type mpegvideo alias cube";
    mciSendStringA("close cube", NULL, 0, NULL);
    mciSendStringA(openCmd.c_str(), NULL, 0, NULL);
    mciSendStringA("play cube from 0", NULL, 0, NULL);
}
