#ifndef ROOM_H
#define ROOM_H

#include "lcg.h"
#include "print_util.h"
#include "lis3dsh.h"
#include "fun_utils.h"
#include "Dungeon.h"

class Dungeon;  // Forward declaration

class Room{
    private:

        Dungeon *dungeon;

        int screenStartX, screenStartY;

        static const int SIZE = 25;

        int map[SIZE][SIZE];
        int map2[SIZE][SIZE];

        bool exits[4];

        bool t;
        bool m;

        static const float SIMULATION_DELAY;

        float playerX;
        float playerY;
        float playerOldX;
        float playerOldY;
        const float playerSpeed = 0.001;
        float playerSpeedX;
        float playerSpeedY;

        int monsterX;
        int monsterY;
        int treasureX;
        int treasureY;

        bool isBorder(int i, int j);

        bool isExit(int i, int j);

        void copyToMap2();

        void switchMap();

        void iteration();

    public:

        Room(Dungeon *dungeon, int entrance, int n, int e, int s, int w, bool t, bool m, int seed, int startX, int startY);

        int movePlayer();

        void nextFrame();

        void updateMap();

        void printMap();

        void genExits();

        void handleEvents();

        void genFeatures();

        void randomMap();

        int generate();

        bool isConnected(int x, int y);
};

#endif