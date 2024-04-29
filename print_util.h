#ifndef PRINT_UTIL_H
#define PRINT_UTIL_H

#include "miosix.h"
#include <string>

// Ansi escape codes for colors
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_CLEAR_SCREEN  "\x1b[2J"
#define ANSI_HIDE_CURSOR   "\x1b[?25l"
#define ANSI_SHOW_CURSOR   "\x1b[?25h"

void printShort(short data);

void printChar(char data);

void moveCursor(int x, int y);

void writeCharOnScreenPosition(int y, int x, char c);

void writeStringOnScreenPosition(int y, int x, const std::string str);

void moveCursiorDown(int n);

void waitForAck();

#endif