/*
Wordle game in Powershell.
Author - Popkov Denis, 21m
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include <wchar.h>

#define testWord "SPACE\0"
#define MAX_TEXTBOX 6
#define SEPARATOR 0x0A
#define DICTIONARY "words.txt"
#define POSSIBLES "words_random.txt"

//UNICODE chars
#define HOR_LINE 9472
#define VER_LINE 9474
#define UPPER_LEFT_CORNER 9484
#define LOWER_LEFT_CORNER 9492
#define UPPER_RIGHT_CORNER 9488
#define LOWER_RIGHT_CORNER 9496

//KEYS
#define K_BACKSPACE 127
#define K_ENTER 13
#define K_ESCAPE 27
#define K_ENTER2 10

// Background colors low intensity
#define B_BLACK 40
#define B_RED 41
#define B_GREEN 42
#define B_YELLOW 43
#define B_BLUE 44
#define B_MAGENTA 45
#define B_CYAN 46
#define B_WHITE 47

// Foreground colors low intensity
#define F_BLACK 30
#define F_RED 31
#define F_GREEN 32
#define F_YELLOW 33
#define F_BLUE 34
#define F_MAGENTA 35
#define F_CYAN 36
#define F_WHITE 37
#define F_GREY 90
#define BOARDSIZEY 21
#define BOARDSIZEX 35

char boardInputs[7][6];
char secretWord[6];
char firstTextBox[MAX_TEXTBOX];
int repeatedLetters[5] = {1, 1, 1, 1, 1};
int checkTrue[5] = {0, 0, 0, 0, 0};
struct winsize max;
static struct termios term1, term2, failsafe;
static int peekCharacter = -1;
int rows = 0, columns = 0;
int wherey = 0, wherex = 0;
int oldy = 0, oldx = 0;
int currentIndex = 0;
int okFile, okFile2;
int dictionaryPresent = 0;
FILE *fileSource;
FILE *fileSource2;
time_t time;
unsigned randomWord = 0;
unsigned words = 0;
unsigned words2 = 0;

int kbhit();
int readKeyTrail(char charTrail[5]);
int readch();
void resetch();
void gotoxy(int x, int y);
void outputColor(int foreground, int background);
int getTerminalDimensions(int *rows, int *columns);
int getPosition(int *y, int *x);
void hideCursor();
void showCursor();
void resetAnsi(int x);
void pushTerm();
int resetTerm();
void cls();

//USER INTERFACE
void writeStr(int wherex, int wherey, char *str, int backcolor, int forecolor);
char textbox(int wherex, int wherey, int displayLength,char label[MAX_TEXTBOX], char text[MAX_TEXTBOX], int backcolor,
	    int labelcolor, int textcolor);
void writeCh(int wherex, int wherey, wchar_t  ch, int backcolor, int forecolor);
int writeNum(int x, int y, int num, char backcolor, char forecolor);
void window(int x1, int y1, int x2, int y2, int backcolor,
         int bordercolor, int titlecolor, int border, int title);
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

int openFile(FILE ** fileHandler, char *fileName, char *mode);
long countWords(FILE * fileHandler);
void getWordfromDictionary(FILE * fileHandler, char WORD[MAX_TEXTBOX]);
int isWordinDictionary(FILE * fileHandler, char WORD[MAX_TEXTBOX]);
int closeFile(FILE * fileHandler);

void pushTerm() {
    tcgetattr(0, &failsafe);
}

int resetTerm() {
  if (tcsetattr(0, TCSAFLUSH, &failsafe) < 0) return -1;
  return 0;
}

int getTerminalDimensions(int *rows, int *columns) {
  ioctl(0, TIOCGWINSZ, &max);
  *columns = max.ws_col;
  *rows = max.ws_row;
  return 0;
}

void hidecursor() {
  printf("\e[?25l");
}

void showcursor() {
  printf("\e[?25h");
}

void cls() {
  system("clear");
}


int getPos(int *y, int *x) {

 char buf[30]={0};
 int ret, i, pow;
 char ch;

*y = 0; *x = 0;

 struct termios term, restore;

 tcgetattr(0, &term);
 tcgetattr(0, &restore);
 term.c_lflag &= ~(ICANON|ECHO);
 tcsetattr(0, TCSANOW, &term);

 write(1, "\033[6n", 4);

 for(i = 0, ch = 0; ch != 'R'; i++) {
    ret = read(0, &ch, 1);
    if (!ret) {
       tcsetattr(0, TCSANOW, &restore);
       return 1;
    }
    buf[i] = ch;
 }

 if (i < 2) {
    tcsetattr(0, TCSANOW, &restore);
    return(1);
 }

 for(i -= 2, pow = 1; buf[i] != ';'; i--, pow *= 10)
     *x = *x + ( buf[i] - '0' ) * pow;

 for(i-- , pow = 1; buf[i] != '['; i--, pow *= 10)
     *y = *y + ( buf[i] - '0' ) * pow;

 tcsetattr(0, TCSANOW, &restore);
 return 0;
}