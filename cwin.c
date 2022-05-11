/*
Игра с графическим интерфейсом - Wordle.
Автор - Попков Денис, 21м
*/

// Добавление библиотек и зависимостей
#include <stdio.h> // Библиотека ввода и вывода
#include <stdlib.h> // Содержит в себе функции, занимающиеся выделением памяти
#include <string.h> // Работа с строками
#include <locale.h> // Локализация
#include <time.h> // Работа с датой и временем
#include <windows.h> // Работа с системой Windows
#include <wchar.h> // Хранения широких символов
#include "conio.h" // Создание текстового интерфейса в консоли

// Слово для проверки работы программы
#define dummyWord "ERASE\0"
// Число столбцов
#define MAX_TEXTBOX 6
// Разделитель для слов, их всего может быть 5
#define SEPARATOR 0x0A
// Словарь всех слов
#define DICTIONARY "dict.txt"
// Словарь возможных слов, которые может ввести игрок
#define POSSIBLES "possible.txt"

// Значения Юникод символов для создания игрового поля
#define HOR_LINE 196
#define VER_LINE 179
#define UPPER_LEFT_CORNER 218
#define LOWER_LEFT_CORNER 192
#define UPPER_RIGHT_CORNER 191
#define LOWER_RIGHT_CORNER 217

// Символы для определения какие символы были нажаты с клавиатуры
#define K_BACKSPACE 8
#define K_ENTER 13
#define K_ESCAPE 27
#define K_ENTER2 10

// Цвета заднего фона
#define B_BLACK 0
#define B_GREEN 37
#define B_YELLOW 110
#define B_WHITE 128

// Цвета переднего фона
#define F_BLACK 0
#define F_RED 12
#define F_GREEN 10
#define F_YELLOW 14
#define F_BLUE 9
#define F_MAGENTA 12
#define F_CYAN 13
#define F_WHITE 15
#define F_GREY 8

// Значения для размера поля
#define BOARDSIZEY 21
#define BOARDSIZEX 35

// Игровое поле
char boardInputs[7][6];
char secretWord[6];
char textbox1[MAX_TEXTBOX];
int  repeatedLetters[5] = {1, 1, 1, 1, 1};
char textbox1[MAX_TEXTBOX];
int checkTrue[5] = {0, 0, 0, 0, 0};

// Структуры и поля для файлов, позиции на поле слов и букв
static int peekCharacter = -1;
int rows = 0, columns = 0;
int wherey = 0, wherex = 0;
int oldy = 0, oldx = 0;
int currentIndex = 0;
int okFile, okFile2;
int dictionaryPresent = 0;

// Указатели на словари
FILE *fileSource;
FILE *fileSource2;
time_t t;
unsigned randomWord = 0;
unsigned words = 0;
unsigned words2 = 0;

// Описаны функции для работы с терминалом
// Контроль курсора, позиции
int kbhit();
TCHAR readch();
void resetch();
void gotoxy(int x, int y);
void outputcolor(int foreground, int background);
int getTerminalDimensions(int *rows, int *columns);
void setCursor(int isVisible);
void cls();

// Описаны функции для чтения, записи слов в поле
void writeStr(int wherex, int wherey, char *str, int backcolor, int forecolor);
char textbox(int wherex, int wherey, int displayLength,char label[MAX_TEXTBOX], char text[MAX_TEXTBOX], 
int backcolor, int labelcolor, int textcolor);
void writeCh(int wherex, int wherey, wchar_t  ch, int backcolor, int forecolor);
int writeNum(int x, int y, int num, char backcolor, char forecolor);
void window(int x1, int y1, int x2, int y2, int backcolor, int bordercolor, int titlecolor, int border, int title);

// Описаны функции для прорисовки игрового поля
void toUpper(char *text);
void newGame();
void gameLoop();
void displayHelp();
void drawBoard();
void credits();
int checkGreen(char c, int index, char *str);
int checkOrange(char c, int index, char *str);
void checkRepeatedLetters();
void writeWord(int index,  char text[MAX_TEXTBOX]);
void cleanArea();
int findIndex(char c);

// Работы с размером словаря
int openFile(FILE ** fileHandler, char *fileName, char *mode);
long countWords(FILE * fileHandler);
void getWordfromDictionary(FILE * fileHandler, char WORD[MAX_TEXTBOX]);
int isWordinDictionary(FILE * fileHandler, char WORD[MAX_TEXTBOX]);
int closeFile(FILE * fileHandler);

// Скрывает или показывает курсор на игровом поле
void setCursor(int isVisible) {
   HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 100;
   info.bVisible = isVisible;
   SetConsoleCursorInfo(consoleHandle, &info);
}

// Стирает информацию с игровго поля
void cls() {
  system("cls");
}

// Обнаруживает если была нажата клавиша на клавиатуре
int kbhit() {
   return _kbhit();
}

// Считывает одну букву
TCHAR readch() {
  return _getch();
}

// Двигает курсор к определенной позиции
void gotoxy(int x, int y) {
  COORD coord;
  coord.X = x;
  coord.Y = y;
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Изменяет задний фон
void outputcolor(int foreground, int background) {
  HANDLE h = GetStdHandle (STD_OUTPUT_HANDLE);
  WORD wOldColorAttrs;
  CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
  GetConsoleScreenBufferInfo(h, &csbiInfo);
  wOldColorAttrs = csbiInfo.wAttributes;
  // Устанавливает текущий цвет
  SetConsoleTextAttribute ( h, foreground | background );
}

// Изменяет весь цвет в программе
void screencol(int x) {
  HANDLE hStdOut;
  HANDLE h = GetStdHandle ( STD_OUTPUT_HANDLE );
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  DWORD count;
  DWORD cellCount;
  COORD homeCoords = {0, 0};
  hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hStdOut == INVALID_HANDLE_VALUE) return;
  if (!GetConsoleScreenBufferInfo( hStdOut, &csbi )) return;
  cellCount = csbi.dwSize.X *csbi.dwSize.Y;
  SetConsoleTextAttribute ( h, x | x );
  if (!FillConsoleOutputCharacter(hStdOut, (TCHAR) ' ', cellCount, homeCoords, &count)) return;
  if (!FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, cellCount, homeCoords, &count)) return;
  SetConsoleCursorPosition( hStdOut, homeCoords );
}

// Получает размер терминала
int getTerminalDimensions(int *rows, int *columns) {
  HANDLE hStdOut;
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  DWORD count;
  DWORD cellCount;
  COORD homeCoords = {80, 26};
  hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );
  if (hStdOut == INVALID_HANDLE_VALUE) return;
  SetConsoleScreenBufferSize(hStdOut,  homeCoords);
  *columns = 80;
  *rows = 25;
  return 0;
}

// Пишет слово в терминал
void writeStr(int wherex, int wherey, char *str, int backcolor, int forecolor){
  gotoxy(wherex,wherey);
  outputcolor(backcolor,forecolor);
  printf("%s", str);
}

// Пишет букву в терминал
void writeCh(int wherex, int wherey, wchar_t ch, int backcolor, int forecolor){
  // Записывает Юникод символ в терминал
  gotoxy(wherex,wherey);
  outputcolor(backcolor,forecolor);
  printf("%lc", ch);
}

// Записывает число в терминал
int writeNum(int x, int y, int num, char backcolor, char forecolor) {
  // Длина строки должна быть передана функции
  char astr[30];
  char len = 0;
  sprintf(astr, "%d", num);
  writeStr(x, y, astr, backcolor, forecolor);
  len = strlen(astr);
  return len;
}

char textbox(int wherex, int wherey, int displayLength,
	char label[MAX_TEXTBOX], char text[MAX_TEXTBOX], int backcolor,
	int labelcolor, int textcolor) {
  int charCount = 0;
  int exitFlag = 0;
  int cursorON = 1;
  long cursorCount = 0;
  int i;
  int limitCursor = 0;
  int positionx = 0;
  int posCursor = 0;
  int keypressed = 0;
  char displayChar;
  char ch;
  strcpy(text, "");
  text[0]= '\0';
  positionx = wherex + strlen(label);
  limitCursor = wherex+strlen(label) + displayLength + 1;
  writeStr(wherex, wherey, label, backcolor, labelcolor);

  writeCh(positionx, wherey, '[', backcolor, textcolor);
  for(i = positionx + 1; i <= positionx + displayLength; i++) {
    writeCh(i, wherey, '.', backcolor, textcolor);
  }
  writeCh(positionx + displayLength + 1, wherey, ']', backcolor,
	   textcolor);
  // Убирает все с клавиатуры
  if(kbhit() == 1) ch = readch();
  ch = 0;

  do {
      keypressed = kbhit();
    // Анимация курсора
   if (keypressed == 0){

   cursorCount++;
    if(cursorCount == 100) {
      cursorCount = 0;
      switch (cursorON) {
	case 1:
	  posCursor = positionx + 1;
          displayChar = '.';
          if (posCursor == limitCursor) {
            posCursor = posCursor - 1;
            displayChar = ch;
          }
          writeCh(posCursor, wherey, displayChar, backcolor, textcolor);
          cursorON = 0;
	  break;
	case 0:
          posCursor = positionx + 1;
          if (posCursor == limitCursor) posCursor = posCursor - 1;
	  writeCh(posCursor, wherey, '|', backcolor, textcolor);
          cursorON = 1;
	  break;
      }
     }
    }
    if(keypressed == 1) {
      ch = readch();
      keypressed = 0;

      if(charCount < displayLength) {
	if(ch > 31 && ch < 127) {
	  writeCh(positionx + 1, wherey, ch, backcolor, textcolor);
	  text[charCount] = ch;
	  positionx++;
	  charCount++;
	}
      }
    }

    if (ch == K_BACKSPACE){
      if (positionx > 0 && charCount > 0){
       positionx--;
       charCount--;
       writeCh(positionx + 1, wherey, '.', backcolor, textcolor);
       if (positionx < limitCursor - 2) writeCh(positionx + 2, wherey, '.', backcolor, textcolor);
       ch=0;
      }
    }
    if(ch == K_ENTER || ch == K_ESCAPE)
      exitFlag = 1;

    // ENTER или ESC чтобы закончить игру
  } while(exitFlag != 1);
  // Очищает курсор
  writeCh(posCursor, wherey, ' ', backcolor, textcolor);
  return ch;
}

void window(int x1, int y1, int x2, int y2, int backcolor, int bordercolor, int titlecolor, int border, int title) {
  int i, j;
  i = x1;
  j = y1;
  // Определяет окончание ячеек
  if (border == 1) {
    for (i = x1; i <= x2; i++) {
      // Нижнии и верхнии окончания ячеек
      writeCh(i, y1, HOR_LINE, backcolor, bordercolor); // Горизонтальный разделитель ячеек
      writeCh(i, y2, HOR_LINE, backcolor, bordercolor);
    }
    for(j = y1; j <= y2; j++) {
      // Левые и правые окончания ячеек
      writeCh(x1, j, VER_LINE, backcolor, bordercolor); // Вертикальный разделитель ячеек
      writeCh(x2, j, VER_LINE, backcolor, bordercolor);
    }
    writeCh(x1, y1, UPPER_LEFT_CORNER, backcolor, bordercolor);
    writeCh(x1, y2, LOWER_LEFT_CORNER, backcolor, bordercolor);
    writeCh(x2, y1, UPPER_RIGHT_CORNER, backcolor, bordercolor);
    writeCh(x2, y2, LOWER_RIGHT_CORNER, backcolor, bordercolor);
  }
  if (title == 1) {
    for(i = x1; i <= x2; i++)
      writeCh(i, y1 - 1, ' ', titlecolor, titlecolor);
  }
}

void toUpper(char *text){
// Делает буквы заглавными
size_t i = 0;
 for (i = 0; i < strlen(text); i++){
    if (text[i] >= 97 && text[i] <= 122) text[i] = text[i] - 32;
  }
}

// Рисует игровое поле
void drawBoard(){
 int i=0,  j=0, shiftx=0, shifty =0;
 int sizeX = 4;
 int sizeY = 2;
  cleanArea();
  shiftx = wherex+4;
  shifty = wherey;
  writeStr(wherex,wherey,"[C WORDLE FOR TERMINAL]", B_BLACK, F_WHITE);
  writeStr(wherex+26,wherey,"Popkov Denis, 21m", B_BLACK, F_GREY);
  writeCh(wherex+4, wherey+2, 'a', B_BLACK, F_WHITE);

  for (j=0; j<6; j++){
  for (i = 0; i<5; i++){
      window(shiftx,shifty+1, (shiftx+sizeX)  , shifty + 1  + sizeY, B_BLACK, F_WHITE, F_WHITE ,1,0);
      shiftx = shiftx + sizeX+1;
   }
      shifty = shifty + sizeY+1;
      shiftx = wherex+4;
  }
 wherey = shifty;

 writeStr(wherex, wherey + 1, "[ESC: EXIT | TYPE <help> for more", B_BLACK, F_GREY);
}

void cleanArea()
{
   int i=0,j=0;
   for (j=0; j<BOARDSIZEY; j++){
     for (i=0; i<50; i++){
       writeCh(oldx+i,oldy+j,' ', B_BLACK, F_BLACK);
     }
   }
}

void displayHelp(){
  char ch=0;
   cleanArea();
   writeStr(1,oldy,"C-WORDLE", B_WHITE, F_BLACK);
   writeStr(1,oldy+1,"Guess a 5-letter secret word in 6 tries.", B_BLACK, F_WHITE);
   writeStr(1,oldy+2,"Type any word to start.", B_BLACK, F_WHITE);
   writeStr(1,oldy+4,"[C]", B_GREEN, F_WHITE);
   writeStr(4,oldy+4,"-> LETTER IS IN THE RIGHT POSITION", B_BLACK, F_WHITE);
   writeStr(1,oldy+5,"[C]", B_YELLOW, F_WHITE);
   writeStr(4,oldy+5,"-> LETTER IS IN THE WRONG POSITION", B_BLACK, F_WHITE);
   writeStr(1,oldy+6,"[C]", B_BLACK, F_WHITE);
   writeStr(4,oldy+6,"-> LETTER IS NOT IN THE WORD", B_BLACK, F_WHITE);
   writeStr(1,oldy+8,"Type <exit> or <quit> to exit.", B_BLACK, F_WHITE);
   writeStr(1,oldy+9,"Type <cheat> to give up.", B_BLACK, F_WHITE);
   writeStr(1,oldy+10,"Total words: ", B_BLACK, F_GREY);
   writeNum(16,oldy+10,words, B_BLACK, F_GREY);
   writeStr(1,oldy+11,"Possible words: ", B_BLACK, F_GREY);
   writeNum(18,oldy+11,words2, B_BLACK, F_GREY);
   writeStr(1,oldy+13, "Press <ENTER> or <ESC> key to return.", B_BLACK, F_WHITE);
   wherex=oldx;
   wherey=oldy;
   do{
      gotoxy(1,1);
      printf("\n");
      if (kbhit()) ch = readch();
      if (ch == K_ESCAPE) break;
   } while (ch != K_ENTER);
   cleanArea();
}

int findIndex(char c)
{
  int i=0; char ch=0;
  do{
     ch=secretWord[i];
     if (c==ch) break;
     i++;
  } while(i<5);
 return i;
}

int checkGreen(char c, int index, char *str){
char ch=0;
size_t i=0;
size_t lindex=index;
int col=B_BLACK; int letterIndex=0;
  letterIndex= findIndex(c);
   for (i=0; i<strlen(str); i++){
     ch = str[i];
     if (c == ch && lindex==i) {
        col = B_GREEN;
        checkTrue[i] = 1;
        repeatedLetters[letterIndex]--;
     }
   }
 return col;
}

int checkOrange(char c, int index, char *str){
char ch=0;
size_t i=0;
int col=B_BLACK, letterIndex=0;

  letterIndex= findIndex(c);
   for (i=0; i<strlen(str); i++){
     ch = str[i];
     if (c == ch &&  repeatedLetters[letterIndex] >0 && checkTrue[index]==0) {
	     col= B_YELLOW;
	     repeatedLetters[letterIndex]--;
	     break;}
   }
  if (checkTrue[index] == 1) col = B_GREEN;
 return col;
}

void writeWord(int index, char text[MAX_TEXTBOX]){
    int j=0,i=0, x=0, y=0, col=B_BLACK;
    x = oldx + 6;
    y = oldy + 2;

    checkRepeatedLetters();
    for(j=0; j<index;j++)
     y = y + 3;
    for (i=0; i<5; i++) checkTrue[i] = 0;
    for (i=0; i<MAX_TEXTBOX; i++)
       {
           col=checkGreen(text[i], i, secretWord);
           writeCh(x, y, text[i], col, F_WHITE);
           x = x + 5;
       }
    x = oldx + 6;
    for (i=0; i<5; i++)
       {
           col=checkOrange(text[i], i, secretWord);
           writeCh(x, y, text[i], col, F_WHITE);
           x = x + 5;
       }
}

void checkRepeatedLetters(){
char ch=0;
size_t i,j;
for (i=0; i<5; i++) repeatedLetters[i] = 1;
   for (i=0; i<strlen(secretWord); i++){
      ch = secretWord[i];
      for (j=i+1; j<strlen(secretWord); j++){
        if (ch==secretWord[j]) {
             repeatedLetters[i] = repeatedLetters[i] + 1;
             repeatedLetters[j] = repeatedLetters[i];
        }
      }
   }
}

void autoSave() {
   int result = EXIT_SUCCESS;
   char file_name[] = "autosave.bin";
   FILE * fp = fopen(file_name, "wb");
   
   if (fp == NULL) {
     result = EXIT_FAILURE;
     fprintf(stderr, "fopen() failed for '%s'\n", file_name);
   }
   else {
     size_t element_size = sizeof *boardInputs;
     size_t elements_to_write = sizeof boardInputs;
     size_t elements_written = fwrite(boardInputs, element_size, elements_to_write, fp); 
     if (elements_written != elements_to_write) {
        result = EXIT_FAILURE;
        fprintf(stderr, "fwrite() failed: wrote only %zu out of %zu elements.\n", 
        elements_written, elements_to_write);
     }

     fclose(fp);
   }
}

int remove(const char * filename);

void gameLoop() {
char ch=0;
int cheat=0;

  do {
    cheat=0;
    memset(&textbox1,'\0',sizeof(textbox1));
     ch = textbox(1,wherey+2,5,"[+] Word:",textbox1,F_WHITE,F_WHITE,F_WHITE);
     if (ch == K_ESCAPE) break;
     if (strcmp(textbox1,"exit") ==0 || strcmp(textbox1,"quit") == 0)
      break;
     if (strcmp(textbox1,"cheat") == 0){
       writeStr(17,oldy+20,"                                  ", B_BLACK, F_GREEN);
       writeStr(18,oldy+20,secretWord, B_BLACK, F_GREEN);
       memset(&textbox1,'\0',sizeof(textbox1));
       cheat=1;
      }
     if ((strcmp(textbox1, "help") == 0) || (strcmp(textbox1, "word") == 0))
       break;
      if (strlen(textbox1) == 5){
         checkRepeatedLetters();
         toUpper(textbox1);
         autoSave();

         if (isWordinDictionary(fileSource,textbox1) == 1) {
             writeWord(currentIndex, textbox1);
             strcpy(boardInputs[currentIndex], textbox1);
             writeStr(wherex+16,wherey+2,"->VALID WORD!                   ", B_BLACK, F_GREEN);
             if (currentIndex<6) currentIndex++;
             if (strcmp(textbox1,secretWord) == 0){
               writeStr(wherex,wherey+1,"->SUCCESS!", B_BLACK, F_BLUE);
               remove("autosave.bin");
               break;
              } else{
             if (currentIndex == 6) {
              writeStr(wherex,wherey+1,"->GAME OVER:                     ", B_BLACK, F_MAGENTA);
              remove("autosave.bin");
              writeStr(wherex+14,wherey+1,secretWord, B_BLACK, F_GREEN);
             break;
            }
          }
         } else {
            writeStr(wherex+16,wherey+2,"->WORD NOT FOUND!                ", B_BLACK, F_RED);
         }
    }
     else{
       if (cheat==0) writeStr(wherex+16,wherey+2,"->TOO SHORT!                      ", B_BLACK, F_RED);
    }
  } while (ch!= K_ESCAPE);
   writeStr(1,wherey+2,"                                 ", B_BLACK,F_WHITE);
   if (strcmp(textbox1,"help") == 0){
     displayHelp();
     newGame();
  }
}

void newGame(){
int i=0;
 drawBoard();
   if (currentIndex >0){
     for (i=0; i<=currentIndex; i++)
       writeWord(i, boardInputs[i]);
   }
 gameLoop();
}

int openFile(FILE ** fileHandler, char *fileName, char *mode) {
  int ok;
  *fileHandler = fopen(fileName, mode);
  if(*fileHandler != NULL)
    ok = 1;
  else
    ok = 0;
  return ok;
}

long countWords(FILE * fileHandler) {
  long    wordCount = 0;
  char    ch;

  if(fileHandler != NULL) {
    rewind(fileHandler);
    ch = getc(fileHandler);
    while(!feof(fileHandler)) {
      if(ch == SEPARATOR) {
	wordCount++;
      }
      ch = getc(fileHandler);
    }
   }
  return wordCount;
}


void getWordfromDictionary(FILE * fileHandler, char WORD[MAX_TEXTBOX]) {
  long    i = 0;
  char    ch;
  char    dataString[MAX_TEXTBOX];

  if(fileHandler != NULL) {
    rewind(fileHandler);
    fseek(fileHandler,MAX_TEXTBOX*randomWord, SEEK_SET);
    ch = getc(fileHandler);
    while(i<5) {
      if (ch!= SEPARATOR) dataString[i++] = ch;
      ch = getc(fileHandler);
    }
  }
  dataString[i] = '\0';
  i = 0;
  strcpy(WORD, dataString);
}

int isWordinDictionary(FILE * fileHandler, char WORD[MAX_TEXTBOX]) {
  long    i = 0;
  int     isFound = 0;
  char    ch;
  char    dataString[MAX_TEXTBOX];

  //Read char by char
  if(fileHandler != NULL) {
    rewind(fileHandler);
    ch = getc(fileHandler);
    while(!feof(fileHandler)) {
      if (ch != SEPARATOR) dataString[i++] = ch;
      if(ch == SEPARATOR) {
	dataString[i] = '\0';
        if (strcmp(dataString, WORD) == 0) {isFound = 1; break;}
	i = 0;
      }
      ch = getc(fileHandler);
    }
  }
  return isFound;
}

int closeFile(FILE * fileHandler) {
  int     ok;
  ok = fclose(fileHandler);
  return ok;
}

void credits() {
  gotoxy(wherex,wherey+2);
  printf("\r");
  printf("C-Wordle. Coded by Popkov Denis, 21m 2022                   \n");
}

void* allocArray (int rows, int cols) {
  return malloc(sizeof(char[rows][cols]));
}

void readArray (int rows, int cols, char array[rows][cols]) {
   FILE *data;
   data = fopen("autosave.bin", "rb");
   fread(array, sizeof(char[rows][cols]), 1, data);
}

int main() {

  int cols = 7;
  int rows = 6;
  char (*myArray)[cols] = allocArray(rows, cols);

  readArray(rows, cols, myArray);
  strcpy(boardInputs, myArray);
  free(myArray);

   srand((unsigned) time(&t));
   oldx = wherex;
   oldy = wherey;
   getTerminalDimensions(&rows, &columns);
   if (rows < BOARDSIZEY || columns < BOARDSIZEX)
    {
      printf("Error: Terminal size is too small. Resize terminal. \n");
      exit(0);
   }
   cls();
   setCursor(0);

   // Поиск в словаре
   okFile = openFile(&fileSource, DICTIONARY, "r");
   okFile2 = openFile(&fileSource2, POSSIBLES, "r");
   if (okFile == 0 || okFile2 == 0) {
     dictionaryPresent = 0;
     words = 1;
     strcpy(secretWord, dummyWord);
     printf("ERROR: Dictionary file(s) is missing. Create file <dict.txt> and/or <possibles.txt>\n");
     exit(0);
   } else {
     dictionaryPresent = 1;
     words = countWords(fileSource);
     words2 = countWords(fileSource2);
     // Выбор случайного слова из словаря
     randomWord = rand() % words2;
     getWordfromDictionary(fileSource2, secretWord);
     newGame();
     if (fileSource != NULL) closeFile(fileSource);
     if (fileSource2 != NULL) closeFile(fileSource2);
     credits();
     setCursor(1);
   }
  return 0;
}
