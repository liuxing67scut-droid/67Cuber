#pragma once

//初始化背景音乐播放
void initMusic();

//停止背景音乐播放
void stopMusic();

//播放点击音效
void playClickSound();

//播放魔方操作音效
void playCubeSound();

//音频开关状态
bool isMusicEnabled();
bool isSoundEnabled();
void setMusicEnabled(bool enabled);
void setSoundEnabled(bool enabled);
