#include <SFML/Graphics.hpp>
#include <fstream>
#include <cstdlib>
#include <string>
using namespace std;
#include <fstream>
using namespace std;
//feature 8: SAVE / LOAD
const int ROWS = 8, COLS = 10;          // team constants
extern int bricks[ROWS][COLS];          // team variables
const int BRICK_W = 60, BRICK_H = 25, BRICKS_Y = 100;   // team constants
const int PADDLE_W = 100, PADDLE_H = 14, PADDLE_Y = 650;
extern int paddleX, paddleW, score, lives;
const int WIN_W = 600, WIN_H = 700;

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

bool loadGame() {
    ifstream in("savegame.txt");
    if (!(in >> score >> lives))         // file missing or empty
        return false;
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            in >> bricks[r][c];
    return true;
}

void clearSave()
{
    ofstream out("savegame.txt");
}
//bonus: POWER-UPS

const int POWER_W = 1, POWER_LIFE = 2;     // two kinds of box
const int POWER_SIZE = 20, POWER_SPEED = 3;
const int WIDE_W = 150, WIDE_FRAMES = 600; // 600 frames = 10 seconds

bool puActive = false;                     // is a box falling now?
int puType = 0, puX = 0, puY = 0;
int wideTimer = 0;                         // frames left of the wide paddle



void resetPowerUps()
{
    puActive = false;
    wideTimer = 0;
    paddleW = PADDLE_W;
}

void maybeSpawnPowerUp(int row, int col)   // Member 1 calls this when a brick breaks
{
    if (puActive) return;                  // only one at a time
    if (rand() % 5 != 0) return;           // 1 in 5 chance
    puActive = true;
    puType = (rand() % 2 == 0) ? POWER_W : POWER_LIFE;
    puX = col * BRICK_W + 20;
    puY = BRICKS_Y + row * BRICK_H;
}

void updatePowerUp()                       // every frame while playing
{
    if (wideTimer > 0)                     // wide paddle countdown
    {
        wideTimer--;
        if (wideTimer == 0) paddleW = PADDLE_W;
    }
    if (!puActive) return;

    puY += POWER_SPEED;                    // fall 3 px
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
    else if (puY > WIN_H)                  // missed it
        puActive = false;
}

void drawPowerUp(sf::RenderWindow& window, const sf::Font& font)
{
    if (!puActive) return;
    sf::RectangleShape box({ (float)POWER_SIZE, (float)POWER_SIZE });
    box.setPosition({ (float)puX, (float)puY });
    box.setFillColor(puType == POWER_W ? sf::Color(0, 200, 255) : sf::Color(255, 90, 160));
    window.draw(box);

    sf::Text letter(font, puType == POWER_W ? "W" : "+", 16);
    letter.setFillColor(sf::Color::Black);
    letter.setPosition({ puX + 4.0f, (float)puY });
    window.draw(letter);
}