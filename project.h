#include "Dungeon.h"
#include "Combact.h"
#include "lis3dsh.h"
#include "print_util.h"
#include <pthread.h>
#include <time.h>
#include "user_button.h"
#include "hcsr04.h"
#include "Room.h"
#include "Buzzer.h"
#include "fun_utils.h"
#include "Interrupt.h"

using namespace std;
using namespace miosix;

Dungeon dungeon;

void callback(){
    printf("Button pressed\n");
}

void dungeonIntroduction(){
    printf(ANSI_CLEAR_SCREEN);
    printf(ANSI_COLOR_RESET);
    moveCursor(1, 1);
    printf(""
    R"( _________  ___  ___   _________  _______   ________          _________  ________  _______   ________  ________  ___  ___  ________  _______   ________      )" "\n"
    R"(|\___   ___\\  \|\  \ |\___   ___\\  ___ \ |\   ___ \        |\___   ___\\   __  \|\  ___ \ |\   __  \|\   ____\|\  \|\  \|\   __  \|\  ___ \ |\   ____\     )" "\n"
    R"(\|___ \  \_\ \  \ \  \\|___ \  \_\ \   __/|\ \  \_|\ \       \|___ \  \_\ \  \|\  \ \   __/|\ \  \|\  \ \  \___|\ \  \\\  \ \  \|\  \ \   __/|\ \  \___|_    )" "\n"
    R"(     \ \  \ \ \  \ \  \    \ \  \ \ \  \_|/_\ \  \ \\ \           \ \  \ \ \   _  _\ \  \_|/_\ \   __  \ \_____  \ \  \\\  \ \   _  _\ \  \_|/_\ \_____  \   )" "\n"
    R"(      \ \  \ \ \  \ \  \____\ \  \ \ \  \_|\ \ \  \_\\ \           \ \  \ \ \  \\  \\ \  \_|\ \ \  \ \  \|____|\  \ \  \\\  \ \  \\  \\ \  \_|\ \|____|\  \  )" "\n"
    R"(       \ \__\ \ \__\ \_______\ \__\ \ \_______\ \_______\           \ \__\ \ \__\\ _\\ \_______\ \__\ \__\____\_\  \ \_______\ \__\\ _\\ \_______\____\_\  \ )" "\n"
    R"(        \|__|  \|__|\|_______|\|__|  \|_______|\|_______|            \|__|  \|__|\|__|\|_______|\|__|\|__|\_________\|_______|\|__|\|__|\|_______|\_________\)" "\n"
    R"(                                                                                                         \|_________|                            \|_________|)" "\n");
    moveCursiorDown(5);
    printf(ANSI_COLOR_YELLOW "Welcome to the Tilted Treasures! " ANSI_COLOR_RESET "\n");
    printf("In this thrilling CLI game, you, the brave adventurer, are tasked with exploring a randomly generated dungeon filled with treasures and menacing monsters.\n");
    printf(ANSI_COLOR_YELLOW "Rooms filled with treasure glow with a golden hue, " ANSI_COLOR_RESET);
    printf(ANSI_COLOR_BLUE "while those harboring monsters emanate a chilling blue light. " ANSI_COLOR_RESET);
    printf(ANSI_COLOR_GREEN "Rooms containing both treasures and monsters radiate a mysterious green. " ANSI_COLOR_RESET "\n");
    printf("When you encounter a monster, you'll enter a high-stakes minigame where you must dodge or shoot down approaching projectiles at the cost of your hard-earned treasure.\n");
    printf("The game concludes when you've explored the entire dungeon or when you've exhausted all your three lives.\n");
    printf("So, gear up, adventurer! The dungeon awaits your courage and cunning. Will you emerge as the ultimate Dungeon Explorer?\n");
    lis3dsh::waitUntilInt1();
}

void setup(){
    // Initialize the accelerometer
    lis3dsh::spiInitLIS3DSH();
    lis3dsh::initializeLIS3DSH();
    lis3dsh::setWakeUpSM();
    lis3dsh::printSensorTest();

    // Initialize the dungeon
    dungeon.generateDungeon();

    // Start the game
    dungeonIntroduction();

    // Change the accelerometer FSM
    lis3dsh::resetSM();
    lis3dsh::setResetSM();
    Interrupt::Interrupt_EXTI_PE0_init();
}

void dungeonLoop(){
    printf(ANSI_CLEAR_SCREEN);
    dungeon.printDungeon();
    dungeon.roomStage();
}

void mainLoop(){
    dungeonLoop();
}

void close(){
}