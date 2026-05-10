#include "cubesolver.h"
#include <map>
#include <sstream>

// 习惯：
// R: 右面 (Right) - 红色
// L: 左面 (Left) - 橙色
// U: 上面 (Up) - 黄色
// D: 下面 (Down) - 白色
// F: 前面 (Front) - 绿色
// B: 后面 (Back) - 蓝色
// 0:Front 1:Left 2:Up 3:Down 4:Right 5:Back
const int F = 0, L = 1, U = 2, D = 3, R = 4, B = 5;
const char Color[6] = { 'G','O','Y','W','R','B' };
string Command;
int step = 0;
bool isSrand = 0;





void init(char cube[][3][3])
{
	Command = "";
	for (int i = 0; i < 6; i++)//initialize
		for (int j = 0; j < 3; j++)
			for (int k = 0; k < 3; k++)
				cube[i][j][k] = Color[i];
}
void random(char cube[][3][3], int N)
{
	if (!isSrand)
		srand(static_cast<unsigned int>(time(nullptr))), isSrand = 1;
	for (int i = 0; i < N; i++)
	{
		int rnd = rand() % 8;
		int rmod = (rand() % 2) * 2 - 1;
		switch (rnd)
		{
		case 0:
			front(cube, rmod);
			break;
		case 1:
			back(cube, rmod);
			break;
		case 2:
			up(cube, rmod);
			break;
		case 3:
			right(cube, rmod);
			break;
		case 4:
			left(cube, rmod);
			break;
		case 5:
			down(cube, rmod);
			break;
		case 6:
			turn(cube);
			break;
		case 7:
			Fturn(cube);
			break;
		}
		printf("\rRanding...\r");
	}

}
// 练习模式专用随机：不包含整块转动（turn/Fturn），避免改变视角映射
void random_practice(char cube[][3][3], int N)
{
	if (!isSrand)
		srand(static_cast<unsigned int>(time(nullptr))), isSrand = 1;
	for (int i = 0; i < N; i++)
	{
		int rnd = rand() % 6; // only 0..5 -> face rotations
		int rmod = (rand() % 2) * 2 - 1;
		switch (rnd)
		{
		case 0:
			front(cube, rmod);
			break;
		case 1:
			back(cube, rmod);
			break;
		case 2:
			up(cube, rmod);
			break;
		case 3:
			right(cube, rmod);
			break;
		case 4:
			left(cube, rmod);
			break;
		case 5:
			down(cube, rmod);
			break;
		}
		printf("\rRanding...\r");
	}
}
void setColor(unsigned short ForeColor, unsigned short BackGroundColor)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);//获取当前窗口句柄
	SetConsoleTextAttribute(handle, ForeColor + BackGroundColor * 0x10);//设置颜色
}
void rotate(char cube[][3][3], int mode, int direct)
{
	char tCube[3][3];//front
	memcpy(tCube, cube[direct], sizeof(tCube));
	if (mode != 1 && mode != -1)
		exit(-1);
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			cube[direct][i][j] = mode == 1 ? tCube[2 - j][i] : tCube[j][2 - i];
}
void front(char cube[][3][3], int mode)
{
	//mode == 1 ? printf("F") : printf("F\'"); step++;
	mode == 1 ? Command += "F " : Command += "F' "; step++;
	rotate(cube, mode, F);//内层检测mode合法性
	char rSide, dSide;
	if (mode == 1)
		for (int i = 0; i < 3; i++)
		{
			rSide = cube[R][i][0];
			cube[R][i][0] = cube[U][2][i];
			cube[U][2][i] = cube[L][2 - i][2];
			cube[L][2 - i][2] = cube[D][0][2 - i];
			cube[D][0][2 - i] = rSide;
		}
	else
		for (int i = 0; i < 3; i++)
		{
			dSide = cube[D][0][2 - i];
			cube[D][0][2 - i] = cube[L][2 - i][2];
			cube[L][2 - i][2] = cube[U][2][i];
			cube[U][2][i] = cube[R][i][0];
			cube[R][i][0] = dSide;
		}
}
void left(char cube[][3][3], int mode)
{
	//mode == 1 ? printf("L") : printf("L\'"); step++;
	mode == 1 ? Command += "L " : Command += "L' "; step++;
	rotate(cube, mode, L);//内层检测mode合法性
	char fSide, dSide;
	if (mode == 1)
		for (int i = 0; i < 3; i++)
		{
			fSide = cube[F][i][0];
			cube[F][i][0] = cube[U][i][0];
			cube[U][i][0] = cube[B][2 - i][2];
			cube[B][2 - i][2] = cube[D][i][0];
			cube[D][i][0] = fSide;
		}
	else
		for (int i = 0; i < 3; i++)
		{
			dSide = cube[D][i][0];
			cube[D][i][0] = cube[B][2 - i][2];
			cube[B][2 - i][2] = cube[U][i][0];
			cube[U][i][0] = cube[F][i][0];
			cube[F][i][0] = dSide;
		}
}
void right(char cube[][3][3], int mode)
{
	//mode == 1 ? printf("R") : printf("R\'"); step++;
	mode == 1 ? Command += "R " : Command += "R' "; step++;
	rotate(cube, mode, R);//内层检测mode合法性
	char bSide, dSide;
	if (mode == 1)
		for (int i = 0; i < 3; i++)
		{
			bSide = cube[B][i][0];
			cube[B][i][0] = cube[U][2 - i][2];
			cube[U][2 - i][2] = cube[F][2 - i][2];
			cube[F][2 - i][2] = cube[D][2 - i][2];
			cube[D][2 - i][2] = bSide;
		}
	else
		for (int i = 0; i < 3; i++)
		{
			dSide = cube[D][2 - i][2];
			cube[D][2 - i][2] = cube[F][2 - i][2];
			cube[F][2 - i][2] = cube[U][2 - i][2];
			cube[U][2 - i][2] = cube[B][i][0];
			cube[B][i][0] = dSide;
		}
}
void back(char cube[][3][3], int mode)
{
	//mode == 1 ? printf("B") : printf("B\'"); step++;
	mode == 1 ? Command += "B " : Command += "B' "; step++;
	rotate(cube, mode, B);//内层检测mode合法性
	char lSide, dSide;
	if (mode == 1)
		for (int i = 0; i < 3; i++)
		{
			lSide = cube[L][i][0];
			cube[L][i][0] = cube[U][0][2 - i];
			cube[U][0][2 - i] = cube[R][2 - i][2];
			cube[R][2 - i][2] = cube[D][2][i];
			cube[D][2][i] = lSide;
		}
	else
		for (int i = 0; i < 3; i++)
		{
			dSide = cube[D][2][i];
			cube[D][2][i] = cube[R][2 - i][2];
			cube[R][2 - i][2] = cube[U][0][2 - i];
			cube[U][0][2 - i] = cube[L][i][0];
			cube[L][i][0] = dSide;
		}
}
void up(char cube[][3][3], int mode)
{
	//mode == 1 ? printf("U") : printf("U\'"); step++;
	mode == 1 ? Command += "U " : Command += "U' "; step++;
	rotate(cube, mode, U);//内层检测mode合法性
	char rSide, fSide;
	if (mode == 1)
		for (int i = 0; i < 3; i++)
		{
			rSide = cube[R][0][2 - i];
			cube[R][0][2 - i] = cube[B][0][2 - i];
			cube[B][0][2 - i] = cube[L][0][2 - i];
			cube[L][0][2 - i] = cube[F][0][2 - i];
			cube[F][0][2 - i] = rSide;
		}
	else
		for (int i = 0; i < 3; i++)
		{
			fSide = cube[F][0][2 - i];
			cube[F][0][2 - i] = cube[L][0][2 - i];
			cube[L][0][2 - i] = cube[B][0][2 - i];
			cube[B][0][2 - i] = cube[R][0][2 - i];
			cube[R][0][2 - i] = fSide;
		}
}
void down(char cube[][3][3], int mode)
{
	//mode == 1 ? printf("D") : printf("D\'"); step++;
	mode == 1 ? Command += "D " : Command += "D' "; step++;
	rotate(cube, mode, D);//内层检测mode合法性
	char rSide, bSide;
	if (mode == 1)
		for (int i = 0; i < 3; i++)
		{
			rSide = cube[R][2][i];
			cube[R][2][i] = cube[F][2][i];
			cube[F][2][i] = cube[L][2][i];
			cube[L][2][i] = cube[B][2][i];
			cube[B][2][i] = rSide;
		}
	else
		for (int i = 0; i < 3; i++)
		{
			bSide = cube[B][2][i];
			cube[B][2][i] = cube[L][2][i];
			cube[L][2][i] = cube[F][2][i];
			cube[F][2][i] = cube[R][2][i];
			cube[R][2][i] = bSide;
		}
}
void move(char cube[][3][3], const char* com)//"FRUR'U'F'"
{
	int m = 0;
	while (com[m] != '\0')
	{
		int mode = 1;
		if (com[m + 1] == '\'')
			mode = -1;
		switch (com[m])
		{
		case 'F':
			front(cube, mode);
			break;
		case 'L':
			left(cube, mode);
			break;
		case 'R':
			right(cube, mode);
			break;
		case 'B':
			back(cube, mode);
			break;
		case 'U':
			up(cube, mode);
			break;
		case 'D':
			down(cube, mode);
			break;
		default:
			exit(-2);
		}
		if (mode == -1)m++;
		m++;
	}
}
void turn(char cube[][3][3])
{
	//printf("→");
	Command += "→ ";
	rotate(cube, 1, F);
	rotate(cube, -1, B);
	char rSide;
	for (int m = 0; m < 3; m++)
		for (int i = 0; i < 3; i++)
		{
			rSide = cube[R][i][m];
			cube[R][i][m] = cube[U][2 - m][i];
			cube[U][2 - m][i] = cube[L][2 - i][2 - m];
			cube[L][2 - i][2 - m] = cube[D][m][2 - i];
			cube[D][m][2 - i] = rSide;
		}
}
void Fturn(char cube[][3][3])
{
	//printf("↓");
	Command += "↓ ";
	rotate(cube, 1, L);
	rotate(cube, -1, R);
	char fSide;
	for (int m = 0; m < 3; m++)
		for (int i = 0; i < 3; i++)
		{
			fSide = cube[F][i][m];
			cube[F][i][m] = cube[U][i][m];
			cube[U][i][m] = cube[B][2 - i][2 - m];
			cube[B][2 - i][2 - m] = cube[D][i][m];
			cube[D][i][m] = fSide;
		}
}
void DownCross(char cube[][3][3])
{
	const char std = cube[F][1][1];//standard color(central color)
	while (cube[F][0][1] != std || cube[F][1][0] != std || cube[F][2][1] != std || cube[F][1][2] != std)//not cross
	{
		for (int i = 0; i < 4; i++)
		{
			if (cube[F][0][1] == std && cube[F][1][0] == std && cube[F][2][1] == std && cube[F][1][2] == std)
				return;
			if (cube[L][0][1] == std || cube[R][0][1] == std || cube[B][0][1] == std)
			{
				while (cube[F][0][1] == std)
					front(cube, 1);
				while (cube[F][0][1] != std)
					up(cube, 1);
			}
			turn(cube);
		}
		for (int i = 0; i < 4; i++)
		{
			if (cube[F][0][1] == std && cube[F][1][0] == std && cube[F][2][1] == std && cube[F][1][2] == std)
				return;
			if (cube[U][0][1] == std || cube[U][2][1] == std)
			{
				while (cube[F][0][1] == std)
					front(cube, 1);
				up(cube, 1);
				if (cube[U][1][0] == std)
				{
					while (cube[F][1][0] == std)
						front(cube, 1);
					left(cube, 1);
				}
				if (cube[U][1][2] == std)
				{
					while (cube[F][1][2] == std)
						front(cube, 1);
					right(cube, -1);
				}
			}
			turn(cube);
		}
	}
}
void DownMid(char cube[][3][3])
{
	const char std = cube[F][1][1];
	while (cube[U][1][1] != cube[U][2][1] || cube[F][0][1] != std)
		front(cube, 1);
	move(cube, "UU");
	while (cube[R][1][0] != cube[R][1][1] || cube[F][1][2] != std)
		front(cube, 1);
	move(cube, "RR");
	while (cube[D][0][1] != cube[D][1][1] || cube[F][2][1] != std)
		front(cube, 1);
	move(cube, "DD");
	while (cube[L][1][2] != cube[L][1][1] || cube[F][1][0] != std)
		front(cube, 1);
	move(cube, "DDRRUU");
}
void DownCorner(char cube[][3][3])
{
	const char std = cube[F][1][1];
	char* Uc = &cube[U][1][1], * Rc = &cube[R][1][1];
	char* Ur = &cube[U][0][2], * Bul = &cube[B][0][0];
	char* Ru = &cube[R][0][2];
	for (int i = 0; i < 4; i++)//整体向右旋转后执行重复操作即可!!!!!!!!!!!!!!!!!!!!!
	{
		if ((cube[F][0][2] != std || cube[U][2][2] != *Uc || cube[R][0][0] != *Rc) && (cube[F][0][2] == std || cube[U][2][2] == std || cube[R][0][0] == std))
		{
			while (*Ur == std || *Ru == std || *Bul == std)
				back(cube, 1);
			move(cube, "RBR'");//根据朝向改公式!!!!!!!!!!!!!
		}
		turn(cube);
	}
	for (int i = 0; i < 4; i++)
	{
		if (cube[F][0][2] == std && cube[U][2][2] == *Uc && cube[R][0][0] == *Rc)
		{
			turn(cube);
			continue;
		}

		while (!((*Ur == *Uc || *Ur == *Rc || *Ur == std) && (*Ru == *Uc || *Ru == *Rc || *Ru == std) && (*Bul == *Uc || *Bul == *Rc || *Bul == std)))
			back(cube, 1);
		if (*Ru == std)
			move(cube, "RBR'");
		else if (*Ur == std)
			move(cube, "BRB'R'");
		else
			move(cube, "RB'R'BBRBR'");
		turn(cube);
	}
}
void MidEdge(char cube[][3][3])//根据朝向改公式!!!!!!!!!!!!!
{
	char Bc = cube[B][1][1];
	for (int i = 0; i < 4; i++)
	{
		if ((cube[U][1][2] == cube[U][1][1] && cube[R][0][1] == cube[R][1][1]) || (cube[U][1][2] == Bc || cube[R][0][1] == Bc))//Correct || Bc
		{
			turn(cube);
			continue;
		}

		while (cube[R][1][2] != Bc && cube[B][1][0] != Bc)
			back(cube, 1);
		move(cube, "R'U'RBRB'R'U");
		turn(cube);
	}
	char* Uc = &cube[U][1][1], * Rc = &cube[R][1][1], * Dc = &cube[D][1][1], * Lc = &cube[L][1][1];
	for (int i = 0; i < 4; i++)
	{
		if ((cube[U][1][2] == *Uc && cube[R][0][1] == *Rc) && (cube[R][2][1] == *Rc && cube[D][1][2] == *Dc) && (cube[D][1][0] == *Dc && cube[L][2][1] == *Lc) && (cube[L][0][1] == *Lc && cube[U][1][0] == *Uc))//Correct
			break;
		while (cube[U][0][1] == Bc || cube[B][0][1] == Bc)
			back(cube, 1);
		while (cube[U][0][1] != *Uc)
			turn(cube), back(cube, 1);//bug???
		if (cube[B][0][1] == *Lc)
			turn(cube), move(cube, "R'U'RBRB'R'U");
		else
			move(cube, "BRB'R'B'U'BU");
	}
}
void TopCross(char cube[][3][3])
{
	Fturn(cube);
	Fturn(cube);
	const char std = cube[F][1][1];
	char* Fu = &cube[F][0][1], * Fl = &cube[F][1][0], * Fr = &cube[F][1][2], * Fd = &cube[F][2][1];
	while (!(*Fu == std && *Fl == std && *Fr == std && *Fd == std))
	{
		if (*Fu != std && *Fl != std && *Fr != std && *Fd != std)
			move(cube, "DRFR'F'D'");
		while (*Fl != std)
			front(cube, 1);
		if (*Fr != std)
		{
			if (*Fd == std)
				front(cube, 1);
			move(cube, "DRFR'F'D'");
		}
		else
			move(cube, "DRFR'F'D'");
	}
}
void TopCorner(char cube[][3][3])
{
	const char std = cube[F][1][1];
	char* ul = &cube[F][0][0], * ur = &cube[F][0][2], * dl = &cube[F][2][0], * dr = &cube[F][2][2];
	int cnt = 0;
	*ul == std ? cnt++ : 0; *ur == std ? cnt++ : 0; *dl == std ? cnt++ : 0; *dr == std ? cnt++ : 0;
	if (cnt == 4)return;
	while (true)
	{
		if (cnt == 0)
		{
			while (cube[U][2][0] != std || cube[U][2][2] != std)
				front(cube, 1);
		}
		else if (cnt == 1)
		{
			while (cube[F][0][2] != std)
				front(cube, 1);
		}
		else if (cnt == 2)
		{
			if (*ul == *ur || *ur == *dr || *dr == *dl || *dl == *ul)
				while (*ul != *dl)
					front(cube, 1);
			else
				while (cube[U][2][2] != std && cube[R][0][2] != std)
					front(cube, 1);
		}
		move(cube, "RF'F'R'F'RF'R'");
		cnt = 0;
		*ul == std ? cnt++ : 0; *ur == std ? cnt++ : 0; *dl == std ? cnt++ : 0; *dr == std ? cnt++ : 0;
		if ((cnt == 1 && cube[D][0][0] == std) || cnt == 4)
			break;
	}
	while (*ur != std)
		front(cube, 1);
	while (*ul != std || *ur != std || *dl != std || *dr != std)//??
		move(cube, "RF'F'R'F'RF'R'");
}
void TopLayerCorner(char cube[][3][3])
{
	char* Uc = &cube[U][1][1], * Rc = &cube[R][1][1], * Lc = &cube[L][1][1], * Dc = &cube[D][1][1];
	char* U1 = &cube[U][2][0], * U2 = &cube[U][2][2], * R1 = &cube[R][0][0], * R2 = &cube[R][2][0];
	char* L1 = &cube[L][0][2], * L2 = &cube[L][2][2], * D1 = &cube[D][0][0], * D2 = &cube[D][0][2];
	if (*U1 == *U2 && *R1 == *R2 && *L1 == *L2 && *D1 == *D2)
		return;
	if (*U1 != *U2 && *R1 != *R2 && *L1 != *L2 && *D1 != *D2)
		move(cube, "RU'RDDR'URDDRR");
	while (*D1 != *D2)
		front(cube, 1);
	move(cube, "RU'RDDR'URDDRR");
}
void TopEdge(char cube[][3][3])
{
	char* Uc = &cube[U][1][1], * Rc = &cube[R][1][1], * Lc = &cube[L][1][1], * Dc = &cube[D][1][1];
	char* Um = &cube[U][2][1], * Rm = &cube[R][1][0], * Lm = &cube[L][1][2], * Dm = &cube[D][0][1];
	while (cube[U][2][2] != *Uc)
		front(cube, 1);
	if (*Um != *Uc && *Rm != *Rc && *Lm != *Lc && *Dm != *Dc)
		move(cube, "RF'RFRFRF'R'F'RR");
	while (*Um != *Uc)
		turn(cube);
	while (*Rm != *Rc || *Lm != *Lc || *Dm != *Dc)
		move(cube, "RF'RFRFRF'R'F'RR");
}
void Solute(char cube[][3][3])
{
	Command = "";
	DownCross(cube);
	DownMid(cube);
	DownCorner(cube);
	MidEdge(cube);
	TopCross(cube);
	TopCorner(cube);
	TopLayerCorner(cube);
	TopEdge(cube);
}




// 新增：验证着色是否合法（基本检查：字符合法性、每种颜色出现9次、中心颜色正确、无空格）
bool validateColoring(const char cube[6][3][3], string &reason) {
	// count occurrences of allowed colors
	map<char, int> cnt;
	for (int i = 0; i < 6; ++i) cnt[Color[i]] = 0;

	for (int f = 0; f < 6; ++f) {
		for (int r = 0; r < 3; ++r) {
			for (int c = 0; c < 3; ++c) {
				char ch = cube[f][r][c];
				if (ch == ' ') {
					reason = "存在未上色的贴纸，无法还原";
					return false;
				}
				if (cnt.find(ch) == cnt.end()) {
					ostringstream ss; ss << "存在非法颜色字符: '" << ch << "'";
					reason = ss.str();
					return false;
				}
				cnt[ch]++;
			}
		}
	}

	// each color must appear exactly 9 times
	for (int i = 0; i < 6; ++i) {
		char ch = Color[i];
		if (cnt[ch] != 9) {
			ostringstream ss; ss << "颜色 " << ch << " 出现次数不为 9 (实际 " << cnt[ch] << ")，无法还原";
			reason = ss.str();
			return false;
		}
	}

	// centers must match standard center colors
	for (int f = 0; f < 6; ++f) {
		if (cube[f][1][1] != Color[f]) {
			ostringstream ss; ss << "第 " << f << " 面中心颜色不正确，无法还原";
			reason = ss.str();
			return false;
		}
	}

	// basic checks passed
	reason = "";
	return true;
}


//补充
// 判断Cube数组是否复原
bool isCubeSolved(const char Cube[6][3][3]) {
	for (int f = 0; f < 6; ++f)
		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
				if (Cube[f][i][j] != Cube[f][1][1])
					return false;
	return true;
}







