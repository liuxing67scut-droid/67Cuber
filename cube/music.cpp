#include "music.h"

#include <windows.h>
#include <mmsystem.h>
#include <string>

#pragma comment(lib, "winmm.lib")

static const char* MUSIC_PATH = "C:/Users/17646/Desktop/cube_v1/cube/music/minecraft.mp3";
static const char* CLICK_SOUND_PATH = "C:/Users/17646/Desktop/cube_v1/cube/music/click.mp3";
static const char* CUBE_SOUND_PATH = "C:/Users/17646/Desktop/cube_v1/cube/music/cube.mp3";

static bool g_musicEnabled = true;
static bool g_soundEnabled = true;
static bool g_musicPlaying = false;

static void playBackgroundMusic() {
    std::string openCmd = "open \"" + std::string(MUSIC_PATH) + "\" type mpegvideo alias bgm";
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

void initMusic() {
    if (g_musicEnabled) {
        playBackgroundMusic();
    }
}

void stopMusic() {
    mciSendStringA("stop bgm", NULL, 0, NULL);
    mciSendStringA("close bgm", NULL, 0, NULL);
    g_musicPlaying = false;
}

void playClickSound() {
    if (!g_soundEnabled) return;

    mciSendStringA("close click", NULL, 0, NULL);
    mciSendStringA(("open \"" + std::string(CLICK_SOUND_PATH) + "\" type mpegvideo alias click").c_str(), NULL, 0, NULL);
    mciSendStringA("play click from 0", NULL, 0, NULL);
}

void playCubeSound() {
    if (!g_soundEnabled) return;

    mciSendStringA("close cube", NULL, 0, NULL);
    std::string openCmd = "open \"" + std::string(CUBE_SOUND_PATH) + "\" type mpegvideo alias cube";
    mciSendStringA(openCmd.c_str(), NULL, 0, NULL);
    mciSendStringA("play cube from 0", NULL, 0, NULL);
}
