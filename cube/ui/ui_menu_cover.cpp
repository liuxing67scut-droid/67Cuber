#include "ui_menu_cover.h"

#include "app_paths.h"
#include "ui.h"
#include "ui_image.h"

#include <ctime>
#include <cmath>

namespace {
int getMenuTitleY() {
	double seconds = clock() / (double)CLOCKS_PER_SEC;
	double offset = sin(seconds * 2.0 * PI / 3.0) * 6.0;
	return 80 + (int)offset;
}

bool drawMenuBackgroundImage() {
	static IMAGE bgImage;
	static bool triedLoad = false;
	static bool loaded = false;

	if (!triedLoad) {
		triedLoad = true;
		loaded = loadScaledImage(bgImage, getImagePath("menu_bg.png"), Width, Height);
	}

	if (!loaded) {
		return false;
	}

	putimage(0, 0, &bgImage);
	return true;
}

bool drawMenuTitleImage() {
	static IMAGE titleImage;
	static bool triedLoad = false;
	static bool loaded = false;

	if (!triedLoad) {
		triedLoad = true;
		loaded = loadImageFitWidth(titleImage, getImagePath("menu_title.png"), 680);
	}

	if (!loaded) {
		return false;
	}

	int x = (Width - titleImage.getwidth()) / 2;
	int y = getMenuTitleY();
	drawImageAlpha(x, y, &titleImage);
	return true;
}
}

void drawMenuCover() {
	if (!drawMenuBackgroundImage()) {
		setbkcolor(UIConfig::COL_BG);
	}

	if (!drawMenuTitleImage()) {
		drawTitle(150, "67 Cuber");
	}
}
