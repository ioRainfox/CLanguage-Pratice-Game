#include <stdio.h>
#include<stdlib.h>
#include <Windows.h>
#include <time.h>


#pragma region Enum
enum COLOR
{
	BLACK,
	BLUE,
	GREEN,
	CYAN,
	RED,
	MAGENTA,
	BROWN,
	LIGHTGRAY,
	DARKGRAY,
	LIGHTBLUE,
	LIGHTGREEN,
	LIGHTCYAN,
	LIGHTRED,
	LIGHTMAGENTA,
	YELLOW,
	WHITE,
};

enum SCENE_ID
{
	LOGO,
	MENU,
	STAGE,
	LEVEL,
	HP,
	ATK,
	DEF,
	GAMEOVER,

};

enum Dir
{
	LEFT,
	RIGHT,
	UP,
	DOWN,
};

#pragma endregion


#pragma region Double Buffer
#define BufferWidth 120	
#define BufferHeight 40	

HANDLE hBuffer[2];
int screenIndex;

//버퍼 초기화
void InitBuffer();
//버퍼 뒤집기
void FlipBuffer();
//버퍼 창 깨끗이
void ClearBuffer();
//버퍼 쓰기
void WriteBuffer(int x, int y, const char* shape, int color);
//버퍼 해제
void ReleaseBuffer();
#pragma endregion

#pragma region Struct

#define BC 10

struct Bullet
{
	bool act;
	bool fire;
	int x;
	int y;
	Dir dir;
	COLOR color;
	const char* shape;
};

#define MAX_ITEM 20

struct Item
{
	bool act;
	int x;
	int y;
	int exp;
	COLOR color;
	const char* shape;
};

#define MAX_ENEMY 10

typedef struct Obj
{

	int x;
	int y;
	Dir dir;
	COLOR color;
	const char* shape;
	int life;
	int atk;
	int def;
	int exp;
}Player, Enemy;

struct Option
{
	int x;
	int y;
	const char* shape;
};

Bullet* bullets[BC] = {};

Item* items[MAX_ITEM] = {};

Player* player = nullptr;
Enemy* enemies[MAX_ENEMY] = {};

Option* hp = nullptr;
Option* atk = nullptr;
Option* def = nullptr;
Option* arrow = nullptr;

#pragma endregion

SCENE_ID id;

void SceneManager();

void Logo();
void Menu();
void Stage();

void EnemyGen();
void ItemGen();
void LevelInit();
void Level();

void GameOverInit();
void GameOver();

void InitStage();
Bullet* FireBullet();
void ProgressStage();
void RenderStage();
void ReleaseStage();


int EnemyMove = 0;
int score = 0;
int index;

int main()
{
	srand((unsigned)time(NULL));

	InitBuffer();

	id = MENU;
	InitStage();

	while (true)
	{
		SceneManager();

		FlipBuffer();
		ClearBuffer();

		Sleep(50);
	}

	return 0;
}

void SceneManager()
{
	switch (id)
	{
	case MENU:
		Menu();
		break;
	case STAGE:
		ProgressStage();
		RenderStage();
		break;
	case LEVEL:
		Level();
		break;
	case HP:
		break;
	case ATK:
		player->atk += 5;
		id = STAGE;
		break;
	case DEF:
		player->def += 2;
		break;
	case GAMEOVER:
		GameOver();
		break;
	default:
		break;
	}
}

void Logo()
{
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		id = MENU;
	}
	WriteBuffer(10, 10, "Logo", WHITE);
}

void Menu()
{
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		id = STAGE;
	}
	WriteBuffer(10, 10, "Menu", WHITE);
}

void Stage()
{
}

#pragma region Stage

void EnemyGen()
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemies[i] == nullptr)
		{
			enemies[i] = (Enemy*)malloc(sizeof(Enemy));

			bool posit;
			do
			{
				posit = true;
				enemies[i]->x = rand() % 60;
				enemies[i]->y = rand() % 40;

				for (int j = 0; j < i; j++)
				{
					if (enemies[j] != nullptr && enemies[i]->x == enemies[j]->x && enemies[i]->y == enemies[j]->y)
					{
						posit = false;
						break;
					}
				}
			} while (abs(enemies[i]->x - player->x) < 5 && abs(enemies[i]->y - player->y) < 5);

			enemies[i]->color = RED;
			enemies[i]->shape = "봇";
			enemies[i]->life = 100;
			enemies[i]->atk = 10;
			enemies[i]->def = 0;
		}
	}
}

void ItemGen()
{
	for (int i = 0; i < MAX_ITEM; i++)
	{
		if (items[i] == nullptr)
		{
			items[i] = (Item*)malloc(sizeof(Item));
			items[i]->act = false;
			items[i]->x = rand() % 60;
			items[i]->y = rand() % 40;
			items[i]->exp = 50;
			items[i]->color = YELLOW;
			items[i]->shape = "■";
		}
	}
}

void LevelInit()
{
	hp = (Option*)malloc(sizeof(Option));
	hp->x = 17;
	hp->y = 20;
	hp->shape = "HP";

	atk = (Option*)malloc(sizeof(Option));
	atk->x = 17;
	atk->y = 22;
	atk->shape = "ATK";

	def = (Option*)malloc(sizeof(Option));
	def->x = 17;
	def->y = 24;
	def->shape = "DEF";

	arrow = (Option*)malloc(sizeof(Option));
	arrow->x = 14;
	arrow->y = 20;
	arrow->shape = "▶";

	index = 0;
}

void Level()
{
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		switch (index)
		{
		case 0:
			id = HP;
			break;
		case 1:
			id = ATK;
			break;
		case 2:
			id = DEF;
			break;
		default:
			break;
		}
	}

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		if (arrow->y != hp->y)
		{
			arrow->y -= 2;
			index--;
		}

	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		if (arrow->y != def->y)
		{
			arrow->y += 2;
			index++;

		}

	}

	WriteBuffer(hp->x, hp->y, hp->shape, WHITE);
	WriteBuffer(atk->x, atk->y, atk->shape, WHITE);
	WriteBuffer(def->x, def->y, def->shape, WHITE);
	WriteBuffer(arrow->x, arrow->y, arrow->shape, WHITE);
}

void GameOverInit()
{
}

void GameOver()
{
	char scoreInfo[100];
	snprintf(scoreInfo, sizeof(scoreInfo), "Score: %d", score);
	WriteBuffer(20, 20, scoreInfo, WHITE);

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		id = MENU;
		ReleaseStage();
		InitStage();
	}
}

void InitStage()
{
	if (player == nullptr)
	{
		player = (Player*)malloc(sizeof(Player));
		player->x = 16;
		player->y = 20;
		player->color = WHITE;
		player->shape = "옷";
		player->life = 100;
		player->atk = 10;
		player->def = 0;
		player->exp = 0;
	}

	EnemyGen();
}

Bullet* FireBullet()
{
	for (int i = 0; i < BC; i++)
	{
		if (bullets[i] != nullptr)
		{
			if (bullets[i]->act == false)
			{
				bullets[i]->act = true;
				bullets[i]->fire = true;
				bullets[i]->dir = player->dir;
				bullets[i]->x = player->x;
				bullets[i]->y = player->y;
				return bullets[i];
			}
		}
		else
		{
			bullets[i] = (Bullet*)malloc(sizeof(Bullet));
			bullets[i]->act = false;
			bullets[i]->fire = false;
			bullets[i]->x = 30 + i;
			bullets[i]->y = 0;
			bullets[i]->color = BLUE;
			bullets[i]->shape = "*";
		}
	}
	return nullptr;
}

void ProgressStage()
{
	// 키입력
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		player->dir = LEFT;
		player->x--;
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		player->dir = RIGHT;
		player->x++;
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		player->dir = UP;
		player->y--;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		player->dir = DOWN;
		player->y++;
	}
	if (GetAsyncKeyState(0x5A) & 0x8000)
	{
		Bullet* bullet = FireBullet();
		if (bullet != nullptr)
		{
			bullet->act = true;
			bullet->x = player->x;
			bullet->y = player->y;
		}
	}

	//적
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemies[i] != nullptr)
		{
			if (EnemyMove % 3 == 0) //속도
			{
				int targetX = enemies[i]->x;
				int targetY = enemies[i]->y;

				//적 player쪽으로 
				if (targetX > player->x)
				{
					targetX--;
				}
				else if (player->x > targetX)
				{
					targetX++;
				}
				if (targetY > player->y)
				{
					targetY--;
				}
				else if (player->y > targetY)
				{
					targetY++;
				}

				bool canM = true;

				for (int j = 0; j < MAX_ENEMY; j++)
				{
					if (i != j && enemies[j] != nullptr && targetX == enemies[j]->x && targetY == enemies[j]->y)
					{
						canM = false;
						break;
					}
				}

				if (canM)
				{
					enemies[i]->x = targetX;
					enemies[i]->y = targetY;
				}
			}
			EnemyMove++;
		}

		//총맞을때
		if (enemies[i] != nullptr)
		{
			for (int j = 0; j < BC; j++)
			{
				if (bullets[j] != nullptr && bullets[j]->act)
				{
					if (bullets[j]->x == enemies[i]->x && bullets[j]->y == enemies[i]->y)
					{
						bullets[j]->act = false;
						bullets[j]->fire = false;

						enemies[j]->life -= player->atk;

						if (enemies[i]->life <= 0)
						{
							for (int g = 0; g < MAX_ITEM; g++)
							{
								ItemGen();
								items[i]->act = true;
							}
							score++;

							free(enemies[i]);
							enemies[i] = nullptr;

							EnemyGen();
						}

						break;
					}
				}
			}
		}

		if (enemies[i] != nullptr)
		{
			if (player->x == enemies[i]->x && player->y == enemies[i]->y)
			{
				player->life -= enemies[i]->atk - player->def;

				free(enemies[i]);
				enemies[i] = nullptr;

				//player 체력 0일때
				if (player->life <= 0)
				{
					id = GAMEOVER;
				}
				EnemyGen();

			}
		}
	}
	for (int i = 0; i < MAX_ITEM; i++)
	{
		if (items[i] != nullptr)
		{
			if (player->x == items[i]->x && player->y == items[i]->y)
			{
				items[i]->act = false;
				player->exp += items[i]->exp;

				if (player->exp == 100)
				{
					id = LEVEL;
				}

				free(items[i]);
				items[i] = nullptr;
			}
		}
	}
}



void RenderStage()
{
	WriteBuffer(player->x, player->y, player->shape, player->color);
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemies[i] != nullptr)
		{
			WriteBuffer(enemies[i]->x, enemies[i]->y, enemies[i]->shape, enemies[i]->color);
		}
	}

	for (int i = 0; i < MAX_ITEM; i++)
	{
		if (items[i] != nullptr && items[i]->act == true)
		{
			WriteBuffer(items[i]->x, items[i]->y, items[i]->shape, items[i]->color);
		}
	}

	for (int i = 0; i < BC; i++)
	{
		if (bullets[i] != nullptr && bullets[i]->act)
		{
			if (bullets[i]->fire)
			{
				switch (bullets[i]->dir)
				{
				case RIGHT:
					bullets[i]->x++;
					break;
				case LEFT:
					bullets[i]->x--;
					break;
				case UP:
					bullets[i]->y--;
					break;
				case DOWN:
					bullets[i]->y++;
				default:
					break;
				}
			}

			if (bullets[i]->y < 0 || bullets[i]->y >= 40 || bullets[i]->x < 0 || bullets[i]->x >= 60)
			{
				bullets[i]->act = false;
				bullets[i]->x = 30 + i;
				bullets[i]->y = 0;
			}

			if (bullets[i]->act)
			{
				WriteBuffer(bullets[i]->x, bullets[i]->y, bullets[i]->shape, bullets[i]->color);
			}
		}

	}

	char healthInfo[100];
	snprintf(healthInfo, sizeof(healthInfo), "Health: %d", player->life);
	WriteBuffer(0, 0, healthInfo, WHITE);

	char atkInfo[100];
	snprintf(atkInfo, sizeof(atkInfo), "Atk: %d", player->atk);
	WriteBuffer(10, 0, atkInfo, WHITE);

	char defInfo[100];
	snprintf(defInfo, sizeof(defInfo), "Def: %d", player->def);
	WriteBuffer(15, 0, defInfo, WHITE);

	char expInfo[100];
	snprintf(expInfo, sizeof(expInfo), "Exp: %d", player->exp);
	WriteBuffer(20, 0, expInfo, WHITE);

	char scoreInfo[100];
	snprintf(scoreInfo, sizeof(scoreInfo), "Score: %d", score);
	WriteBuffer(50, 0, scoreInfo, WHITE);
}

void ReleaseStage()
{
	if (player != nullptr)
	{
		free(player);
		player = nullptr;
	}
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemies[i] != nullptr)
		{
			free(enemies[i]);
			enemies[i] = nullptr;
		}
	}
	for (int i = 0; i < BC; i++)
	{
		if (bullets[i] != nullptr)
		{
			free(bullets[i]);
			bullets[i] = nullptr;
		}
	}

	for (int i = 0; i < MAX_ITEM; i++)
	{
		if (items[i] != nullptr)
		{
			free(items[i]);
			items[i] = nullptr;
		}
	}

	score = 0;

	if (hp != nullptr)
	{
		free(hp);
		hp = nullptr;
	}
	if (atk != nullptr)
	{
		free(atk);
		atk = nullptr;
	}
	if (def != nullptr)
	{
		free(def);
		def = nullptr;
	}
	if (arrow != nullptr)
	{
		free(arrow);
		arrow = nullptr;
	}
}

#pragma endregion

#pragma region Double Buffer
void InitBuffer()
{
	screenIndex = 0;

	COORD size = { BufferWidth, BufferHeight };

	SMALL_RECT rect = { 0, 0, BufferWidth - 1, BufferHeight - 1 };

	hBuffer[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleScreenBufferSize(hBuffer[0], size);
	SetConsoleWindowInfo(hBuffer[0], TRUE, &rect);

	hBuffer[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleScreenBufferSize(hBuffer[1], size);
	SetConsoleWindowInfo(hBuffer[1], TRUE, &rect);

	CONSOLE_CURSOR_INFO info;
	info.dwSize = 1;
	info.bVisible = FALSE;

	SetConsoleCursorInfo(hBuffer[0], &info);
	SetConsoleCursorInfo(hBuffer[1], &info);
}

void FlipBuffer()
{
	SetConsoleActiveScreenBuffer(hBuffer[screenIndex]);

	screenIndex = !screenIndex;
}

void ClearBuffer()
{
	COORD pos = { 0,0 };
	DWORD dw;

	FillConsoleOutputCharacter(hBuffer[screenIndex], ' ', BufferWidth * BufferHeight, pos, &dw);
}

void WriteBuffer(int x, int y, const char* shape, int color)
{
	COORD pos = { x * 2, y };

	SetConsoleCursorPosition(hBuffer[screenIndex], pos);
	SetConsoleTextAttribute(hBuffer[screenIndex], color);

	DWORD dw;
	WriteFile(hBuffer[screenIndex], shape, strlen(shape), &dw, NULL);
}
void ReleaseBuffer()
{
	CloseHandle(hBuffer[0]);
	CloseHandle(hBuffer[1]);
}
#pragma endregion