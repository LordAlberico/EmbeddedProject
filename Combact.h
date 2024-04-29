#ifndef COMBACT_H
#define COMBACT_H

#include "miosix.h"
#include "lis3dsh.h"
#include "print_util.h"
#include "lcg.h"
#include <string.h>
#include <sstream>
#include "fun_utils.h"
#include "user_button.h"
#include "Interrupt.h"

using namespace miosix;
using namespace std;

/*
 * This class is used to handle the combact with the monsters.
 * The combact will be a miniagme with two possible difficulty levels.
 * The game uses the accelerometer LIS3DH to detect user's movements.
 * The user has to move the device in the direction to avoid the monster's attacks.
 * Random balls will be spawned from the screen borders and the user has to avoid them or shhot them using the user button.
 * The user wins if he survives for a certain amount of time.
 * The user loses if he gets hit by a ball. 
 * Both time and number of balls will increase with the difficulty level.
 */
class Combact{
	private:

    static const int SCREEN_SIZE = 25;
    static const int POSITION_MULTIPLIER = 1;
	
	bool difficulty; // 0 for easy, 1 for hard
	float time; // Time to survive
    int currency; // The currency that the user has, came from the dungeon and will be used to spawn projectiles

	char ballsNumber; // Number of balls to avoid

    float playerX;
    float playerY;
	float playerOldX;
	float playerOldY;
    const float playerSpeed = 0.002;
    float playerSpeedX;
    float playerSpeedY;

    bool isRunning;

	// Screen buffer
	stringstream screenString;
	int screenStartYPosition = 2;
	int screenStartXPosition = 1;
    
    // Simulation speed
    static const float SIMULATION_DELAY; // Control the simulation speed

    // Last cursor y position
    char lastCursorY;
	
	struct Ball{
        float x;
        float y;
		float oldX;
	    float oldY;
        float lastDrawnX;
        float lastDrawnY;
        float speed;
        char direction;
    };

    struct Projectile{
        float x;
        float y;
        float oldX;
        float oldY;
        float lastDrawnX;
        float lastDrawnY;
        float speed;
        char direction;
    };

	Ball balls[100];
    Projectile projectiles[4];

    bool hasHit(int x1, int x2, int oldX1, int oldX2, int y1, int y2, int oldY1, int oldY2);
	
	public:

	Combact(bool difficulty, int currency);

    // Destructor
    ~Combact();
	
	void startGame();

    int closeGame();

	void spawnBall(int i, bool newBall = true);

    void spawnProjectile();

	bool moveBalls();

    void movePlayer();

    bool movePlayerBallsProj();

    void moveProjectiles();

    void printScreen();

	void updateScreen();

    float getTime();

    void nextFrame();
};

#endif