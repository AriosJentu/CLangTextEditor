#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#define RED_COLOR "\033[1m\033[31m"
#define GREEN_COLOR "\033[1m\033[32m"
#define YELLOW_COLOR "\033[1m\033[33m"
#define BOLD "\033[1m\033[34m"
#define UNDERCOLOR "\033[7m"
#define DEFAULT_COLOR "\033[0m"
#define FILESIZE 10485

int c, LinesCount = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void resultstring(char* output, char str[]){

	int i = 0;
	for (i = 0; str[i]; i++){
		*(output+i) = str[i];
	}
	*(output+i) = '\0';

}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* convertstr(char* input) {

	char* str = malloc(FILESIZE);
	sprintf(str, "%s1: %s", BOLD, DEFAULT_COLOR);
	LinesCount = 1;

	for (int i = 0; input[i]; i++) {

		sprintf(str, "%s%c", str, input[i]);
		
		if (input[i] == '\n') {
			LinesCount++;
			sprintf(str, "%s%s%i: %s", str, BOLD, LinesCount, DEFAULT_COLOR);
		}
	}
	return str;
}

void getlinerange(int* from, int* to, int line, char str[], int adding) {

	int cnt = 0, ldx = 0, rdx = strlen(str);
	for (int i = 0; str[i] && line != LinesCount; i++) {
		
		if (str[i] == '\n') {
			cnt++;

			if (cnt == line) {
				rdx = i;
				break;
			}
		}
	}

	for (int i = rdx-1; str[i] && line != 1; i--) {
		if (str[i] == '\n') {
			ldx = i+adding;
			break;
		}
	}

	*from = ldx;
	*to = rdx;
}

int isdirexist(char* input) {

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

void writechanges(char dir[], char text[]) {
			
	FILE* EditableFile = fopen(dir, "w");
	fprintf(EditableFile, "%s", text);
	fclose(EditableFile);	

}

void editlineloop(char* output, char input[], char reserve[]) {

	printf("\n\n%s%s Исходная строка: \n%s '%s' \n", BOLD, UNDERCOLOR, YELLOW_COLOR, reserve);
	
	if (input != reserve) {
		printf("\n%s Измененная строка: \n%s '%s' \n", RED_COLOR, GREEN_COLOR, input);
	}

	printf("\n%sСписок доступных действий:\n", DEFAULT_COLOR);
	printf("%s%s 1 - Ввести новую строку                             \n", GREEN_COLOR, UNDERCOLOR);
	printf(" 2 - Сохранить изменения и выйти из редактирования   \n");
	printf(" 3 - Выйти из редактирования без сохранения          \n");	
	printf("%sВведите номер действия: ", DEFAULT_COLOR);

	int SelectionID = 0;
	scanf("%d", &SelectionID);

	if (SelectionID >= 1 && SelectionID <= 3) {

		char strs[FILESIZE];

		if (SelectionID == 1) {

			printf("\n%sВведите текст для строки: %s", GREEN_COLOR, DEFAULT_COLOR);
			
			getchar();

			while ((c = getchar()) != '\n') {
				sprintf(strs, "%s%c", strs, c);
			}

			system("clear");

			editlineloop(output, strs, reserve);

		} else if (SelectionID == 2) {

			resultstring(output, input);
		
		} else if (SelectionID == 3) {

			resultstring(output, reserve);

		}

	} else {

		system("clear");
		printf("\n\n%s%s-Число-Некорректно-\n%s", RED_COLOR, UNDERCOLOR, DEFAULT_COLOR);

		editlineloop(output, input, reserve);

	}
}

////////////////////////////////////////////////////////////////////////////////
void addendline(char* input, char* exittxt, char* x) {

	*(input+strlen(input)) = '\n';

	resultstring(exittxt, GREEN_COLOR "\n------Успешно------\n");
}

////////////////////////////////////////////////////////////////////////////////
void addstartline(char* input, char* exittxt, char* x) {

	char str[FILESIZE];
	resultstring(str, input);

	*input = '\n';
	int i = 1;
	for (i = 1; str[i-1]; i++) {
		*(input+i) = str[i-1];
	}
	*(input+i) = '\0';

	resultstring(exittxt, GREEN_COLOR "\n------Успешно------\n");

}

////////////////////////////////////////////////////////////////////////////////
void addlineafterline(char* input, char* exittxt, char* x) {
			
	int line;

	printf("Введите строку: ");
	scanf("%d", &line);

	if (1 <= line && line < LinesCount) {

		int indx = 0, cnt = 0;
		char strs[FILESIZE];
		
		for (int i = 0; input[i]; i++) {

			if (input[i] == '\n') {
				cnt++;
			}
			
			if (cnt == line) {
				indx = i;
				break;
			}
		}

		int j = 0;
		for (int i = 0; input[i]; i++, j++) {

			strs[j] = input[i];
			
			if (i == indx) {
				j++;
				strs[j] = '\n';
			}
		}
		strs[j] = '\0';

		int i = 0;
		for (i = 0; strs[i]; i++) {
			*(input+i) = strs[i];
		}
		*(input+i) = '\0';

		resultstring(exittxt, GREEN_COLOR "\n------Успешно------\n");

	} else {

		resultstring(exittxt, RED_COLOR "\n------Ошибка!------\n");

	}
}

////////////////////////////////////////////////////////////////////////////////
void deleteline(char* input, char* exittxt, char* x) {
	int line;

	printf("Введите строку: ");
	scanf("%d", &line);

	if (1 <= line && line <= LinesCount) {

		int findx, sindx;
		char strs[FILESIZE];

		getlinerange(&findx, &sindx, line, input, 0);

		int j = 0;		
		for (int i = 0; input[i]; i++) {
			
			if (i < findx || (line == 1 ? (i > sindx) : (i >= sindx) ) ){
				strs[j] = input[i];
				j++; 
			}
		}
		strs[j] = '\0';

		resultstring(input, strs);			
		resultstring(exittxt, GREEN_COLOR "\n------Успешно------\n");

	} else {

		resultstring(exittxt, RED_COLOR "\n------Ошибка!------\n");
	
	}
}

////////////////////////////////////////////////////////////////////////////////
void editline(char* input, char* exittxt, char* x) {
	int line;

	printf("Введите строку: ");
	scanf("%d", &line);

	if (1 <= line && line <= LinesCount) {
		
		int findx, sindx;
		char edit[FILESIZE], ending[FILESIZE], reserved[FILESIZE];
		strcpy(reserved, input);

		getlinerange(&findx, &sindx, line, reserved, 1);

		int i = 0;
		for (i = 0; reserved[i+findx] && i < sindx-findx; i++) {
			edit[i] = reserved[i+findx];
		}
		edit[i] = '\0';

		for (i = 0; reserved[i+sindx]; i++) {
			ending[i] = reserved[i+sindx];
		}
		ending[i] = '\0';

		editlineloop(edit, edit, edit);
		
		i = findx;
		int k = 0;

		for (i = findx; edit[i-findx]; i++) {
			*(input+i) = edit[i-findx];
		}

		for (k = 0; ending[k]; k++) {
			*(input+i+k) = ending[k];	
		}
		
		*(input+i+k) = '\0';
		
		resultstring(exittxt, GREEN_COLOR "\n------Успешно------\n");

	} else {

		resultstring(exittxt, RED_COLOR "\n------Ошибка!------\n");

	}
}

void savechanges(char input[], char* exittxt, char dir[]) {

	writechanges(dir, input);
	resultstring(exittxt, GREEN_COLOR "\n---Файл-Сохранен---\n");

}

void savechangesas(char input[], char* exittxt, char* dir) {
	char* newdir = malloc(128);

	printf("Введите новую директорию файла: ");
	scanf("%s", newdir);

	if (isdirexist(newdir)) {

		if (fopen(newdir, "r")) {
			char agreement = 'n';

			printf("%sФайл существует.\n%sЗаменить? (y/n): ", RED_COLOR, DEFAULT_COLOR);
			scanf("\n%c", &agreement);

			if (agreement == 'y'){
		
				resultstring(dir, newdir);

				writechanges(dir, input);
				resultstring(exittxt, GREEN_COLOR "\n---Файл-Сохранен---\n");

			} else {

				resultstring(exittxt, GREEN_COLOR "\n-Действие-Отменено-\n");

			}

		} else {
				
			resultstring(dir, newdir);
			
			writechanges(dir, input);
			resultstring(exittxt, GREEN_COLOR "\n----Файл-Создан----\n");

		}

	} else {
		
		resultstring(exittxt, RED_COLOR "\n-Ошибка-Директории-\n");

	}

	free(newdir);
}

void (*actions[7])(char*, char*, char*) = {
	addendline,
	addstartline,
	addlineafterline,
	deleteline,
	editline,
	savechanges,
	savechangesas
};

void mainloop(char input[], char dir[]) {

	int SelectionID = 0;

	printf("%s%s Используемый файл:%s %s \n\n", BOLD, UNDERCOLOR, YELLOW_COLOR, dir);
	printf("%sСписок доступных действий:\n", DEFAULT_COLOR);
	printf("%s%s 1 - Добавить пустую строку в конец                  \n", GREEN_COLOR, UNDERCOLOR);
	printf(" 2 - Добавить пустую строку в начало                 \n");
	printf(" 3 - Добавить пустую строку под определенной строкой \n");
	printf(" 4 - Вырезать строку                                 \n");	
	printf(" 5 - Редактировать строку                            \n");
	printf(" 6 - Сохранить изменения                             \n");
	printf(" 7 - Сохранить как                                   \n");
	printf(" 8 - Сохранить изменения и выйти                     \n");
	printf(" 9 - Выйти без сохранения                            \n");
	printf("%sВведите номер действия: ", DEFAULT_COLOR);

	scanf("%d", &SelectionID);

	if (SelectionID >= 1 && SelectionID <= 9) {

		char str[19] = "";
		int exiting = 0;

		if (1 <= SelectionID && SelectionID <= 7) {

			(*actions[SelectionID-1])(input, str, dir);
		
		} else if (SelectionID == 8) {
			
			savechanges(input, str, dir);
			exiting = 1;

		} else if (SelectionID == 9) {
			
			exiting = 1;
		
		}

		if (!exiting) {
			system("clear");
		}
		
		printf("%s", str);
		if (exiting) {
			printf("\n");
			exit(0);
		}

	} else {

		system("clear");
		printf("\n%s%s-Число-Некорректно-\n%s", RED_COLOR, UNDERCOLOR, DEFAULT_COLOR);
	
	}
	
	printf("\n%s----Содержимое:----\n%s", RED_COLOR, DEFAULT_COLOR);
	printf("%s\n%s-------Конец-------\n\n", convertstr(input), RED_COLOR);
	mainloop(input, dir);
}

int main(int argc, char* args[]) {
	
	if (args[1]) {
			
		system("clear");
			
		printf("\n%s--Редактор-Текста--\n", GREEN_COLOR);

		if (fopen(args[1], "r")) {

			printf("\nФайл: '%s'\n", args[1]);

		} else {

			fclose(fopen(args[1], "w+"));
			printf("\nФайла не существует\nФайл (Создание): '%s'\n", args[1]);

		}

		FILE* ExportFile;

		if ((ExportFile = fopen(args[1], "r+"))) {

			char* input = malloc(FILESIZE);
			fread(input, FILESIZE, 1, ExportFile);
			fclose(ExportFile);

			printf("\n%s----Содержимое:----\n%s", RED_COLOR, DEFAULT_COLOR);
			printf("%s\n%s-------Конец-------\n\n", convertstr(input), RED_COLOR);
			
			mainloop(input, args[1]);
			
			free(input);
		
		} else {

			printf("%s%sНе удалось создать файл\n", BOLD, RED_COLOR);	

		}

	} else {

		printf("Не выбран файл\n");
	}

	return 0;
}