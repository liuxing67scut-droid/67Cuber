//67Cuber: Rubik's Cube assistant

#include "game.h"

int main(int argc, char* argv[]) {
	initGameApp(argv[0]);

	while (true) {
		runGameApp();
	}

	closeGameApp();
	return 0;
}
