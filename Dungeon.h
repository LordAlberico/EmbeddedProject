#ifndef DUNGEON_H
#define DUNGEON_H

#include "lcg.h"
#include "miosix.h"
#include "print_util.h"
#include "Combact.h"
#include "user_button.h"
#include "Room.h"

class Room;

/**
 * @brief Dungeon class
 * 
 * This class represents a dungeon in a game made of only rooms that can have a maximum of 4 exits.
 * Every room can be empty or contain a monster or a treasure.
 * The room type can be seen by the bits of the room's byte.
 * See the constants for the room types.
 */
class Dungeon{
    private:
        // Dungeon size
        static const char WIDTH = 10;
        static const char HEIGHT = 10;

        // Player position
        int playerX = WIDTH / 2;
        int playerY = HEIGHT / 2;
        char playerFromDirection = -1;

        // Player stats
        char health = 3;
        int treasure = 0;

        // Dungeon
        char dungeon[WIDTH][HEIGHT];
        int seeds[WIDTH][HEIGHT];

        // Get the complementar direction (e.g. if the direction is north, the complementar direction is south)
        void getComplementarDirection(char direction);

        // Update the coordinates according to the direction (e.g. if the direction is north, the y coordinate is decremented)
        void updateCoordinates(char direction, int& x, int& y);

        // Combact class, onne instance of this class that will be used to handle multiple combacts, it will be reinitialized every time a combact starts
        Combact *combact = NULL;

        // Room class, one instance of this class that will be used to handle the room generation
        Room *room = NULL;

    public:
        // Room types
        static const char VISITED = 0b10000000;    
        static const char MONSTER = 0b01000000;
        static const char TREASURE = 0b00100000;
        static const char NORTH = 0b00010000;
        static const char EAST = 0b00001000;
        static const char SOUTH = 0b00000100;
        static const char WEST = 0b00000010;
        static const char MONSTER_TYPE_ALTERNATE = 0b00000001;
        
        // Constructor
        Dungeon();

        // Start combact with difficulty
        bool startCombact(bool difficulty);

        // Handle the combact stage
        bool combactStage();

        // Handle room stage
        void roomStage();

        // Print the dungeon
        void printDungeon();

        // Generate the dungeon randomly only around the player current position
        void generateDungeon();

        bool isDungeonExplored();

        // Get the room type
        char getRoomType(int x, int y);

        // Set the room type
        void setRoomType(int x, int y, char type);

        // Set treasure
        void setTreasure(int x, int y);

        // Set monster
        void setMonster(int x, int y, bool type);

        // Remove a feature
        void removeFeature(int x, int y, char feature);

        // Add a feature
        void addFeature(int x, int y, char feature);

        // Set visited
        void setVisited(int x, int y);

        // Check if the room is visited
        bool isVisited(int x, int y);

        // Check if the room is empty
        bool isEmpty(int x, int y);

        // Check if the room has a monster
        bool hasMonster(int x, int y);

        // Check if the room has a treasure
        bool hasTreasure(int x, int y);

        // Check if the room has an exit
        char getExits(int x, int y);

        // Get the monster type
        char getMonsterType(int x, int y);

        // Get the complementar direction
        char getCoplementarDirection(char direction);

        // Get the player position
        char getPlayerX();
        char getPlayerY();

        void decreaseHealth();
        void addTreasure();
        char getHealth();

        // Move the player
        void movePlayer(char direction);
};

#endif