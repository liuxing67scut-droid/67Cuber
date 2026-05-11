#include "ui_play_cover.h"

#include "app_paths.h"
#include "ui.h"
#include "ui_image.h"

namespace {
const bool USE_PLAY_BACKGROUND_IMAGE = true; 

bool drawPlayBackgroundImage() {
	static IMAGE bgImage;
	static bool triedLoad = false;
	static bool loaded = false;

	if (!triedLoad) {
		triedLoad = true;
		loaded = loadScaledImage(bgImage, getImagePath("play_bg.png"), Width, Height);
	}

	if (!loaded) {
		return false;
	}

	putimage(0, 0, &bgImage);
	return true;
}

void drawSolidPlayBackground() {
	setfillcolor(UIConfig::COL_PLAY_BG);
	setlinecolor(UIConfig::COL_PLAY_BG);
	fillrectangle(0, 0, Width, Height);
}
}

void drawPlayBackground() {
	if (USE_PLAY_BACKGROUND_IMAGE && drawPlayBackgroundImage()) {
		return;
	}

	drawSolidPlayBackground();
}
