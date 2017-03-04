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
int FilePosX = 0, FilePosY = 0;

int SpacesCount = 1;
int TitleSize = 6;

void mainloop(char x[], char y[]);
int getlinelen(char input[], int line);

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
	printw(" Используемый файл%s: ", fopen(filename, "r") ? "" : " (Создать)");

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

int mainmenu(char input[], char dir[]) {
	//Менюшка 
	//TODO - прикрутить ебливый скроллер на стрелочки, было бы заебись

	clear();
	rendertitle(dir, "Меню", YELLOW_COLSCH);

	attron(A_BOLD);	

	attron(COLOR_PAIR(DEFAULT_COLSCH));
	printw(" > ");
	attron(COLOR_PAIR(SUCCESS_COLSCH));
	printw("1 - Создать\n");

	attron(COLOR_PAIR(DEFAULT_COLSCH));
	printw("   ");
	attron(COLOR_PAIR(SUCCESS_COLSCH));
	printw("2 - Открыть\n");

	attron(COLOR_PAIR(DEFAULT_COLSCH));
	printw("   ");
	attron(COLOR_PAIR(SUCCESS_COLSCH));
	printw("3 - Сохранить\n");

	attron(COLOR_PAIR(DEFAULT_COLSCH));
	printw("   ");
	attron(COLOR_PAIR(SUCCESS_COLSCH));
	printw("4 - Сохранить как\n");

	attron(COLOR_PAIR(DEFAULT_COLSCH));
	printw("   ");
	attron(COLOR_PAIR(SUCCESS_COLSCH));
	printw("5 - Выйти\n\n");

	attron(COLOR_PAIR(DEFAULT_COLSCH));
	printw("   ");
	attron(COLOR_PAIR(SUCCESS_COLSCH));
	printw("F2 - Вернуться\n");
	refresh();

	keypad(stdscr, 1);

	curs_set(0);
	int chr = getch();

	int ItemsCount = 7;

	int SelectedItem = ItemsCount;
	while (chr) {

		if (chr == '5') {			
			return 42;

		} else if (chr == KEY_F(2) || chr == ItemsCount+'0') {			
			return 0;

		} else if (chr == KEY_UP) {

			if (SelectedItem > ItemsCount){

				attron(COLOR_PAIR(DEFAULT_COLSCH));

				mvprintw(SelectedItem, 1, " ");

				if (SelectedItem == ItemsCount+TitleSize) {
					SelectedItem--;
				}
				mvprintw(--SelectedItem, 1, ">");
			
			}

		} else if (chr == KEY_DOWN) {

			if (SelectedItem < ItemsCount+TitleSize){

				attron(COLOR_PAIR(DEFAULT_COLSCH));
				mvprintw(SelectedItem, 1, " ");
				if (SelectedItem == 5+TitleSize) {
					SelectedItem++;
				}
				mvprintw(++SelectedItem, 1, ">");
			
			}

		} else if (chr == KEY_RIGHT || chr == '\n') {

			chr = (SelectedItem-TitleSize)+'0';
			continue;

		}


		chr = getch();
	}

	keypad(stdscr, 0);

}

void movecur(int y, int x) {
	move(y, x);
	FilePosX = x;
	FilePosY = y;
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

	int MinX = SpacesCount+3, MinY = 7, MaxY = 6+LinesCount;
	int x, y;

	if (FilePosY == 0){
		FilePosY = MinY;
	}
	if (FilePosX == 0){
		FilePosX = MinX;
	}

	move(FilePosY, FilePosX);

	int chr = getch();
	while (chr) {

		getyx(stdscr, y, x);

		if (chr == KEY_F(2)) {

			int z = mainmenu(input, dir);
			
			if (z == 42) { //Просто аргумент выхода из программы
				return;

			} else if (z == 0) {
				break;
			}

		} else if (chr == KEY_UP) {

			if (y > MinY){
				y--;
			}

			if (x > MinX+getlinelen(input, y-(TitleSize+1) )) {
				x = MinX+getlinelen(input, y-(TitleSize+1)); 
			}

		} else if (chr == KEY_DOWN) {

			if (y < MaxY) {
				y++;
			}
			if (x > MinX+getlinelen(input, y-(TitleSize+1) )) {
				x = MinX+getlinelen(input, y-(TitleSize+1)); 
			}

		} else if (chr == KEY_RIGHT) {

			if (x < MinX+getlinelen(input, y-(TitleSize+1) )) {

				x++;
			
			} else if (x == MinX+getlinelen(input, y-(TitleSize+1) ) && y < MaxY) {

				y++;
				x = MinX;
			
			}

		} else if (chr == KEY_LEFT) {
			
			if (x > MinX) {

				x--;

			} else if(x == MinX && y > MinY) {

				y--;
				x = MinX+getlinelen(input, y-(TitleSize+1));

			}

		}

		//mvprintw(3, 0, "Char: '%c'", inch());
		
		movecur(y, x);

		chr = getch();
	}

	keypad(stdscr, 0);

	mainloop(input, dir);
}

int main(int argc, char* args[]) {
	//Главная часть - тут крч все настройки и вызов функции цикла

	char input[FILESIZE];
	char dir[144] = "*new";
	if (argc > 1) {
		strcpy(dir, args[1]);
	}

	strcpy(input, "\0");

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

	if (fopen(dir, "r")) {

		fread(input, FILESIZE, 1, fopen(dir, "r"));

    }
    
    mainloop(input, dir);

	endwin();

	return 0;
}
int getlinelen(char input[], int line) {

	setlocale(LC_CTYPE, "C-UTF-8");

	int thisline = 0;

	int i = 0;
	int result = 0;
	//mvprintw(0, 0, "'");
	while (input[i]) {

		//mvprintw(0, result+1, "%c", input[i]);
		if (thisline == line && input[i] != '\n') {
			result++;
		}


		if (input[i] == '\n') {

			thisline++;
			if (thisline > line) {
				break;
			}

		} 
		i++;
		
	}
	//mvprintw(0, result+2, "' '%i'", result);

	return result;
}