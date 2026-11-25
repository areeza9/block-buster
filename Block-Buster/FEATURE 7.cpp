#include <SFML/Graphics.hpp>
#include <string>
using namespace std;

const int MENU = 0, PLAYING = 1, GAME_OVER = 2;   // the 3 screens
const int NEW_GAME = 0, CONTINUE = 1, EXIT = 2;   // the 3 menu lines

int gameState = MENU;
int menuChoice = NEW_GAME;       // which menu line is highlighted
int noSaveTimer = 0;             // frames left to show "No saved game"
bool playerWon = false;
extern int score, lives, paddleX, paddleW, ballX, ballY, dx, dy;
bool ballLaunched;
const int  PADDLE_START_X = 250;
const int BALL_SIZE = 12, BALL_START_Y = 638, START_LIVES = 3;

bool loadLevel();                // Member 1
void saveGame();                 // Feature 8
bool loadGame();
void clearSave();
void resetPowerUps();            // Bonus

void drawText(sf::RenderWindow& window, const sf::Font& font, string s, float x, float y, int size, sf::Color color)
{
    sf::Text text(font, s, size);
    text.setFillColor(color);
    text.setPosition({ x, y });
    window.draw(text);
}

void resetBallAndPaddle()        // ball goes back on the middle of the paddle
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
    if (loadLevel()) gameState = PLAYING;
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
        noSaveTimer = 120;       // show "No saved game" for 2 seconds
}

void endGame(bool won)           // Member 2 calls this: true = win, false = lose
{
    playerWon = won;
    clearSave();                 // a finished game cannot be continued
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
        if (key == sf::Keyboard::Key::Enter) gameState = MENU;
        break;
    }
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
    {
        drawText(window, font, "No saved game", 200, 520, 26, sf::Color::Red);
        noSaveTimer--;           // counts down once per drawn frame
    }
}

void drawEndScreen(sf::RenderWindow& window, const sf::Font& font)
{
    if (playerWon) drawText(window, font, "YOU WIN", 190, 230, 64, sf::Color::Green);
    else           drawText(window, font, "GAME OVER", 150, 230, 64, sf::Color::Red);
    drawText(window, font, "Final score: " + to_string(score), 190, 330, 30, sf::Color::White);
    drawText(window, font, "Press Enter to return to the menu", 120, 420, 22, sf::Color::White);
}