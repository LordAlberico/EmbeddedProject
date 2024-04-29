#include "Room.h"

using namespace std;

const float Room::SIMULATION_DELAY = 0.016;

// Initialize the room based on the dungeon parameters and the player entrance
Room::Room(Dungeon *dungeon, int entrance, int n, int e, int s, int w, bool t, bool m, int seed, int startX, int startY)
{
    this->dungeon = dungeon;

    this->t = t;
    this->m = m;

    exits[0] = n;
    exits[1] = e;
    exits[2] = s;
    exits[3] = w;
    setPseudoSeed(seed);

    switch (entrance)
    {
    case -1:
        playerX = (int)(SIZE / 2);
        playerY = (int)(SIZE / 2);
        break;
    case 0:
        playerX = (int)(SIZE / 2);
        playerY = 1;
        break;
    case 1:
        playerX = (int)(SIZE - 2);
        playerY = (int)(SIZE / 2);
        break;
    case 2:
        playerX = (int)(SIZE / 2);
        playerY = (int)(SIZE - 2);
        break;
    case 3:
        playerX = 1;
        playerY = (int)(SIZE / 2);
        break;
    }
    playerOldX = playerX;
    playerOldY = playerY;

    this->screenStartX = startX;
    this->screenStartY = startY;
}

bool Room::isBorder(int i, int j)
{
    return i == 0 || i == SIZE - 1 || j == 0 || j == SIZE - 1;
}

bool Room::isExit(int i, int j)
{
    return (i == 0 && j == SIZE / 2) || (i == SIZE / 2 && j == SIZE - 1) || (i == SIZE - 1 && j == SIZE / 2) || (i == SIZE / 2 && j == 0);
}

void Room::switchMap()
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            map[i][j] = map2[i][j];
        }
    }
}

void Room::copyToMap2()
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            map2[i][j] = map[i][j];
        }
    }
}

bool Room::isConnected(int x, int y)
{
    // Check if the room is connected to the center. Use recursion.
    // Call the function going in every direction and if it finds the center, return true.
    // The visited rooms are marked with -1 value in map2.
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE)
    {
        return false;
    }
    if (map2[y][x] == 1)
    {
        return false;
    }
    if (map2[y][x] == -1)
    {
        return false;
    }
    if (map2[y][x] == 0)
    {
        map2[y][x] = -1;
    }
    if (x == SIZE / 2 && y == SIZE / 2)
    {
        return true;
    }
    return isConnected(x + 1, y) || isConnected(x - 1, y) || isConnected(x, y + 1) || isConnected(x, y - 1);
}

void Room::handleEvents(){
    // Handle events in the room
    // 1: Monster
    bool comabctStarted = false;
    bool hasBeenHit = false;
    if (m && (int)playerX == monsterX && (int)playerY == monsterY) {
        comabctStarted = true;
        hasBeenHit = dungeon->startCombact(dungeon->getMonsterType(playerX, playerY));
    }
    
    if(comabctStarted){
        // Toggle the monster
        dungeon->removeFeature(dungeon->getPlayerX(), dungeon->getPlayerY(), dungeon->MONSTER);
        map[monsterY][monsterX] = 0;
        m = false;
        monsterX = -1;
        monsterY = -1;

        // Decrease the health
        if (hasBeenHit) {
            dungeon->decreaseHealth();
        }

        // Clear screen and print win/lose message
        printf(ANSI_CLEAR_SCREEN);
        if(hasBeenHit){
            printf(ANSI_COLOR_RED "You lost the combact\n" ANSI_COLOR_RESET);
        } else {
            printf(ANSI_COLOR_YELLOW "You won the combact\n" ANSI_COLOR_RESET);
        }
        waitForAck();

        printf(ANSI_CLEAR_SCREEN);
        dungeon->printDungeon();
        printMap();
    }

    // 2: Health
    if (dungeon->getHealth() <= 0) {
        printf(ANSI_CLEAR_SCREEN);
        moveCursor(1, 1);
        printf(ANSI_COLOR_RED "You lost the game!\n" ANSI_COLOR_RESET);
        waitForAck();
        exit(0);
    }

    // 3: Treasure:
    bool treasureTaken = false;
    if (t && (int)playerX == treasureX && (int)playerY == treasureY) {
        dungeon->addTreasure();
        treasureTaken = true;
    }
    // Toggle the treasure
    if (treasureTaken) {
        dungeon->removeFeature(dungeon->getPlayerX(), dungeon->getPlayerY(), dungeon->TREASURE);
        map[treasureY][treasureX] = 0;
        t = false;
        treasureX = -1;
        treasureY = -1;

        printf(ANSI_CLEAR_SCREEN);
        dungeon->printDungeon();
        printMap();
    }
}

int Room::movePlayer()
{
    // Read the accelerometer
    short x, y, z;
    float K = lis3dsh::K;
    lis3dsh::readAllAxis(x, y, z);
    float X = x * K;
    float Y = y * K;

    // Calculate the speed based on the accelerometer
    playerSpeedX = fun_utils::abs(X) > 200 ? playerSpeed * x * SIMULATION_DELAY : 0;
    playerSpeedY = fun_utils::abs(Y) > 200 ? -playerSpeed * y * SIMULATION_DELAY : 0;

    // Save the old position
    playerOldX = playerX;
    playerOldY = playerY;

    // Move the player
    playerX += playerSpeedX;
    playerY += playerSpeedY;

    // Block player if it is going to hit a wall
    if (map[(int)playerY][(int)playerX] == 1)
    {
        playerX = playerOldX;
        playerY = playerOldY;
    }

    handleEvents();

    // If the player is in an exit, return the exit number
    if (isExit((int)playerY, (int)playerX))
    {
        if ((int)playerY == 0)
        {
            return 0;
        }
        else if ((int)playerX == SIZE - 1)
        {
            return 1;
        }
        else if ((int)playerY == SIZE - 1)
        {
            return 2;
        }
        else
        {
            return 3;
        }
    }else{
        return -1;
    }
}

void Room::nextFrame()
{
    Thread::sleep(SIMULATION_DELAY * 1000);
}

void Room::updateMap()
{
    printf(ANSI_HIDE_CURSOR);
    moveCursor(screenStartX, screenStartY);
    printf(ANSI_COLOR_RESET "(X: %d, Y: %d)", (int)playerX, (int)playerY);
    // Update player position if it changed
    if ((int)playerOldX != (int)playerX || (int)playerOldY != (int)playerY)
    {
        // Remove the player from the old position
        writeStringOnScreenPosition((int)playerOldX + screenStartX - 1, (int)playerOldY + screenStartY, ANSI_COLOR_RESET " ");

        // Rewrite the player in the new position
        writeStringOnScreenPosition((int)playerX + screenStartX - 1, (int)playerY + screenStartY, ANSI_COLOR_RED "X");
    }

    // Update treasure position if it changed
    if (t)
    {
        writeStringOnScreenPosition(treasureX + screenStartX - 1, treasureY + screenStartY, ANSI_COLOR_YELLOW "$");
    }else{
        writeStringOnScreenPosition(treasureX + screenStartX - 1, treasureY + screenStartY, ANSI_COLOR_RESET " ");
    }

    // Update monster position if it changed
    if (m)
    {
        writeStringOnScreenPosition(monsterX + screenStartX - 1, monsterY + screenStartY, ANSI_COLOR_BLUE "@");
    }else{
        writeStringOnScreenPosition(monsterX + screenStartX - 1, monsterY + screenStartY, ANSI_COLOR_RESET " ");
    }
    fflush(stdout);
}

void Room::printMap()
{
    moveCursor(screenStartX, screenStartY);
    printf(ANSI_COLOR_RESET "(X: %d, Y: %d)", (int)playerX, (int)playerY);
    moveCursor(screenStartX, screenStartY + 1);
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if((int)playerX == j && (int)playerY == i){
                printf(ANSI_COLOR_RED "X" ANSI_COLOR_RESET);
            }else if (map[i][j] == 0)
            {
                printf(" ");
            }
            else if (map[i][j] == 1)
            {
                printf("▓");
            }
            else if (map[i][j] == 2)
            {
                printf(ANSI_COLOR_YELLOW "$" ANSI_COLOR_RESET);
            }
            else if (map[i][j] == 3)
            {
                printf(ANSI_COLOR_BLUE "@" ANSI_COLOR_RESET);
            }
        }
        moveCursor(screenStartX, screenStartY + i + 2);
    }

    // Flush
    fflush(stdout);
}

// A cellar automata algorithm to generate the room, if there are a majority of walls around a cell, it becomes a wall
void Room::iteration()
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            int count = 0;
            for (int a = -1; a <= 1; a++)
            {
                for (int b = -1; b <= 1; b++)
                {
                    if (i + a >= 0 && i + a < SIZE && j + b >= 0 && j + b < SIZE)
                    {
                        if (map[i + a][j + b] == 1)
                        {
                            count++;
                        }
                    }
                }
            }
            if (count >= 5 || count == 0 || isBorder(i, j))
            {
                map2[i][j] = 1;
            }
            else
            {
                map2[i][j] = 0;
            }
        }
    }

    switchMap();
}

// Generate simple exits and corridors from the center in the direction of the exits
void Room::genExits()
{
    int center = SIZE / 2;
    // Path from center to each exit
    for (int i = 0; i < 4; i++)
    {
        if (exits[i] == 1)
        {
            if (i == 0)
            { // North
                for (int j = 0; j <= center; j++)
                {
                    map[j][center] = 0;
                }
            }
            else if (i == 1)
            { // East
                for (int j = center; j < SIZE; j++)
                {
                    map[center][j] = 0;
                }
            }
            else if (i == 2)
            { // South
                for (int j = center; j < SIZE; j++)
                {
                    map[j][center] = 0;
                }
            }
            else
            {
                for (int j = 0; j <= center; j++)
                {
                    map[center][j] = 0;
                }
            }
        }
    }
}

void Room::genFeatures()
{
    // Generate treasure in the room in a casual empty position that is reachable from the center
    if (t)
    {
        int x = pseudoRNG(1, SIZE - 2);
        int y = pseudoRNG(1, SIZE - 2);
        int count = 0;
        while (map[y][x] != 0 || !isConnected(x, y))
        {
            if (count > 10000) // Avoid infinite loop, theoretically it should never happen
            {
                printf(ANSI_CLEAR_SCREEN);
                printf("Error: Could not generate treasure! Sorry for the inconvenience, the game must be restarted.\n");
                waitForAck();
                exit(1);
            }

            x = pseudoRNG(1, SIZE - 1);
            y = pseudoRNG(1, SIZE - 1);

            count++;
        }
        map[y][x] = 2;
        treasureX = x;
        treasureY = y;
    }

    // Generate monster always befor an exit
    if (m)
    {
        // Randomize the exit
        int i;
        do
        {
            i = pseudoRNG(0, 3);
        } while (exits[i] == 0);

        // Randomize the position
        int x, y;
        if (exits[i] == 1)
        {
            if (i == 0)
            { // North
                x = SIZE / 2;
                y = pseudoRNG(1, 5);
            }
            else if (i == 1)
            { // East
                x = pseudoRNG(SIZE - 6, SIZE - 2);
                y = SIZE / 2;
            }
            else if (i == 2)
            { // South
                x = SIZE / 2;
                y = pseudoRNG(SIZE - 6, SIZE - 2);
            }
            else
            {
                x = pseudoRNG(1, 5);
                y = SIZE / 2;
            }
            map[y][x] = 3;
            monsterX = x;
            monsterY = y;
        }
    }
}

void Room::randomMap()
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            map[i][j] = (pseudoRNG(0, 100)) < 40 ? 1 : 0;
            if (isBorder(i, j))
            {
                map[i][j] = 1;
            }
        }
    }
}

int Room::generate()
{
    randomMap();
    for (int i = 0; i < 5; i++)
    {
        iteration();
    }
    genExits();
    copyToMap2(); // VERY IMPORTANT! This is the map that will be used to check if the room is connected to the center in the genFeatures method
    genFeatures();
    return 0;
}