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
int ReservedLines = 0;
int FilePosX = 0, FilePosY = 0;


int SpacesCount = 1;
int TitleSize = 6;

void mainloop(char x[], char y[], char z[], int f);
int mainmenu(char x[], char y[]);

/////////////////////////////////////////////////////////////////
void movecur(int y, int x) {
	move(y, x);
	FilePosX = x;
	FilePosY = y;
}

/////////////////////////////////////////////////////////////////
int getlinelen(char input[], int line) {

	char reserved[FILESIZE];
	int linelen = SpacesCount+3, i = 0, j = 0;
	ReservedLines = 0;

	while (input[i]) {

		linelen++;
		
		if (linelen >= getmaxx(stdscr) && !(input[i] == '\n')) {
			linelen = SpacesCount+3;
			reserved[j++] = '\n';
			ReservedLines++;	
		}	

		linelen = input[i] == '\n' ? SpacesCount+3 : linelen;
		reserved[j++] = input[i++];
	}
	i = 0;
	reserved[j] = '\0';

	int thisline = 0;
	int result = 0;

	while (reserved[i]) {

		if (thisline == line && reserved[i] != '\n') {
			result++;
		}

		if (reserved[i++] == '\n') {

			thisline++;
			if (thisline > line) {
				break;
			}

		} 
	}

	return result;
}

/////////////////////////////////////////////////////////////////

int resrvdbefore(char input[], int line) {//Количество резервных строк (переносок) до текущей

	int cnt = 0;

	int i = 0;
	int thisline = 0;
	int linelen = SpacesCount+3;

	while (input[i] && thisline < line) {
		
		linelen++;
		if (linelen > getmaxx(stdscr) && !(input[i] == '\n')) {\
			linelen = SpacesCount+3;
			thisline++;			
			cnt++;
		}
		if (input[i++] == '\n') {
			thisline++;
			linelen = SpacesCount+3;
		}
	}

	return cnt;

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
	printw("\n %s\n", message);
	attron(COLOR_PAIR(DEFAULT_COLSCH));
	
	attron(COLOR_PAIR(LINES_COLSCH));
	for (int i = 0; i < getmaxx(stdscr); i++){
		printw("_");
	}
	printw("\n");
	attroff(A_BOLD);	

	refresh();
}

void printline(int indx, int strcount, int isreserver) {
	//Хуйня, которая выводит номера строк

	attron(COLOR_PAIR( isreserver == 0 ? LINES_COLSCH : YELLOW_COLSCH));
	attron(A_BOLD);	
	printw(" %d%s", indx, isreserver == 0 ? ")" : ":");

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
		LinesCount += input[i++] == '\n' ? 1 : 0;
	}

	char c[5];
	sprintf(c, "%d", LinesCount);
	SpacesCount = strlen(c);
	ReservedLines = 0;

	printline(nowline, SpacesCount, 0);

	i = 0;
	int linelen = SpacesCount+3;
	while (input[i]) {

		linelen++;
		
		if (linelen >= getmaxx(stdscr) && !(input[i] == '\n')) {
			linelen = SpacesCount+3;
			ReservedLines++;
			printw("\n");
			printline(nowline, SpacesCount, 1);
		}

		printw("%c", input[i]);
		if (input[i++] == '\n') {

			nowline++;
			linelen = SpacesCount+3;

			printline(nowline, SpacesCount, 0);

		}
	}
}

void createfile(char* dir, char* input) {

	char txt[4] = "*new";
	int x;
	for (x = 0; txt[x]; x++) {
		*(dir+x) = txt[x];
	}
	*(dir+x) = '\0';
	*(input+0) = '\0';

}

int openfile(char* dir, char* input) {

	int posX = 20, posY = TitleSize+9;
	attron(A_BOLD);
	attron(COLOR_PAIR(YELLOW_COLSCH));
	mvprintw(posY, 0, " Введите имя файла: ");
	mvprintw(posY+1, 0, " Для выхода нажмите ESC (и подождите)");
	attroff(A_BOLD);
	attron(COLOR_PAIR(DEFAULT_COLSCH));
	refresh();

	char lst[50] = "";

	int x;
	for (x = 0; dir[x]; x++) {
		*(dir+x) = dir[x];
	}
	*(dir+x) = '\0';
	for (x = 0; input[x]; x++) {
		*(input+x) = input[x];
	}
	*(input+x) = '\0';

	int chr = getch();
	while (chr != '\n') {
		if ( (chr >= 32 && chr <= 126) ) {

			mvprintw(posY, posX, "%c", chr);
			posX++;
			sprintf(lst, "%s%c", lst, chr);

		} else if (chr == KEY_BACKSPACE && posX > 20) { 

			posX--;
			mvprintw(posY, posX, " ");		
			int i;
			lst[strlen(lst)-1] = '\0';

		} else if ( chr == 27  ) {

			return 0;
		
		}
		chr = getch();
	}

	if (strlen(lst) <= 1 || lst[0] == ' ') {
		return 13;
	}		

	FILE* InputFile;
	char Content[FILESIZE];
	if (InputFile = fopen(lst, "r")) {

		fread(Content, FILESIZE, 1, InputFile);
		fclose(InputFile);

		int k = 0;
		for (k = 0; Content[k]; k++) {
			 *(input+k) = Content[k];	
		}
		*(input+k) = '\0';
		
		for (k = 0; lst[k]; k++) {
			*(dir+k) = lst[k];
		}
		*(dir+k) = '\0';

		return 12;
		
	} else {	
			
		return 13;

	}
}

int savefileas(char* dir, char input[]) {

	int posX = 20, posY = TitleSize+9;
	attron(A_BOLD);
	attron(COLOR_PAIR(YELLOW_COLSCH));
	mvprintw(posY, 0, " Введите имя файла: ");
	mvprintw(posY+1, 0, " Для выхода нажмите ESC (и подождите)");
	attroff(A_BOLD);
	attron(COLOR_PAIR(DEFAULT_COLSCH));
	refresh();

	char lst[50] = "";

	int chr = getch();

	int x;
	for (x = 0; dir[x]; x++) {
		*(dir+x) = dir[x];
	}
	*(dir+x) = '\0';

	while (chr != '\n') {
		if ( (chr >= 32 && chr <= 126) ) {

			mvprintw(posY, posX, "%c", chr);
			posX++;
			sprintf(lst, "%s%c", lst, chr);

		} else if (chr == KEY_BACKSPACE && posX > 20) { 

			posX--;
			mvprintw(posY, posX, " ");		
			int i;
			lst[strlen(lst)-1] = '\0';

		} else if ( chr == 27  ) {

			return 0;
		
		}
		chr = getch();
	}

	if (strlen(lst) <= 1 || lst[0] == ' ') {
		return 13;
	}

	int noext = 1;
	for (x = 0; lst[x]; x++) {
		*(dir+x) = lst[x];
		noext = (lst[x] == '.' && x < strlen(lst)-1) ? 0 : noext;
	}
	if (noext) {
		*(dir+(x++)) = '.';
		*(dir+(x++)) = 't';
		*(dir+(x++)) = 'x';
		*(dir+(x++)) = 't';
	}
	*(dir+x) = '\0';
	
	FILE* OutputFile;
	if (OutputFile = fopen(dir, "w")) {

		fprintf(OutputFile, "%s", input);
		fclose(OutputFile);
		
		return 12;
		
	} else {	
			
		return 13;

	}
}

int savefile(char* dir, char input[]) {

	if (fopen(dir, "r") && strcmp(dir, "*new") != 0) {
		
		FILE* OutputFile;
		if (OutputFile = fopen(dir, "w")) {
		
			fprintf(OutputFile, "%s", input);
			fclose(OutputFile);
			return 12;

		} else {	
			
			return 13;
		}

	} else {

		return savefileas(dir, input);

	}
}

int mainmenu(char input[], char dir[]) {
	//Менюшка 

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

		} else if (chr == '1') {

			return 4;

		} else if (chr == '2') {

			return 3;

		} else if (chr == '3') {

			return 2;

		} else if (chr == '4') {

			return 1;

		} else if (chr == KEY_F(2) || chr == ItemsCount+'0') {			
			return 0;

		} else if (chr == KEY_UP) {

			if (SelectedItem > ItemsCount){

				attron(COLOR_PAIR(DEFAULT_COLSCH));

				mvprintw(SelectedItem, 1, " ");

				SelectedItem -= SelectedItem == ItemsCount+TitleSize ? 1 : 0;
				mvprintw(--SelectedItem, 1, ">");
			
			}

		} else if (chr == KEY_DOWN) {

			if (SelectedItem < ItemsCount+TitleSize){

				attron(COLOR_PAIR(DEFAULT_COLSCH));
				mvprintw(SelectedItem, 1, " ");

				SelectedItem += SelectedItem == 5+TitleSize ? 1 : 0;
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

void insertchar(char* text, char input, int position) {

	for (int i = strlen(text); i >= position; i--) {
		*(text+(i+1)) = *(text+i);
	}
	*(text+position) = input;

}

void delchar(char* text, int position) {

	for (int i = position-1; text[i]; i++) {
		*(text+i) = *(text+i+1);
	}
}


void mainloop(char input[], char dir[], char reason[], int col) {
	//Главный цикл, где просчитывается всякая хуйня

	clear();
	rendertitle(dir, reason, col);
	rendercontent(input);

	int MinX = SpacesCount+3, MinY = 7, MaxY = 6+LinesCount+ReservedLines;
	int x, y;

	FilePosY = FilePosY == 0 ? MinY : FilePosY;
	FilePosX = FilePosX == 0 ? MinX : FilePosX;

	refresh();
	
	noecho();
	curs_set(1);	

	keypad(stdscr, 1);

	move(FilePosY, FilePosX);

	int chr = getch();

	char exittext[50] = "Текстовый редактор";
	int exitcol = LINES_COLSCH;

	while (chr) {

		MinX = SpacesCount+3; 
		MinY = 7; 
		MaxY = 6+LinesCount+ReservedLines;

		getyx(stdscr, y, x);
        if (chr == KEY_RESIZE) {
        	clear();
			rendertitle(dir, exittext, exitcol);
			rendercontent(input);
			refresh();
        }

        y = (y > MaxY) ? MaxY : y;

		refresh();

		if (chr == KEY_F(2)) {

			int z = mainmenu(input, dir);
			
			if (z == 42) { //Просто аргумент выхода из программы
				return;

			} else if (z == 0) {
				break;

			} else if (z == 1) {

				int sv = savefileas(dir, input);
				if (sv == 12) {

					sprintf(exittext, "Файл сохранен");
					exitcol = SUCCESS_COLSCH;
				
				} else if (sv == 13) {

					sprintf(exittext, "Не удалось сохранить файл");
					exitcol = ERROR_COLSCH;

				}
				break;

			} else if (z == 2) {

				int sv = savefile(dir, input);
				if (sv == 12) {

					sprintf(exittext, "Файл сохранен");
					exitcol = SUCCESS_COLSCH;
				
				} else if (sv == 13) {

					sprintf(exittext, "Не удалось сохранить файл");
					exitcol = ERROR_COLSCH;

				}
				break;

			} else if (z == 3) {

				int op = openfile(dir, input);
				if (op == 12) {

					sprintf(exittext, "Файл открыт");
					exitcol = SUCCESS_COLSCH;
				
				} else if (op == 13) {

					sprintf(exittext, "Не удалось открыть файл");
					exitcol = ERROR_COLSCH;

				}
				movecur(MinY, MinX);
				break;

			} else if (z == 4) {

				createfile(dir, input);
				movecur(MinY, MinX);
				break;

			}

		} else if (chr == KEY_UP) {

			y = (y > MinY) ? y-1 : y;
			x = (x > MinX+getlinelen(input, y-(TitleSize+1))) ? MinX+getlinelen(input, y-(TitleSize+1)) : x;

		} else if (chr == KEY_DOWN) {

			y = (y < MinY) ? y+1 : y;
			x = (x > MinX+getlinelen(input, y-(TitleSize+1))) ? MinX+getlinelen(input, y-(TitleSize+1)) : x;

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

		} else if ( (chr >= 32 && chr <= 126) || chr == KEY_BACKSPACE || chr == KEY_DC || chr == '\n') {
			
			int pos = 0;
			for (int val = 0; val < y-(TitleSize+1); val++) {
				pos += getlinelen(input, val)+1;
			}
			pos += (x-MinX)-(resrvdbefore(input, y-TitleSize-1));

			if (chr == KEY_BACKSPACE) {

				if (pos > 0) {
					if(x == MinX && y > MinY) {
					
						y--;
						x = MinX+getlinelen(input, y-(TitleSize+1));
					
					} else {

						x--;
					}
					movecur(y, x);
					LinesCount -= inch() == '\n' ? 1 : 0;

					delchar(input, pos);
				}

			} else if (chr == KEY_DC) {

				LinesCount -= inch() == '\n' ? 1 : 0;

				delchar(input, pos+1);

			} else {

				insertchar(input, chr, pos);
				if (chr == '\n') {
					y++;
					x = MinX;
					LinesCount++;
				} else {
					x++;
				}

			}

			clear();
			rendertitle(dir, exittext, exitcol);
			rendercontent(input);
			refresh();
			move(FilePosY, FilePosX);
		} 

		movecur(y, x);
		chr = getch();
	}

	keypad(stdscr, 0);

	mainloop(input, dir, exittext, exitcol);
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

	scrollok(stdscr, 1);

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

    } else {

    	if (fopen(dir, "w")) {
    		remove(dir);
    	} else {
    		strcpy(dir, "*new");
    	}
    }
    
    mainloop(input, dir, "Текстовый редактор", LINES_COLSCH);

	endwin();

	return 0;
}