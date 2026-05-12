#include "ui_select_cover.h"

#include "app_paths.h"
#include "ui.h"
#include "ui_image.h"

#include <ctime>
#include <cmath>

namespace {
int getSelectTitleY() {
	double seconds = clock() / (double)CLOCKS_PER_SEC;
	double offset = sin(seconds * 2.0 * PI / 3.0) * 5.0;
	return 115 + (int)offset;
}

bool drawSelectBackgroundImage() {
	static IMAGE bgImage;
	static bool triedLoad = false;
	static bool loaded = false;

	if (!triedLoad) {
		triedLoad = true;
		loaded = loadScaledImage(bgImage, getImagePath("select_bg.png"), Width, Height);
		if (!loaded) {
			loaded = loadScaledImage(bgImage, getImagePath("menu_bg.png"), Width, Height);
		}
	}

	if (!loaded) {
		return false;
	}

	putimage(0, 0, &bgImage);
	return true;
}

bool drawSelectTitleImage() {
	static IMAGE titleImage;
	static bool triedLoad = false;
	static bool loaded = false;

	if (!triedLoad) {
		triedLoad = true;
		loaded = loadImageFitWidth(titleImage, getImagePath("select_title.png"), 420);
	}

	if (!loaded) {
		return false;
	}

	int x = (Width - titleImage.getwidth()) / 2;
	int y = getSelectTitleY();
	drawImageAlpha(x, y, &titleImage);
	return true;
}
}

void drawSelectCover() {
	if (!drawSelectBackgroundImage()) {
		setbkcolor(UIConfig::COL_BG);
	}

	if (!drawSelectTitleImage()) {
		drawTitle(150, "选择魔方");
	}
}
