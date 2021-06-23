#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

int sizewindow = 700; //Размер окна
const int kkl = 700; //Кол-во клеток
int klsize = sizewindow / (kkl);
const int	width = sizewindow + klsize, //Ширина окна
			height = sizewindow; //Высота окна
int x, y; //Позиция игрока
CircleShape player(klsize / 2); //Игрок
const string windowTitle = "wtf?"; //Название окна
bool win = 0; //Победа

double trg[4][3][2] //Стрелки
{
	{{0.5, 0}, {0, 1}, {1, 1}}, 
	{{1, 0.5}, {0, 0}, {0, 1}}, 
	{{0, 0}, {1, 0}, {0.5, 1}}, 
	{{0, 0.5}, {1, 0}, {1, 1}}
};

//НАСТРОЙКИ ---------------------------------------------
int cooldown = 0; //Ожидание следущего хода
int lvisibly = 5; //Дальность подсветки
int visiblyclr = 10; //Сила подсветки
bool night = 1; //Ночной режим
bool rnd = 1; //Случайное поле
bool res = 0; //Начинать сначала после победы
bool inp = 0; //Чтение массива rot
bool optimasedmode = 1;		//Оптимизированный мод
double plusclr = 10;		//Соклько яркости прибавлять своей клетке
double minusclr = 0.1;		//Соклько яркости отнимать клеткам за ход
char rot[kkl][kkl] //Поворот стрелок из видео SciOne "ПУШКА #20"
{
	{'u', 'r', 'l', 'd', 'l', 'l', 'u', 'd'},
	{'l', 'd', 'u', 'u', 'r', 'l', 'r', 'd'},
	{'l', 'r', 'l', 'd', 'd', 'r', 'l', 'd'},
	{'l', 'd', 'r', 'r', 'd', 'u', 'u', 'u'},
	{'d', 'r', 'd', 'r', 'r', 'd', 'l', 'l'},
	{'d', 'u', 'r', 'd', 'r', 'd', 'd', 'u'},
	{'u', 'd', 'l', 'd', 'd', 'u', 'd', 'l'},
	{'l', 'l', 'l', 'l', 'u', 'u', 'r', 'd'}
};

RenderWindow window(VideoMode(width, height), windowTitle); //Окно

class kl
{
public:
	int rotate; // 0 /\  |  1 >  |  2 \/  |  3 <
	bool e; //Ошиба поворота
	bool at2; //Подсветка
	double at; //Яркость
	ConvexShape kls; //Стрелка

	void nrotate() //Поворот 
	{
		rotate++;
		if (rotate > 3)
		{
			rotate = 0;
		}
	}
	void act() //Игрок на этой клетке
	{
		int xo = x; //Запоминаем позиции игрока 
		int yo = y;
		do {
			e = 0;
			switch (rotate)
			{
			case 0:
				if (y > 0) y -= 1;
				else e = 1;
				break;
			case 1:
				if (x < kkl - 1 || y == kkl - 1 && x == kkl - 1) x += 1;
				else e = 1;
				break;
			case 2:
				if (y < kkl - 1) y += 1;
				else e = 1;
				break;
			case 3:
				if (x > 0) x -= 1;
				else e = 1;
				break;
			}
			nrotate();
		} while (e == 1);
		this->kls.setPoint(0, Vector2f(trg[rotate][0][0] * klsize + xo * klsize, trg[rotate][0][1] * klsize + yo * klsize)); //Ставим нужную стрелку
		this->kls.setPoint(1, Vector2f(trg[rotate][1][0] * klsize + xo * klsize, trg[rotate][1][1] * klsize + yo * klsize));
		this->kls.setPoint(2, Vector2f(trg[rotate][2][0] * klsize + xo * klsize, trg[rotate][2][1] * klsize + yo * klsize));
	}
};

kl mapp[kkl][kkl]; //Карта

void logic(); 
void WhileWindowOpen(); 
void CheckEvents(); 
void Setup();

int main()
{
	srand(time(nullptr)); //Случайные числа
	Setup();
	WhileWindowOpen();
	if (win)
		return 1;
	return 0;
}

void WhileWindowOpen()
{
	while (window.isOpen()) //Главный цикл
	{
		logic();
		if (win && !res)
		{
			return;
		}
		else if (win) //Заного
		{
			x = 0;
			y = 0;
			win = 0;
		}
	}
}

void logic()
{
	CheckEvents(); //События

	player.setPosition(x * klsize, y * klsize); //Позиция игрока

	window.clear();

	for (int i = 0; i < kkl; i++)
	{
		for (int j = 0; j < kkl; j++)
		{
			mapp[i][j].at2 = 0; //Клетка не подсвечена
			if (optimasedmode) //Задаём цвет клеткам
				mapp[i][j].kls.setFillColor(Color(255 - 255 / ((mapp[i][j].at / (plusclr * 10 + 1)) + 1), 0, 255 - 255 / ((mapp[i][j].at / (plusclr * 10 + 1)) + 1)));
			
			if (optimasedmode && (j <= x + lvisibly && j >= x - lvisibly) && (i <= y + lvisibly && i >= y - lvisibly)) //Подсветка клеток
			{
				if (mapp[i][j].kls.getFillColor().r + visiblyclr > 255 || mapp[i][j].kls.getFillColor().b + visiblyclr > 255) //Переполнение яркости
					mapp[i][j].kls.setFillColor(Color(255, 0, 255));
				else
					mapp[i][j].kls.setFillColor(Color(mapp[i][j].kls.getFillColor().r + visiblyclr, 0, mapp[i][j].kls.getFillColor().b + visiblyclr));
				mapp[i][j].at2 = 1; //Клетка подсвечена
			}

			if (mapp[i][j].at2 || mapp[i][j].at > 0 || !optimasedmode) //Отрисовка
			{
				if (!night) //Ночной режим
					window.draw(mapp[i][j].kls);
				else if (mapp[i][j].at2) //Подсвеченые клетки
					window.draw(mapp[i][j].kls);
				if (mapp[i][j].at > 0) //Клетка затухает
					mapp[i][j].at -= minusclr;
			}
		}
	}
	window.draw(player); //Рисуем игрока
	window.display();

	if (y == kkl - 1 && x == kkl) //Победа
	{
		cout << "WIN!" << endl;
		if (!res) 
			cin.get();
		else
			this_thread::sleep_for(chrono::milliseconds(500));
		win = 1;
		return;
	}

	mapp[y][x].act(); //Логика клетки
	if ((y != kkl - 1 || x != kkl) && optimasedmode) { //Если мы не на победной клетке то:
		mapp[y][x].at += plusclr; //Увеличиваем яркость своей клетке
	}
	
	this_thread::sleep_for(chrono::milliseconds(cooldown)); //Ожидание
}

void CheckEvents()
{
	Event event;
	while (window.pollEvent(event))
	{
		if (event.type == Event::Closed) //Событие закрытия окна
			window.close();
	}
}

void Setup()
{
	for (int i = 0; i < kkl; i++) //Заполнение поля
	{
		for (int j = 0; j < kkl; j++)
		{
			if (inp) {
				if (rot[i][j] == 'u')
				{
					mapp[i][j].rotate = 0;
				}
				else if (rot[i][j] == 'r')
				{
					mapp[i][j].rotate = 1;
				}
				else if (rot[i][j] == 'd')
				{
					mapp[i][j].rotate = 2;
				}
				else if (rot[i][j] == 'l')
				{
					mapp[i][j].rotate = 3;
				}
				else
				{
					if (rnd)
						mapp[i][j].rotate = rand() % 4;
					else
						mapp[i][j].rotate = 0;
				}
			}
			else
			{
				if (rnd)
					mapp[i][j].rotate = rand() % 4;
				else
					mapp[i][j].rotate = 0;
			}
		}
	}
	for (int i = 0; i < kkl; i++) //Создание стрелок
	{
		for (int j = 0; j < kkl; j++)
		{
			mapp[i][j].kls.setPointCount(3);
			mapp[i][j].kls.setPoint(0, Vector2f(trg[mapp[i][j].rotate][0][0] * klsize + j * klsize, trg[mapp[i][j].rotate][0][1] * klsize + i * klsize));
			mapp[i][j].kls.setPoint(1, Vector2f(trg[mapp[i][j].rotate][1][0] * klsize + j * klsize, trg[mapp[i][j].rotate][1][1] * klsize + i * klsize));
			mapp[i][j].kls.setPoint(2, Vector2f(trg[mapp[i][j].rotate][2][0] * klsize + j * klsize, trg[mapp[i][j].rotate][2][1] * klsize + i * klsize));
		}
	}
	player.setFillColor(Color(0, 255, 0)); //Цвет игрока
	window.setPosition(Vector2i(window.getPosition().x, 0)); //Позиция окна
}