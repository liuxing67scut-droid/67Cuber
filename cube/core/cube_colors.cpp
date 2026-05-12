#include "cube_colors.h"

#include "cubesolver.h"

COLORREF cubeColorToRgb(char color) {
	switch (color) {
	case 'Y': return RGB(220, 180, 100);
	case 'W': return RGB(240, 230, 210);
	case 'R': return RGB(180, 80, 60);
	case 'O': return RGB(200, 120, 60);
	case 'B': return RGB(100, 140, 180);
	case 'G': return RGB(120, 160, 100);
	default:  return RGB(70, 70, 70);
	}
}

char cubeFaceColorChar(int face) {
	if (face < 0 || face >= 6) {
		return ' ';
	}
	return Color[face];
}

COLORREF cubeFaceColorToRgb(int face) {
	return cubeColorToRgb(cubeFaceColorChar(face));
}
