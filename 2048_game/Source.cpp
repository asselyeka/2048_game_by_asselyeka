#include <iostream>
#include <Windows.h>
#include <time.h>
#include <conio.h>
#include <fstream>

using namespace std;

// Функция для выбора позиции
void SetPos(int x, int y) {// функция для установки позиции каретки по координатам
	COORD cd;
	cd.X = x;
	cd.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cd);
}

// функция для смена цвета
void SetColor(WORD wAttributes) {
	HANDLE hOUTPUT = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hOUTPUT, wAttributes);
}

// Функция для скрытия каретки
void CursorHide(BOOL bVis = FALSE) {
	HANDLE hOUTPUT = GetStdHandle(STD_OUTPUT_HANDLE);
	// Get console handle
	hOUTPUT = GetStdHandle(STD_OUTPUT_HANDLE);

	// Hide console cursor
	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.dwSize = 1;
	cursorInfo.bVisible = 0;
	SetConsoleCursorInfo(hOUTPUT, &cursorInfo);
}

enum keyboard {
	key_left = 75,
	key_right = 77,
	key_up = 72,
	key_down = 80,
	key_esc = 27,
	KEY_BACKSPACE = 8,
	KEY_DELETE = 83,
	KEY_F1 = 59,
};

int random(int a)  //ф-ция генерирует случайные числа
{
	int index;
	index = rand() % a;

	return index;
}

struct Game
{
	int step; //переменная для определения следующего шага в игре: 0 - игра продолжается, -1 - конец игры
	int end;  //переменная для определения следующего шага в игре: 1-конец игры, 0 - игра продолжается
	char name[20]; //имя пользлвателя

	int max; //максимальная собранная цифра на игравом поле
	int win = 2048; //максимальная цифра, которую необходимо собрать, чтобы выйграть игру
	int dif; //набранные очки за один шаг
	int score; //набранные очки за всю игру
	int pole[4][4]; //массив игравого поле
	int bpole[4][4];//массив игравого поле на один шаг ранее

	char answer;// ответ на вопрос - Вы хотите продолжить игру? : да или нет
	int press_enter;

	//--------------------------------------------------------------------------------------------------------------------
	//сохранение игры в файл
	void SaveFile()
	{
		fstream file;
		file.open("OldGame.txt", ios::out | ios::trunc);
		file << max << " " << score << " ";
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				file << pole[i][j] << " ";
		}
		file.close();
	}
	//------------------------------------------------------------------------------------------------------------------
	//чтение с файла сохраненной последней игры
	void OldGame()
	{
		ifstream file;
		file.open("OldGame.txt", ios::in);
		//file.seekg(0, ios::beg);
		file >> max >> score;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				file >> pole[i][j];
		}
		file.close();
	}
	//--------------------------------------------------------------------------------------------
	//логические шаги игры
	void logic_game(Game *execute, keyboard key)
	{
		switch (key)
		{
		case key_up:
		case key_left:
		case key_down:
		case key_right:
			execute->backup_pole();
			execute->fill_space(key);
			execute->update_pole(key);
			execute->fill_space(key);
			execute->find_max();
			//execute->display_pole();

			if (execute->full() && end)
			{
				step = -1;
				break;
			}
			else if (execute->block_moves())
			{
				execute->add();
				break;
			}
			else
			{
				step = 0;
				break;
			}

		case KEY_BACKSPACE:
			if (execute->block_moves())
				score -= dif;
			execute->cancel();
			break;


		case KEY_DELETE:
			execute->start_pole();
			score = 0;
			dif = 0;
			break;

		case key_esc:
			step = -1;
			break;

		case KEY_F1:
			execute->display_help();
			press_enter = getchar();
			system("cls");
			setlocale(LC_ALL, "C");
			execute->display_pole();
			break;

		}
	}

	//----------------------------------------------------------------------------------
	// проверка окончания игры
	void game_end_check(Game *screen)
	{
		if (max == win)
		{
			screen->display_win();

			if (screen->display_try_again(0) == 'n')
				step = -1;
			else
			{
				win *= 2;
				system("cls");
				setlocale(LC_ALL, "C");
			}
		}

		else if (step == -1)
		{
			screen->display_gameover();

			if (screen->display_try_again(1) == 'y')
			{
				system("cls");
				setlocale(LC_ALL, "C");
				screen->start_pole();
				step = 0;
			}
		}

		if (step == -1)
		{
			SaveFile();
			cout << "\n\n\t\t            > Спасибо за игру. "
				<< "\n\n\n\t\t\t\t\t\t   Разработано Молдашевой Асель \n\n";
			Sleep(2000);
			press_enter = getchar();
			exit(0);
		}
	}
	//--------------------------------------------------------------------------------
	// новое поле
	void start_pole()
	{
		int i, j;

		dif = 0;
		score = 0;
		max = 0;

		for (i = 0; i<4; i++)
			for (j = 0; j<4; j++)
				pole[i][j] = 0;

		i = random(4);
		j = random(4);

		pole[i][j] = 2;

		i = random(4);
		j = random(4);

		pole[i][j] = 2;
	}
	//---------------------------------------------------------------------------
	//измененное поле после одного шага
	void update_pole(keyboard key)
	{
		dif = 0;
		end = 1;

		switch (key)
		{
		case key_up:
			for (int i = 0; i<4; i++)
				for (int j = 0; j<3; j++)
				{
					if (pole[j][i] && pole[j][i] == pole[j + 1][i])
					{
						end = 0;
						pole[j][i] += pole[j + 1][i];
						pole[j + 1][i] = 0;
						dif += pole[j][i];
						score += pole[j][i];

					}
				}break;

		case key_down:
			for (int i = 0; i<4; i++)
				for (int j = 3; j>0; j--)
				{
					if (pole[j][i] && pole[j][i] == pole[j - 1][i])
					{
						end = 0;
						pole[j][i] += pole[j - 1][i];
						pole[j - 1][i] = 0;
						dif += pole[j][i];
						score += pole[j][i];
					}
				}break;

		case key_left:
			for (int i = 0; i<4; i++)
				for (int j = 0; j<3; j++)
				{
					if (pole[i][j] && pole[i][j] == pole[i][j + 1])
					{
						end = 0;
						pole[i][j] += pole[i][j + 1];
						pole[i][j + 1] = 0;
						dif += pole[i][j];
						score += pole[i][j];
					}
				}break;

		case key_right:
			for (int i = 0; i<4; i++)
				for (int j = 3; j>0; j--)
				{
					if (pole[i][j] && pole[i][j] == pole[i][j - 1])
					{
						end = 0;
						pole[i][j] += pole[i][j - 1];
						pole[i][j - 1] = 0;
						dif += pole[i][j];
						score += pole[i][j];
					}
				}break;


		}



	}
	//-----------------------------------------------------------------------------------------------------------	
	//скидывание всех плиток игрового поля в одну из 4 сторон и заполнение результатов
	void fill_space(keyboard key)
	{

		switch (key)
		{
		case key_up:
			for (int i = 0; i<4; i++)
				for (int j = 0; j<4; j++)
				{
					if (!pole[j][i])
					{
						for (int k = j + 1; k<4; k++)
							if (pole[k][i])
							{
								pole[j][i] = pole[k][i];
								pole[k][i] = 0;
								break;
							}
					}

				}break;

		case key_down:
			for (int i = 0; i<4; i++)
				for (int j = 3; j >= 0; j--)
				{
					if (!pole[j][i])
					{
						for (int k = j - 1; k >= 0; k--)
							if (pole[k][i])
							{
								pole[j][i] = pole[k][i];
								pole[k][i] = 0;
								break;
							}
					}

				}break;
		case key_left:
			for (int i = 0; i<4; i++)
				for (int j = 0; j<4; j++)
				{
					if (!pole[i][j])
					{
						for (int k = j + 1; k<4; k++)
							if (pole[i][k])
							{
								pole[i][j] = pole[i][k];
								pole[i][k] = 0;
								break;
							}
					}

				}break;


		case key_right:
			for (int i = 0; i<4; i++)
				for (int j = 3; j >= 0; j--)
				{
					if (!pole[i][j])
					{
						for (int k = j - 1; k >= 0; k--)
							if (pole[i][k])
							{
								pole[i][j] = pole[i][k];
								pole[i][k] = 0;
								break;
							}
					}

				}break;

		}
	}
	//--------------------------------------------------------------------------------------------------
	//шаг игры и появление новых плиток
	void add()
	{
		int i, j, k;

		do
		{
			i = random(4);
			j = random(4);
			k = random(11);

		} while (pole[i][j]);



		if (k<1)
			pole[i][j] = 4;

		else
			pole[i][j] = 2;


	}
	//-----------------------------------------------------------------------------------------------------
	//поиск максимальной плитки
	void find_max()
	{
		for (int i = 0; i<4; i++)
			for (int j = 0; j<4; j++)
				if (pole[i][j]>max)
					max = pole[i][j];
	}
	//----------------------------------------------------------------------------------------------------------
	//копирование поле предыдущего шага
	void backup_pole()
	{
		for (int i = 0; i<4; i++)
			for (int j = 0; j<4; j++)
				bpole[i][j] = pole[i][j];
	}
	//------------------------------------------------------------------------------------------------------------
	//отмена предыдущего шага
	void cancel()
	{
		for (int i = 0; i<4; i++)
			for (int j = 0; j<4; j++)
				pole[i][j] = bpole[i][j];
	}
	//--------------------------------------------------------------------------------------------------------------
	//проверка заполненного поле
	int full()
	{
		int k = 1;

		for (int i = 0; i<4; i++)
			for (int j = 0; j<4; j++)
			{
				if (!pole[i][j])
					k = 0;

			}
		return k;
	}
	//----------------------------------------------------------------------------------------------------------------------
	//проверка шага, при котором не было изменении
	int block_moves()
	{
		int k = 0;

		for (int i = 0; i<4; i++)
			for (int j = 0; j<4; j++)
				if (bpole[i][j] != pole[i][j])
				{
					k = 1;
					break;
				}
		return k;
	}
	//-----------------------------------------------------------------------------------------------------------------------------
	//печать поле на консоль
	void display_pole()
	{
		//system("cls");
		int posX = 5, posY = 5;
		SetPos(posX, posY);

		const int row = 28, column = 50;
		char pole_img[row][column]{
			"#################################################",
			"#  **[  GAME 2048  ]**     Developed            #",
			"#                          Assel Moldasheva     #",
			"#                                               #",
			"#                          SCORE:               #",
			"#                                               #",
			"#    Control               Backspace - UNDO     #",
			"#       ^                  Delete - RESTART     #",
			"#    <     >               F1 - HELP            #",
			"#       v                  Esc - EXIT           #",
			"#                                               #",
			"#################################################",
			"#           #           #           #           #",
			"#           #           #           #           #",
			"#           #           #           #           #",
			"#################################################",
			"#           #           #           #           #",
			"#           #           #           #           #",
			"#           #           #           #           #",
			"#################################################",
			"#           #           #           #           #",
			"#           #           #           #           #",
			"#           #           #           #           #",
			"#################################################",
			"#           #           #           #           #",
			"#           #           #           #           #",
			"#           #           #           #           #",
			"#################################################",
		};



		for (int y = 0; y < row; y++) {
			SetPos(posX, posY + y);
			for (int x = 0; x < column; x++) {
				if (pole_img[y][x] == '#') { SetColor(0xA); cout << char(177); }
				else if (pole_img[y][x] == '*') { SetColor(0xC); cout << "*"; }
				else if (pole_img[y][x] == ' ')	cout << ' ';
				else if (pole_img[y][x] == '&') { SetColor(0xE); cout << char(2); }
				else cout << pole_img[y][x];
			}
		}


		//отображение набранных очков за один шаг
		if (dif) {
			SetPos(posX + 8, posY + 4);
			cout << "+" << dif << "!";
		}
		else {
			SetPos(posX + 8, posY + 4);
			cout << "    ";
		}

		//отображение набранных очков за всю игру
		SetPos(posX + 36, posY + 4);
		cout << score;

		//отображение плиток
		for (int i = 0; i<4; i++)
		{
			for (int j = 0; j<4; j++)
			{
				if (pole[i][j]) {
					SetPos((posX + 4 + 13 * j), (posY + 13 + i * 4));
					if (pole[i][j] == 2) SetColor(0xA);		  //зеленый
					if (pole[i][j] == 4) SetColor(0xB);		//голубой
					if (pole[i][j] == 8) SetColor(0xC);				   //красный
					if (pole[i][j] == 16) SetColor(0xD);	 //фмолетовый
					if (pole[i][j] == 32) SetColor(0xE);	  //желтый
					if (pole[i][j] == 64) SetColor(0xF);	 //белый
					if (pole[i][j] == 128) SetColor(0x5);  //	  темно фиолетовый
					if (pole[i][j] == 256) SetColor(0x6);  //темно желтый
					if (pole[i][j] == 512) SetColor(0x7);  //белый
					if (pole[i][j] == 1024) SetColor(0x8);	//серый
					if (pole[i][j] == 2048) SetColor(0x9);
					printf("%4d", pole[i][j]);
				}
				else {
					SetPos((posX + 4 + 13 * j), (posY + 13 + i * 4));
					printf("%4c", ' ');
				}
			}
		}

	}
	//------------------------------------------------------------------------------------------------------------
	//отображение справочной информации и информации об авторе при нажатии на F1
	void display_help()
	{
		system("cls");
		setlocale(0, "rus");
		cout << endl << "  **[  ИГРА 2048  ]**\t\t\t\tРазработано Молдашевой Асель";

		char text[] = "> Игра 2048>	Правила игры>1.	В каждом раунде появляется плитка номинала «2»(с вероятностью 90, 9090909 % ) или «4»(с> вероятностью 9, 09090909 %)>2.	Нажатием стрелки игрок может скинуть все плитки игрового поля в одну из 4 сторон.Если при >сбрасывании две плитки одного номинала «налетают» одна на другую, то они слипаются в >одну, номинал которой равен сумме соединившихся плиток.После каждого хода на >свободной секции поля появляется новая плитка номиналом «2» или «4».Если при нажатии >кнопки местоположение плиток или их номинал не изменится, то ход не совершается.>3.	Если в одной строчке или в одном столбце находится более двух плиток одного номинала, то >при сбрасывании они начинают слипаться с той стороны, в которую были направлены. >Например, находящиеся в одной строке плитки(4, 4, 4) после хода влево они превратятся в >(8, 4), а после хода вправо — в(4, 8).Данная обработка неоднозначности позволяет более >точно формировать стратегию игры.>4.	За каждое соединение игровые очки увеличиваются на номинал получившейся плитки.>5.	Игра заканчивается поражением, если после очередного хода невозможно совершить >действие.>>> Чтобы продолжить нажмите Enter...";

		for (int i = 0; text[i]; i++)
		{
			if (text[i] == '>')
			{
				text[i] = ' ';
				cout << "\n";
			}

			cout << text[i];
		}

	}
	//-----------------------------------------------------------------------------------------------------------------------------
	//вопрос о продолжении игры 
	char display_try_again(int lose)
	{
		if (lose)
			cout << "\n\n\n\t    > Вы хотите попытать свою удачу еще раз? " << name << " (y/n) ? : ";
		else
			cout << "\n\n\n\t    > Вы хотите продолжить игру? " << name << " (y/n) ? : ";

		cin >> answer;


		return answer;
	}
	//----------------------------------------------------------------------------------------------------------------------------
	//экран победителя
	void display_win()
	{
		system("cls");
		setlocale(0, "rus");

		cout << endl << endl;
		cout << "\n\t\t\t  ** [  ВЫ НАБРАЛИ " << win << "!  ] **"
			<< "\n\n\t\t\t  ** [ ВЫ ВЫЙГРАЛИ!!! ] **"
			<< "\n\n\n\n\t\t\t ПЛИТКИ\t     ОЧКИ\t    ИМЯ";
		printf("\n\n\t\t\t %4d\t      %6d\t    ", max, score);
		cin >> name;
	}
	//-------------------------------------------------------------------------------------------------------------------------
	//экран проигравшего
	void display_gameover()
	{
		system("cls");
		setlocale(0, "rus");

		cout << "\n\n\n\t\t\t  :: [ G A M E  O V E R ] ::"
			<< "\n\n\n\n\t\t\t ПЛИТКА\t     ОЧКИ\t    ИМЯ";
		printf("\n\n\t\t\t %4d\t    %6d\t    ", max, score);
		cin >> name;
	}
	//--------------------------------------------------------------------------------------------

};


void main()
{
	srand(time(0));
	CursorHide();
	Game exec;
	char a;
	SetPos(18, 9);
	cout << "Continue old game? (y/n) :";
	cin >> a;
	if (a == 'y') exec.OldGame();
	else exec.start_pole();

	int key;
	for (;;) {
		key = _getch();
		exec.logic_game(&exec, (keyboard)key);
		exec.display_pole();
		exec.game_end_check(&exec);

	}

	cin.get(); cin.get();
}