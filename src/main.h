#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/time.h>
#include <string.h>

void DrawMap();
int ExitGame();
void RenderUI();
void InputLogic();
void AppleLogic();
void SnakeLogic();
void GameOver();
void Initialization();
void Debugger();
void WriteLine();
void AI_Mode();

/* map size */ 
#define COLS 50
#define ROWS 20

/* relative to top left corner */ 
#define UI_OFFSET_X 58
#define UI_OFFSET_Y 3

/* values */
int head, tail;
int dir_x, dir_y; 
int apple_x, apple_y;
int score; 
int debug; 
int debug_cleared; 
int ai_mode;
int x_diff;
int y_diff;

struct termios orig_termios, new_termios;

const int map_size = COLS * ROWS;
int x[COLS * ROWS], y[COLS * ROWS];