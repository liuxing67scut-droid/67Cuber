#pragma once

#include "common.h"

#include <string>

bool loadScaledImage(IMAGE& image, const std::string& path, int width, int height);
bool loadImageFitWidth(IMAGE& image, const std::string& path, int targetWidth);
void drawImageAlpha(int x, int y, IMAGE* image);
