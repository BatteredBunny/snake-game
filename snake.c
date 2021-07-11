#include "raylib.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    //How big the window is
    const int screenWidth = 700;
    const int screenHeight = 500;

    InitWindow(screenWidth, screenHeight, "Snake game");
    InitAudioDevice();  
    SetTargetFPS(60);

    //Apple collection sound effect
    Sound appleCollect = LoadSound("assets/collect.wav"); 

    //Sound that plays when you lose game
    Sound loseSound = LoadSound("assets/boom.wav"); 

    //Arrow icons
    Texture upArrow = LoadTexture("assets/up.png");
    Texture downArrow = LoadTexture("assets/down.png");
    Texture leftArrow = LoadTexture("assets/left.png");
    Texture rightArrow = LoadTexture("assets/right.png");

    //Area where snake can go, some is reserved for UI
    const int playAreaWidth = screenWidth;
    const int playAreaHeight = screenHeight - 40;

    //0 = LEFT
    //1 = RIGHT
    //2 = UP
    //3 = DOWN
    int direction = 1;

    //Snake pixel size
    int snakeSize = 10;
    Vector2 vecSnakeSize = {snakeSize, snakeSize};

    //How many pixels snake moves each time
    int snakeSpeed = 10;

    //Starting tail length
    int snakeTailLength = 5;

    //Vector containing snake head cordinate
    Vector2 snake = { 350, 250 };

    //Vector containing all snake tail pieces
    Vector2 snakeTails[256];

    //Enables walls
    int walls = false;

    //Enables and disables the bottom black bar
    int gameUI = true;

    //Enables showing what keys are being pressed
    int showKeys = true;

    //Game losing state
    int gameLost = false;

    //Counter for swithing lose screen text color
    int lostCount = 0;

    //If enabled shows debug info by default
    int debug = false;

    //Makes sure game displays all parts of tail
    int counterTail = 0;

    //Counts frames used for moving snake every few frames
    int frameCounter = 0;

    //Player score
    int score = 0;

    //Snake colors
    Color snakeColor = { 0, 255, 0, 255 };
    Color snakeAlternateColor = { 0, 186, 52, 255 };

    //Generates random X value
    int genAppleX() {
        int appleX = (rand() % (playAreaWidth/10))*10;
        return appleX;
    }

    //Generates random Y value
    int genAppleY() {
        int appleY = (rand() % (playAreaHeight/10))*10;
        return appleY;
    }

    //Generates new apple location
    Vector2 genApple() {
        Vector2 apple = { genAppleX(), genAppleY() };

        //Makes sure new apple location doesn't collide with snake's tail
        for (int i = 0; i <= snakeTailLength; i++) {
            if (apple.x == snakeTails[i].x && apple.y == snakeTails[i].y) {
                apple.x = genAppleX();
                apple.y = genAppleY();
            }
        }

        return apple;
    }

    Color appleColor = { 211, 38, 41, 255 }; 
    Vector2 apple = genApple();

    //Function triggered when you lose the game
    int loseGame() {
        if (gameLost == false) {
            gameLost = true;
            PlaySound(loseSound);
        }
    }

    while (!WindowShouldClose())
    {
        int time = GetTime();

        //Adds current snake head to tail
        snakeTails[counterTail] = snake;

        //Toggle debug menu
        if (GetKeyPressed() == 68) {
            debug = !debug;
        }

        //Out of bounds
        if (walls) {
            if (((int)snake.x > playAreaWidth - snakeSize) || ((int)snake.x < 0) || ((int)snake.y < 0) || ((int)snake.y > playAreaHeight - snakeSize)) {
                loseGame();
            } 
        } else {
            if ((int)snake.x > playAreaWidth - snakeSize) {
                snake.x = ((int)snake.x % playAreaWidth);
            } 
            if ((int)snake.x < 0) {
                snake.x = playAreaWidth - abs((int)snake.x);
            } 
            if ((int)snake.y < 0) {
                snake.y = playAreaHeight - abs((int)snake.y);
            } 
            if ((int)snake.y > playAreaHeight - snakeSize) {
                snake.y = ((int)snake.y - playAreaHeight);
            }

        }	

        if (!gameLost) {

            //Checks if player is on apple
            if (apple.x == snake.x && apple.y == snake.y) {

                //Generates new cords for apple
                apple = genApple();

                score++;

                //Max tail length is 256
                if (snakeTailLength != 256) {
                    snakeTailLength++;
                }
            
                PlaySound(appleCollect);
            }

            //Arrow key controls
            if (IsKeyPressed(KEY_LEFT) && direction != 1) {
                direction = 0;
            } else if (IsKeyPressed(KEY_RIGHT) && direction != 0) {
                direction = 1;
            } else if (IsKeyPressed(KEY_UP) && direction != 3) {
                direction = 2;
            } else if (IsKeyPressed(KEY_DOWN) && direction != 2) {
                direction = 3;
            }

            //Moves player every 2 frames
            if (frameCounter >= 2) {
                //Directons
                if (direction == 0) {
                    snake.x -= snakeSpeed;
                } else if (direction == 1) {
                    snake.x += snakeSpeed;
                } else if (direction == 2) {
                    snake.y -= snakeSpeed;
                } else if (direction == 3) {
                    snake.y += snakeSpeed;
                }

                //Makes sure it renders all snake tail parts
                if (counterTail >= snakeTailLength) {
                    counterTail = 0;
                } else {
                    counterTail += 1;
                }

                //Checks collision with tails
                for (int i = 1; i <= snakeTailLength; i++) {
                    if (snake.x == snakeTails[i].x && snake.y == snakeTails[i].y) {
                        loseGame();
                    }
                }

                frameCounter = 0;
            } else {
                frameCounter++;
            }
        }

        //Starts the drawing process
        BeginDrawing();
        ClearBackground(RAYWHITE);

        //Draws all snake tail parts
        for (int i = 0; i <= snakeTailLength; i++) {
            if (i % 2 == 0) {
                DrawRectangleV(snakeTails[i], vecSnakeSize, snakeAlternateColor);
            } else {
                DrawRectangleV(snakeTails[i], vecSnakeSize, snakeColor);
            }
        }

        //Draws apples on screen
        DrawRectangleV(apple, vecSnakeSize, appleColor);

        if (gameUI) {
            //Draws black box at bottom containing score
            DrawRectangle(0, playAreaHeight, screenWidth, screenHeight - playAreaHeight, BLACK);

            //Draws score
            char scoreChar[20];
            if (!gameLost) {
                sprintf(scoreChar, "%d", score);
                DrawText(scoreChar, screenWidth/2, playAreaHeight+5, 30, WHITE);
            } else {

                Color final = BLACK;

                //Cycles between red and black every 20 frames
                if (lostCount >= 40) {
                    final = BLACK;
                    lostCount = 0;
                } else if (lostCount >= 20) {
                    final = RED;
                }        

                //Shows final score on screen
                sprintf(scoreChar, "Final score: %d", score);
                DrawText(scoreChar, 100, playAreaHeight/3, 30, final);

                lostCount++;
            }

            //Shows what key is being held down in bottom left corner
            if (showKeys) {
                if (IsKeyDown(KEY_LEFT)) {
                    DrawTexture(leftArrow, 40, playAreaHeight, RAYWHITE);
                } 

                if (IsKeyDown(KEY_RIGHT)) {
                    DrawTexture(rightArrow, 60, playAreaHeight, RAYWHITE);
                } 

                if (IsKeyDown(KEY_UP)) {
                    DrawTexture(upArrow, 0, playAreaHeight, RAYWHITE);
                } 

                if (IsKeyDown(KEY_DOWN)) {
                    DrawTexture(downArrow, 20, playAreaHeight, RAYWHITE);
                }

            }
        }
        if (debug) {
            //Debug info showing snake cords
            char snakeCords[20];
            sprintf(snakeCords, "snake: %dX %dY", (int)snake.x, (int)snake.y);
            DrawText(snakeCords, 5, 45, 20, BLACK);

            //Debug info showing apple cords
            char appleCords[20];
            sprintf(appleCords, "apple: %dX %dY", (int)apple.x, (int)apple.y);
            DrawText(appleCords, 5, 65, 20, BLACK);

            //Debug info showing time played
            char timeChar[10];
            sprintf(timeChar, "time: %d", time);
            DrawText(timeChar, 5, 25, 20, BLACK);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

