#pragma once

// 初始化背景音乐播放（程序启动时调用）
void initMusic();

// 停止背景音乐播放（可选，程序退出时调用）
void stopMusic();

// 播放点击音效
void playClickSound();

// 播放魔方操作音效（公式控制专用）
void playCubeSound();

// 音频开关状态
bool isMusicEnabled();
bool isSoundEnabled();
void setMusicEnabled(bool enabled);
void setSoundEnabled(bool enabled);
