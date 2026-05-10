//67Cuber

#include "game.h"

int main(int argc, char* argv[]) {
	
	//初始化运行环境
	initGameApp(argv[0]);

	//主循环
	while (true) {
		runGameApp();
	}

	//收尾入口
	closeGameApp();
	return 0;
}