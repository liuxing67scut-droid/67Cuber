#include "ui_image.h"

#pragma comment(lib, "Msimg32.lib")

bool loadScaledImage(IMAGE& image, const std::string& path, int width, int height) {
	loadimage(&image, path.c_str(), width, height, true);
	return image.getwidth() > 0 && image.getheight() > 0;
}

bool loadImageFitWidth(IMAGE& image, const std::string& path, int targetWidth) {
	IMAGE rawImage;
	loadimage(&rawImage, path.c_str());
	if (rawImage.getwidth() <= 0 || rawImage.getheight() <= 0) {
		return false;
	}

	int targetHeight = targetWidth * rawImage.getheight() / rawImage.getwidth();
	return loadScaledImage(image, path, targetWidth, targetHeight);
}

void drawImageAlpha(int x, int y, IMAGE* image) {
	if (!image || image->getwidth() <= 0 || image->getheight() <= 0) {
		return;
	}

	HDC dstHdc = GetImageHDC(NULL);
	HDC srcHdc = GetImageHDC(image);

	BLENDFUNCTION blend = {};
	blend.BlendOp = AC_SRC_OVER;
	blend.BlendFlags = 0;
	blend.SourceConstantAlpha = 255;
	blend.AlphaFormat = AC_SRC_ALPHA;

	AlphaBlend(
		dstHdc,
		x,
		y,
		image->getwidth(),
		image->getheight(),
		srcHdc,
		0,
		0,
		image->getwidth(),
		image->getheight(),
		blend
	);
}
