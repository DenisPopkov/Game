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