#include "Dungeon.h"

using namespace miosix;

// Constructor: Initialize the dungeon
Dungeon::Dungeon() {
    for(int i = 0; i < WIDTH; i++) {
        for(int j = 0; j < HEIGHT; j++) {
            dungeon[i][j] = 0;
            seeds[i][j] = hardwareRNG(0, 0x7FFFFFFF);
        }
    }
}

// Start the combact by creating a new combact object
bool Dungeon::startCombact(bool difficulty) {
    this->combact = new Combact(difficulty, treasure);
    combact->startGame();
    return combactStage();
}

// The combact main loop, return true if the player was hit
bool Dungeon::combactStage(){
    bool isHit = false;
    while(combact->getTime() > 0 && !isHit){
        UserButton::getInstance()->callCallback();
        isHit = combact->movePlayerBallsProj();
		combact->updateScreen();
        combact->nextFrame();
    }
    if(isHit){
        treasure = combact->closeGame();
        delete combact;
        return true;
    }
    treasure = combact->closeGame();
    delete combact;
    return false;
}

// The room stage main loop, i.e. the movement and interacton of the player in a room
void Dungeon::roomStage() {
    // Get the exits of the room
    char exits = getExits(playerX, playerY);

    int n = exits & NORTH ? 1 : 0;
    int e = exits & EAST ? 1 : 0;
    int s = exits & SOUTH ? 1 : 0;
    int w = exits & WEST ? 1 : 0;

    int fromDirection;

    // Initial room has fromDirection = -1
    if(playerX == WIDTH / 2 && playerY == HEIGHT / 2 && playerFromDirection == -1){
        fromDirection = -1;
    }else{
        // Get the direction from where the player came
        fromDirection = playerFromDirection == NORTH ? 0 : playerFromDirection == EAST ? 1 : playerFromDirection == SOUTH ? 2 : 3;
    }

    // Create the room
    room = new Room(this, fromDirection, n, e, s, w, hasTreasure(playerX, playerY), hasMonster(playerX, playerY), seeds[playerX][playerY], 25, 3);
    room->generate();
    room->printMap();

    printf(ANSI_HIDE_CURSOR);
    int exitDirection;
    do{
        exitDirection = room->movePlayer();
        room->updateMap();
        room->nextFrame();
    }while(exitDirection == -1);
    printf(ANSI_SHOW_CURSOR);

    // Delete the room
    delete room;

    // Convert the exit direction to the char direction (TODO: refactor this to simplify the code)
    char direction;
    switch(exitDirection){
        case 0:
            direction = NORTH;
            break;
        case 1:
            direction = EAST;
            break;
        case 2:
            direction = SOUTH;
            break;
        case 3:
            direction = WEST;
            break;
    }

    movePlayer(direction);
}

char Dungeon::getRoomType(int x, int y) {
    return dungeon[x][y];
}

void Dungeon::setRoomType(int x, int y, char type) {
    dungeon[x][y] = type;
}

void Dungeon::setTreasure(int x, int y) {
    dungeon[x][y] |= TREASURE;
}

void Dungeon::setMonster(int x, int y, bool type) {
    dungeon[x][y] |= MONSTER;
    if (type) {
        dungeon[x][y] |= MONSTER_TYPE_ALTERNATE;
    }
}

void Dungeon::removeFeature(int x, int y, char feature) {
    dungeon[x][y] &= ~feature;
}

void Dungeon::addFeature(int x, int y, char feature) {
    dungeon[x][y] |= feature;
}

void Dungeon::setVisited(int x, int y) {
    dungeon[x][y] |= VISITED;
}

bool Dungeon::isVisited(int x, int y) {
    return dungeon[x][y] & VISITED;
}

bool Dungeon::isEmpty(int x, int y) {
    return dungeon[x][y] == 0;
}

bool Dungeon::hasMonster(int x, int y) {
    return dungeon[x][y] & MONSTER;
}

bool Dungeon::hasTreasure(int x, int y) {
    return dungeon[x][y] & TREASURE;
}

char Dungeon::getExits(int x, int y) {
    return dungeon[x][y] & (NORTH | EAST | SOUTH | WEST);
}

char Dungeon::getMonsterType(int x, int y) {
    return dungeon[x][y] & MONSTER_TYPE_ALTERNATE;
}

char Dungeon::getCoplementarDirection(char direction) {
    switch(direction) {
        case NORTH:
            return SOUTH;
        case EAST:
            return WEST;
        case SOUTH:
            return NORTH;
        case WEST:
            return EAST;
    }
    return 0;
}

char Dungeon::getPlayerX() {
    return playerX;
}

char Dungeon::getPlayerY() {
    return playerY;
}

void Dungeon::movePlayer(char direction) {
    // Move the player if possible
    switch(direction) {
        case NORTH:
            if (playerY > 0 && getExits(playerX, playerY) & NORTH) {
                playerY--;
                playerFromDirection = SOUTH;
            }
            break;
        case EAST:
            if (playerX < WIDTH - 1 && getExits(playerX, playerY) & EAST) {
                playerX++;
                playerFromDirection = WEST;
            }
            break;
        case SOUTH:
            if (playerY < HEIGHT - 1 && getExits(playerX, playerY) & SOUTH) {
                playerY++;
                playerFromDirection = NORTH;
            }
            break;
        case WEST:
            if (playerX > 0 && getExits(playerX, playerY) & WEST) {
                playerX--;
                playerFromDirection = EAST;
            }
            break;
    }

    // Set the room as visited
    setVisited(playerX, playerY);
    
    // Generate rooms around the player
    generateDungeon();

    // Win condition
    if (isDungeonExplored()) {
        printf(ANSI_CLEAR_SCREEN);
        moveCursor(1, 1);
        printf(ANSI_COLOR_GREEN "You explored the dungeon!\n" ANSI_COLOR_RESET);
        waitForAck();
        exit(0);
    }
}

void Dungeon::printDungeon() {
    printf(ANSI_HIDE_CURSOR);
    moveCursor(1, 1);
    printf("Health: %d\tTreasure: %d\n", health, treasure);
    for(char y = 0; y < HEIGHT; y++) {
        for(char x = 0; x < WIDTH; x++) {
            if (x == playerX && y == playerY) {
                printf(ANSI_COLOR_RED);
            } else if (hasMonster(x, y) && hasTreasure(x, y)) {
                printf(ANSI_COLOR_GREEN);
            } else if (hasMonster(x, y)) {
                printf(ANSI_COLOR_BLUE);
            } else if(hasTreasure(x, y)) {
                printf(ANSI_COLOR_YELLOW);
            }


            if (getExits(x, y) == (NORTH | EAST | SOUTH | WEST)) {
                printf("╬");
            } else if (getExits(x, y) == (NORTH | EAST | SOUTH)) {
                printf("╠");
            } else if (getExits(x, y) == (NORTH | EAST | WEST)) {
                printf("╩");
            } else if (getExits(x, y) == (NORTH | SOUTH | WEST)) {
                printf("╣");
            } else if (getExits(x, y) == (EAST | SOUTH | WEST)) {
                printf("╦");
            } else if (getExits(x, y) == (NORTH | EAST)) {
                printf("╚");
            } else if (getExits(x, y) == (NORTH | SOUTH)) {
                printf("║");
            } else if (getExits(x, y) == (NORTH | WEST)) {
                printf("╝");
            } else if (getExits(x, y) == (EAST | SOUTH)) {
                printf("╔");
            } else if (getExits(x, y) == (EAST | WEST)) {
                printf("═");
            } else if (getExits(x, y) == (SOUTH | WEST)) {
                printf("╗");
            } else if (getExits(x, y) == NORTH) {
                printf("╨");
            } else if (getExits(x, y) == EAST) {
                printf("╞");
            } else if (getExits(x, y) == SOUTH) {
                printf("╥");
            } else if (getExits(x, y) == WEST) {
                printf("╡");
            } else {
                printf(" ");
            }

            printf(ANSI_COLOR_RESET);
        }
        printf("\n");
    }
    printf(ANSI_SHOW_CURSOR);

    // Flush
    fflush(stdout);
}

void Dungeon::updateCoordinates(char direction, int& x, int& y) {
    switch(direction) {
        case NORTH:
            y--;
            break;
        case EAST:
            x++;
            break;
        case SOUTH:
            y++;
            break;
        case WEST:
            x--;
            break;
    }
}

bool Dungeon::isDungeonExplored() {
    // All the rooms must be visited
    for(int i = 0; i < WIDTH; i++) {
        for(int j = 0; j < HEIGHT; j++) {
            if (!isVisited(i, j) && !isEmpty(i, j)) {
                return false;
            }
        }
    }
    return true;
}

void Dungeon::generateDungeon() {
    // First room is visited and has all the exits
    if (isEmpty(playerX, playerY)) {
        setRoomType(playerX, playerY, NORTH | EAST | SOUTH | WEST | VISITED);
    }

    char availableExits = getExits(playerX, playerY);

    // If the player adjacent rooms are empty, generate the room coherently with the adjacent rooms
    for(char d = NORTH; d >= WEST; d >>= 1) {
        // Generate the room if an exits to that direction is available
        if (availableExits & d) {
            int x = playerX;
            int y = playerY;
            updateCoordinates(d, x, y);

            if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT && isEmpty(x, y)) {
                // Check the adjacent rooms of the room to generate and get the mandatory exits
                // Also save the directions of empty rooms for the eventual optional exits
                char manadatoryDirections = 0;
                char emptyDirections = 0;
                for(char d2 = NORTH; d2 >= WEST; d2 >>= 1) {
                    int x2 = x;
                    int y2 = y;
                    updateCoordinates(d2, x2, y2);
                    if (x2 >= 0 && x2 < WIDTH && y2 >= 0 && y2 < HEIGHT) {
                        manadatoryDirections |= dungeon[x2][y2] & getCoplementarDirection(d2) ? d2 : 0;
                        if (isEmpty(x2, y2)) {
                            emptyDirections |= d2;
                        }
                    }
                }

                // Generate the room
                printf("Generating room at %d, %d\n", x, y);

                // Generate the room "mandatory" exits
                char exits = 0;
                for(char d2 = NORTH; d2 >= WEST; d2 >>= 1) {
                    if (manadatoryDirections & d2) {
                        exits |= d2;
                    }
                }
                
                // Generate the room "optional" exits only if can be generated
                for(char d2 = NORTH; d2 >= WEST; d2 >>= 1) {
                    if (emptyDirections & d2) {
                        if (hardwareRNG(0, 1)) {
                            exits |= d2;
                        }
                    }
                }

                setRoomType(x, y, exits);
                    
                // Generate the room features
                if (hardwareRNG(0, 1)) {
                    setTreasure(x, y);
                }
                if (hardwareRNG(0, 1)) {
                    setMonster(x, y, hardwareRNG(0, 1));
                }

                printf("Room type: ");
                printChar(dungeon[x][y]);           
            }      
        }
    }
}

void Dungeon::decreaseHealth() {
    health--;
}

void Dungeon::addTreasure() {
    treasure++;
}

char Dungeon::getHealth() {
    return health;
}