#include <string.h>
#include <locale.h>
#include <dirent.h>
#include <unistd.h>

#include <ncursesw/ncurses.h>

#define FILESIZE 10485
#define DEFAULT_COLSCH 1
#define ERROR_COLSCH 2
#define SUCCESS_COLSCH 3
#define SUPERERROR_COLSCH 4
#define YELLOW_COLSCH 5
#define LINES_COLSCH 6
#define TITLE_COLSCH 7

int LinesCount = 1;
int FilePositions[2] = {0, 0};
int SpacesCount = 1;

void mainloop(char x[], char y[]);

/////////////////////////////////////////////////////////////////
//Скопипастил функцию на проверку существования и доступности директории из моей псевдо-утилиты filecreator нахуй
int isdirexist(char input[]) {

	int slashpos = 0;
	for (int i = 0; input[i]; i++) {
	
		if (input[i] == '/') {
			slashpos = i;
		}
	}

	char strs[strlen(input)];
	if (slashpos > 0) {
	
		strncpy(strs, input, slashpos);
	
	} else {
	
		char cwd[128];
		getcwd(cwd, sizeof(cwd));
	
		sprintf(strs, "%s/", cwd);
	
	}
	return opendir(strs) ? 1 : 0;
}
/////////////////////////////////////////////////////////////////

void rendertitle(char filename[], char message[], int colorid) {
	//Дичь с цветами нахуй

	attron(A_BOLD);	
	attron(COLOR_PAIR(ERROR_COLSCH));
	printw("\n \tF2 - Меню\n");

	attron(COLOR_PAIR(TITLE_COLSCH));
	printw(" Используемый файл: ");

	attron(COLOR_PAIR(DEFAULT_COLSCH));
	printw("%s\n", filename);

	attron(COLOR_PAIR(colorid));

	attron(COLOR_PAIR(colorid));
	printw("\n %s\n", message == "nth" ? "Текстовый редактор" : message);
	attron(COLOR_PAIR(DEFAULT_COLSCH));
	
	attron(COLOR_PAIR(LINES_COLSCH));
	for (int i = 0; i < getmaxx(stdscr); i++){
		printw("_");
	}
	printw("\n");
	attroff(A_BOLD);	

	refresh();
}

void printline(int indx, int strcount) {
	//Хуйня, которая выводит номера строк

	attron(COLOR_PAIR(LINES_COLSCH));
	attron(A_BOLD);	
	printw(" %d)", indx);

	char c[5];
	sprintf(c, "%d", indx);

	for (int i = strlen(c); i <= strcount; i++) {
		printw(" ");
	}
	attroff(A_BOLD);
	attron(COLOR_PAIR(DEFAULT_COLSCH));
}

void rendercontent(char input[]) {
	
	//Хуйня, которая считывает строки и выводит их 

	int i = 0, nowline = 1;
	LinesCount = 1;

	while (input[i]) {
		if (input[i] == '\n') {
			LinesCount++;
		}
		i++;
	}

	char c[5];
	sprintf(c, "%d", LinesCount);
	SpacesCount = strlen(c);

	printline(nowline, SpacesCount);

	i = 0;
	while (input[i]) {

		printw("%c", input[i]);
		if (input[i] == '\n') {

			nowline++;
			printline(nowline, SpacesCount);

		} 
		i++;
		
	}


}

	i = 0;
	while (input[i]) {

		printw("%c", input[i]);
		if (input[i] == '\n') {

			LinesCount++;
			printline(LinesCount);

		} 
		i++;
		
	}
int mainmenu(char input[], char dir[]) {
	//Менюшка 
	//TODO - прикрутить ебливый скроллер на стрелочки, было бы заебись

	clear();
	rendertitle(dir, "Меню", YELLOW_COLSCH);

	attron(A_BOLD);	
	attron(COLOR_PAIR(SUCCESS_COLSCH));
	printw(" 1 - Открыть\n");
	printw(" 2 - Сохранить\n");
	printw(" 3 - Сохранить как\n");
	printw(" 4 - Выйти\n\n");
	printw(" F2 - Вернуться\n");
	refresh();

	keypad(stdscr, 1);

	curs_set(0);
	int chr = getch();


	while (chr) {

		if (chr == '4') {			
			return 42;

		} else if (chr == KEY_F(2)) {			
			return 0;

		}


		chr = getch();
	}

	keypad(stdscr, 0);

}

void movecursor(int* x, int* y){

}

void mainloop(char input[], char dir[]) {
	//Главный цикл, где просчитывается всякая хуйня

	clear();
	rendertitle(dir, "nth", LINES_COLSCH);
	rendercontent(input);
	refresh();
	
	noecho();
	curs_set(1);	

	keypad(stdscr, 1);
	int chr = getch();

	while (chr) {

		if (chr == KEY_F(2)) {

			int x = mainmenu(input, dir);
			
			if (x == 42) { //Просто аргумент выхода из программы
				return;

			} else if (x == 0) {
				break;

			} else if (x == KEY_UP) {

			}

		}

		chr = getch();
	}

	keypad(stdscr, 0);

	mainloop(input, dir);
}

int main(int argc, char* args[]) {
	//Главная часть - тут крч все настройки и вызов функции цикла

	setlocale(LC_ALL, "");

	initscr();
	printw("\n");

	start_color();	

	//Цветовые схемы
    init_pair(DEFAULT_COLSCH, COLOR_WHITE, COLOR_BLACK);
    init_pair(ERROR_COLSCH, COLOR_RED, COLOR_BLACK);
    init_pair(SUCCESS_COLSCH, COLOR_GREEN, COLOR_BLACK);
    init_pair(SUPERERROR_COLSCH, COLOR_BLACK, COLOR_RED);
    init_pair(YELLOW_COLSCH, COLOR_YELLOW, COLOR_BLACK);	
    init_pair(LINES_COLSCH, COLOR_BLUE, COLOR_BLACK);	
    init_pair(TITLE_COLSCH, COLOR_MAGENTA, COLOR_BLACK);	
	
	raw();
    mainloop("char\nnahui\npidoras\nебаный сука блять\n\nИДИ НАХУЙ\n\nПИДОРАСЫ\n\nИДИ НАХУЙ\n\nПИДОРАСЫ", "dir");

	endwin();

	return 0;
}