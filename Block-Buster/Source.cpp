
#include <SFML/Graphics.hpp>
#include <fstream>
#include <cstdlib>
#include <string>
using namespace std;

// team constants
const int WIN_W = 600, WIN_H = 700, ROWS = 8, COLS = 10;
const int BRICK_W = 60, BRICK_H = 25, BRICKS_Y = 100;
const int PADDLE_W = 100, PADDLE_H = 14, PADDLE_Y = 650, PADDLE_START_X = 250;
const int BALL_SIZE = 12, BALL_START_Y = 638, START_LIVES = 3;

// team variables
extern int bricks[ROWS][COLS];
extern int score, lives, paddleX, paddleW, ballX, ballY, dx, dy;
extern bool ballLaunched;
bool loadLevel();

// my constants
const int MENU = 0, PLAYING = 1, GAME_OVER = 2;          // game states
const int NEW_GAME = 0, CONTINUE = 1, EXIT = 2;          // menu lines
const int POWER_W = 1, POWER_LIFE = 2;                   // power-up types
const int POWER_SIZE = 20, POWER_SPEED = 3, WIDE_W = 150, WIDE_FRAMES = 600;

// my variables 
int gameState = MENU;
int menuChoice = NEW_GAME;
int noSaveTimer = 0;            // frames to show "No saved game"
bool playerWon = false;
bool puActive = false;          // is a power-up falling?
int puType = 0, puX = 0, puY = 0;
int wideTimer = 0;              // frames left of the wide paddle
// feature 9
//BONUS: POWER-UPS
void resetPowerUps()
{
    puActive = false;
    wideTimer = 0;
    paddleW = PADDLE_W;
}

void maybeSpawnPowerUp(int row, int col)    // call when a brick BREAKS
{
    if (puActive) return;                   // only one at a time
    if (rand() % 5 != 0) return;            // 1 in 5 chance
    puActive = true;
    puType = (rand() % 2 == 0) ? POWER_W : POWER_LIFE;
    puX = col * BRICK_W + 20;
    puY = BRICKS_Y + row * BRICK_H;
}

void updatePowerUp()                        // every frame while PLAYING
{
    if (wideTimer > 0)
    {
        wideTimer--;
        if (wideTimer == 0) paddleW = PADDLE_W;
    }
    if (!puActive) return;

    puY += POWER_SPEED;                     // fall 3 px
    bool caught = puX + POWER_SIZE > paddleX && puX < paddleX + paddleW &&
        puY + POWER_SIZE > PADDLE_Y && puY < PADDLE_Y + PADDLE_H;
    if (caught)
    {
        if (puType == POWER_W)
        {
            paddleW = WIDE_W;
            wideTimer = WIDE_FRAMES;
            if (paddleX + paddleW > WIN_W) paddleX = WIN_W - paddleW;
        }
        else
            lives++;
        puActive = false;
    }
    else if (puY > WIN_H)
        puActive = false;
}

// drawing helper 
void drawText(sf::RenderWindow& window, const sf::Font& font, string s, float x, float y, int size, sf::Color color)
{
    sf::Text text(font, s, size);
    text.setFillColor(color);
    text.setPosition({ x, y });
    window.draw(text);
}

void drawPowerUp(sf::RenderWindow& window, const sf::Font& font)
{
    if (!puActive) return;
    sf::RectangleShape box({ (float)POWER_SIZE, (float)POWER_SIZE });
    box.setPosition({ (float)puX, (float)puY });
    box.setFillColor(puType == POWER_W ? sf::Color(0, 200, 255) : sf::Color(255, 90, 160));
    window.draw(box);
    drawText(window, font, puType == POWER_W ? "W" : "+", puX + 4.0f, puY, 16, sf::Color::Black);
}

// FEATURE 8: SAVE / LOAD 
void saveGame()
{
    ofstream out("savegame.txt");
    out << score << " " << lives << endl;
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
            out << bricks[r][c] << " ";
        out << endl;
    }
}

bool loadGame()          
{
    ifstream in("savegame.txt");
    if (!(in >> score >> lives)) return false;      // missing or empty file
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            in >> bricks[r][c];
    return true;
}

void clearSave()
{
    ofstream out("savegame.txt");  
}

//  FEATURE 7: STATES AND MENU 
void resetBallAndPaddle()
{
    paddleX = PADDLE_START_X;
    ballLaunched = false;
    dx = 0;
    dy = 0;
    ballX = paddleX + (paddleW - BALL_SIZE) / 2;
    ballY = BALL_START_Y;
}

void startNewGame()
{
    score = 0;
    lives = START_LIVES;
    resetPowerUps();
    resetBallAndPaddle();
    if (loadLevel())                // Member 1's function
        gameState = PLAYING;
}

void startContinue()
{
    if (loadGame())
    {
        resetPowerUps();
        resetBallAndPaddle();
        gameState = PLAYING;
    }
    else
        noSaveTimer = 120;          // show "No saved game" for 2 seconds
}

void endGame(bool won)              // Member 2 calls this
{
    playerWon = won;
    clearSave();
    resetPowerUps();
    gameState = GAME_OVER;
}

void handleStateKey(sf::Keyboard::Key key, sf::RenderWindow& window)
{
    switch (gameState)
    {
    case MENU:
        if (key == sf::Keyboard::Key::Up)   menuChoice = (menuChoice + 2) % 3;
        if (key == sf::Keyboard::Key::Down) menuChoice = (menuChoice + 1) % 3;
        if (key == sf::Keyboard::Key::Enter)
        {
            if (menuChoice == NEW_GAME) startNewGame();
            if (menuChoice == CONTINUE) startContinue();
            if (menuChoice == EXIT)     window.close();
        }
        break;
    case PLAYING:
        if (key == sf::Keyboard::Key::Escape)
        {
            saveGame();
            gameState = MENU;
        }
        break;
    case GAME_OVER:
        if (key == sf::Keyboard::Key::Enter)
            gameState = MENU;
        break;
    }
}

void updateState()                  // call once per frame
{
    if (gameState == MENU && noSaveTimer > 0) noSaveTimer--;
    if (gameState == PLAYING) updatePowerUp();
}

void drawMenu(sf::RenderWindow& window, const sf::Font& font)
{
    drawText(window, font, "BLOCK BUSTER", 130, 120, 56, sf::Color::White);
    string names[3] = { "New Game", "Continue", "Exit" };
    for (int i = 0; i < 3; i++)
    {
        sf::Color color = (i == menuChoice) ? sf::Color::Yellow : sf::Color::White;
        drawText(window, font, names[i], 220, 300.0f + i * 60, 32, color);
    }
    if (noSaveTimer > 0)
        drawText(window, font, "No saved game", 200, 520, 26, sf::Color::Red);
}

void drawEndScreen(sf::RenderWindow& window, const sf::Font& font)
{
    if (playerWon) drawText(window, font, "YOU WIN", 190, 230, 64, sf::Color::Green);
    else           drawText(window, font, "GAME OVER", 150, 230, 64, sf::Color::Red);
    drawText(window, font, "Final score: " + to_string(score), 190, 330, 30, sf::Color::White);
    drawText(window, font, "Press Enter to return to the menu", 120, 420, 22, sf::Color::White);
}
