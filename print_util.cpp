#include "print_util.h"

using namespace std;

void printShort(short data){
    // Print the short value bit by bit
    for(int i = 15; i >= 0; i--){
        printf("%d", (data >> i) & 1);
    }
    printf("\n");
}

void printChar(char data){
    // Print the char value bit by bit
    for(int i = 7; i >= 0; i--){
        printf("%d", (data >> i) & 1);
    }
    printf("\n");
}

void moveCursor(int x, int y){
    // Move the cursor to the specified position
    printf("\x1b[%d;%dH", y, x);
}

void writeCharOnScreenPosition(int x, int y, char c){
    moveCursor(x + 1, y + 1);
    printf("%c", c);
}

void writeStringOnScreenPosition(int x, int y, const string str){
    moveCursor(x + 1, y + 1);
    printf("%s", str.c_str());
}

void moveCursiorDown(int n){
    printf("\x1b[%dB", n);
}

void waitForAck(){
    // Wait for the user to press enter
    printf("Press y to continue\n");
    char c;
    while(c != 'y'){
        scanf("%c", &c);
    }
    // Absorb the newline character
    while ((getchar()) != '\n');
}