#include "Combact.h"

const float Combact::SIMULATION_DELAY = 0.032;

Combact::Combact(bool difficulty, int currency)
{
    if(!difficulty){
        time = 10;
        ballsNumber = 5;
    }else{
        time = 20;
        ballsNumber = 7;
    }
    this->difficulty = difficulty;
    this->currency = currency;

    // Reset the balls
    for(int i = 0; i < 100; i++){
        balls[i].x = -1;
        balls[i].y = -1;
    }

    // Reset the projectiles
    for(int i = 0; i < 4; i++){
        projectiles[i].x = -1;
        projectiles[i].y = -1;
    }
}

Combact::~Combact(){
    // Close the game if it's still running
    if(isRunning){
        closeGame();
    }
}

void Combact::startGame(){
    // Set the game as running
    isRunning = true;

    // Initialize the player position
    playerX = SCREEN_SIZE / 2;
    playerY = SCREEN_SIZE / 2;
	playerOldX = playerX;
	playerOldY = playerY;

    // Initialize the balls
    for(int i = 0; i < ballsNumber; i++){
        spawnBall(i);
    }

    // Create the print thread
    printf(ANSI_CLEAR_SCREEN);
	printf(ANSI_HIDE_CURSOR);
	printScreen();

    // Set callback for the user button: Combact::spawnProjectile and start the button thread
    UserButton::getInstance()->start();
    UserButton::getInstance()->setCallback([this]() { this->spawnProjectile(); });
}

// Returns the left currency
int Combact::closeGame(){
    // Set the game as not running
    isRunning = false;

    // Remove the callback for the user button and stop the button thread
    UserButton::getInstance()->removeCallback();
    UserButton::getInstance()->close();

	printf(ANSI_SHOW_CURSOR);

    // Return the currency
    return currency;
}
    
void Combact::spawnBall(int i, bool newBall){
    // Randomize the corner from which the ball will spawn and change direction accordingly
    // 0: left, 1: right, 2: top, 3: bottom
    // The balls will spawn near the player
    int corner = hardwareRNG(0, 3);
    switch(corner){
        case 0:
            balls[i].x = 0;
            balls[i].y = hardwareRNG(playerY - 5, playerY + 5);
            balls[i].direction = 0;
            break;
        case 1:
            balls[i].x = SCREEN_SIZE - 1;
            balls[i].y = hardwareRNG(playerY - 5, playerY + 5);
            balls[i].direction = 1;
            break;
        case 2:
            balls[i].x = hardwareRNG(playerX - 5, playerX + 5);
            balls[i].y = 0;
            balls[i].direction = 2;
            break;
        case 3:
            balls[i].x = hardwareRNG(playerX - 5, playerX + 5);
            balls[i].y = SCREEN_SIZE - 1;
            balls[i].direction = 3;
            break;
    }
    balls[i].oldX = balls[i].x;
    balls[i].oldY = balls[i].y;

    // Randomize the speed.
    if(difficulty){
        balls[i].speed = hardwareRNG(4, 5) * SIMULATION_DELAY;
    }else{
        balls[i].speed = hardwareRNG(3, 4) * SIMULATION_DELAY;
    }
}

void Combact::spawnProjectile(){
    if(currency > 0){
        // The projectiles will spawn from the player to all the directions
        for(int i = 0; i < 4; i++){
            projectiles[i].x = playerX;
            projectiles[i].y = playerY;
            projectiles[i].oldX = playerX;
            projectiles[i].oldY = playerY;
            projectiles[i].speed = 24.0 * SIMULATION_DELAY;
            projectiles[i].direction = i;
        }
        currency--;
    }
}

bool Combact::moveBalls(){
    for(int i = 0; i < ballsNumber; i++){
		// Save the old position
		balls[i].oldX = balls[i].x;
		balls[i].oldY = balls[i].y;

        // Move the ball
        switch(balls[i].direction){
            case 0:
                balls[i].x += balls[i].speed;
                break;
            case 1:
                balls[i].x -= balls[i].speed;
                break;
            case 2:
                balls[i].y += balls[i].speed;
                break;
            case 3:
                balls[i].y -= balls[i].speed;
                break;
        }

        // Check if the ball is out of the screen
        if(balls[i].x < 0 || balls[i].x >= SCREEN_SIZE || balls[i].y < 0 || balls[i].y >= SCREEN_SIZE){
            // Respawn the ball
            spawnBall(i, false);
        }

        // Check if the player is hit by a ball
        if(hasHit((int)playerX, (int)balls[i].x, (int)playerOldX, (int)balls[i].oldX, (int)playerY, (int)balls[i].y, (int)playerOldY, (int)balls[i].oldY)){
            return true;
        }
    }

    return false;
}

void Combact::moveProjectiles(){
    for(int i = 0; i < 4; i++){
        // Save the old position
        projectiles[i].oldX = projectiles[i].x;
        projectiles[i].oldY = projectiles[i].y;

        // Move the projectile
        if(projectiles[i].x >= 0 && projectiles[i].x < SCREEN_SIZE && projectiles[i].y >= 0 && projectiles[i].y < SCREEN_SIZE){
            switch(projectiles[i].direction){
            case 0:
                projectiles[i].x += projectiles[i].speed;
                break;
            case 1:
                projectiles[i].x -= projectiles[i].speed;
                break;
            case 2:
                projectiles[i].y += projectiles[i].speed;
                break;
            case 3:
                projectiles[i].y -= projectiles[i].speed;
                break;
            }
        }
        
        // Check if the projectile is out of the screen
        if(projectiles[i].x < 0 || projectiles[i].x >= SCREEN_SIZE || projectiles[i].y < 0 || projectiles[i].y >= SCREEN_SIZE){
            // Remove the projectile from the screen (for now make them out of the screen)
            projectiles[i].x = -1;
            projectiles[i].y = -1;
            projectiles[i].oldX = -1;
            projectiles[i].oldY = -1;
        }

        // Check if the projectile hit a ball, if so, respawn the ball and remove the projectile
        for(int j = 0; j < ballsNumber; j++){
            if(hasHit((int)projectiles[i].x, (int)balls[j].x, (int)projectiles[i].oldX, (int)balls[j].oldX, (int)projectiles[i].y, (int)balls[j].y, (int)projectiles[i].oldY, (int)balls[j].oldY)){
                // Respawn the ball
                spawnBall(j, false);
                // Remove the projectile from the screen (for now make them out of the screen)
                projectiles[i].x = -1;
                projectiles[i].y = -1;
                projectiles[i].oldX = -1;
                projectiles[i].oldY = -1;
            }
        }
    }

}

bool Combact::hasHit(int x1, int x2, int oldX1, int oldX2, int y1, int y2, int oldY1, int oldY2){
    // Check if the two entities are in the same position
    if(x1 == x2 && y1 == y2){
        return true;
    }
    // Can happen that the speed of one of the entitiy is too high and the collision is not detected, so we need to check if the old position was between the new and old position of the other entity
    if((fun_utils::isBetween(oldX1, oldX2, x2) && fun_utils::isBetween(oldY1, oldY2, y2)) || (fun_utils::isBetween(oldX2, oldX1, x1) && fun_utils::isBetween(oldY2, oldY1, y1))){
        return true;
    }
    return false;
}

void Combact::movePlayer(){
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

    // Clamp the player position
    playerX = fun_utils::clamp(playerX, 0, SCREEN_SIZE - 1);
    playerY = fun_utils::clamp(playerY, 0, SCREEN_SIZE - 1);
}

bool Combact::movePlayerBallsProj(){
    movePlayer();
    bool isHit = moveBalls();
    moveProjectiles(); // Must be called after moveBalls to avoid the balls to be moved again after respawn
    return isHit;
}

float Combact::getTime(){
    return time;
}

void Combact::nextFrame(){
    // Decrease the timer based on SIMULATION_DELAY
    time -= SIMULATION_DELAY;
    Thread::sleep(SIMULATION_DELAY * 1000);
}


void Combact::updateScreen(){
	// Update time
	moveCursor(1, 1);
	printf(ANSI_COLOR_YELLOW "Currency: %d" ANSI_COLOR_RESET "\tTime: %.1f\tSpeed_x: %0.2f\tSpeed_y: %0.2f\tPosition: (%d, %d)\n", currency, time, playerSpeedX, playerSpeedY, (int)playerX, (int)playerY);

    // Update the balls position
    for(int i = 0; i < ballsNumber; i++){
		// Update the ball position if it changed
        if((int)balls[i].lastDrawnX != (int)balls[i].x || (int)balls[i].lastDrawnY != (int)balls[i].y){
        	// Remove the ball from the old position
            writeStringOnScreenPosition((int)balls[i].lastDrawnX + screenStartXPosition, (int)balls[i].lastDrawnY + screenStartYPosition, ANSI_COLOR_RESET " ");

        	// Add the ball to the new position
        	writeStringOnScreenPosition((int)balls[i].x + screenStartXPosition, (int)balls[i].y + screenStartYPosition, ANSI_COLOR_BLUE "O");
            balls[i].lastDrawnX = balls[i].x;
            balls[i].lastDrawnY = balls[i].y;
		}
    }

    // Update the projectiles position
    for(int i = 0; i < 4; i++){
        // Update the projectile position if it changed
        if((int)projectiles[i].lastDrawnX != (int)projectiles[i].x || (int)projectiles[i].lastDrawnY != (int)projectiles[i].y){
        	// Remove the projectile from the old position
            writeStringOnScreenPosition((int)projectiles[i].lastDrawnX + screenStartXPosition, (int)projectiles[i].lastDrawnY + screenStartYPosition, ANSI_COLOR_RESET " ");

        	// Add the projectile to the new position only if is inside the screen (the projectile can be in -1, -1 if it's out of the screen)
            if(projectiles[i].x >= 0 && projectiles[i].x < SCREEN_SIZE && projectiles[i].y >= 0 && projectiles[i].y < SCREEN_SIZE){
        	    writeStringOnScreenPosition((int)projectiles[i].x + screenStartXPosition, (int)projectiles[i].y + screenStartYPosition, ANSI_COLOR_GREEN "*");
                projectiles[i].lastDrawnX = projectiles[i].x;
                projectiles[i].lastDrawnY = projectiles[i].y;
            }
        }
    }

    // Update player position if it changed
	if((int)playerOldX != (int)playerX || (int)playerOldY != (int)playerY){
		// Remove the player from the old position
		writeStringOnScreenPosition((int)playerOldX + screenStartXPosition, (int)playerOldY + screenStartYPosition, ANSI_COLOR_RESET " ");
	}

    // Rewrite always the player position to avoid the player to disappear
    writeStringOnScreenPosition((int)playerX + screenStartXPosition, (int)playerY + screenStartYPosition, ANSI_COLOR_RED "X");

	// Reset the cursor position
	moveCursor(1, 1);

    //Flush
    fflush(stdout);
}

void Combact::printScreen(){
	// Clear the active buffer
	screenString.str("");

    // Fill the buffer with the screen content

    // Add top border
    for(int i = 0; i < SCREEN_SIZE / POSITION_MULTIPLIER + 2; i++){
        // screenString << ANSI_COLOR_RESET "-";
        screenString << ANSI_COLOR_YELLOW "#";
    }
    screenString << "\n";
    
    // Print the screen
    for(int i = 0; i < SCREEN_SIZE / POSITION_MULTIPLIER; i++){
        for(int j = 0; j < SCREEN_SIZE / POSITION_MULTIPLIER; j++){

            // Add left border
            if(j == 0){
                screenString << ANSI_COLOR_YELLOW "#";
            }

            // Add the player
            if(i == (int)(playerY / POSITION_MULTIPLIER) && j == (int)(playerX / POSITION_MULTIPLIER)){
                screenString << ANSI_COLOR_RED "X";
            }else{
                // Print balls and projectiles, projectiles will be printed over the balls
                bool isBall = false;
                for(int k = 0; k < ballsNumber; k++){
                    if(i == (int)(balls[k].y / POSITION_MULTIPLIER) && j == (int)(balls[k].x / POSITION_MULTIPLIER)){
                        screenString << ANSI_COLOR_BLUE "O";
                        balls[k].lastDrawnX = balls[k].x;
                        balls[k].lastDrawnY = balls[k].y;
                        isBall = true;
                        break;
                    }
                }
                if(!isBall){
                    for(int k = 0; k < 4; k++){
                        if(i == (int)(projectiles[k].y / POSITION_MULTIPLIER) && j == (int)(projectiles[k].x / POSITION_MULTIPLIER)){
                            screenString << ANSI_COLOR_GREEN "*";
                            projectiles[k].lastDrawnX = projectiles[k].x;
                            projectiles[k].lastDrawnY = projectiles[k].y;
                            isBall = true;
                            break;
                        }
                    }
                }
                if(!isBall){
                    screenString << " ";
                }
            }

            // Add right border
            if(j == SCREEN_SIZE / POSITION_MULTIPLIER - 1){
                screenString << ANSI_COLOR_YELLOW "#";
            }
        }
        screenString << "\n";
    }
    
    // Add bottom border
    for(int i = 0; i < SCREEN_SIZE / POSITION_MULTIPLIER + 2; i++){
        screenString << ANSI_COLOR_YELLOW "#";
    }
    screenString << "\n";

    // Reset the color
	screenString << ANSI_COLOR_RESET;

    // Reset cursor position
    moveCursor(1, 1);
    
    // Print the time to the console
    printf(ANSI_COLOR_YELLOW "Currency: %d" ANSI_COLOR_RESET "\tTime: %.1f\tSpeed_x: %0.2f\tSpeed_y: %0.2f\tPosition: (%d, %d)\n", currency, time, playerSpeedX, playerSpeedY, (int)playerX, (int)playerY);
    
    // Print the buffer content to the console
    printf("%s", screenString.str().c_str());

    // Flush
    fflush(stdout);
}
