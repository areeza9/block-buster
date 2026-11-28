#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <optional>
#include <string>
using namespace std;
const string save_file = "save.txt";
int bricks[8][10];
const int rows = 8;
const int cols = 10;
const int brick_w = 60;
const int brick_h = 25;
const int bricks_y = 100;
void handleInput(sf::RenderWindow& window)
{
	while (const std::optional event = window.pollEvent())
		if (event->is<sf::Event::Closed>())
			window.close();
}
void updateGame()
{

}
void drawBricks(sf::RenderWindow& window)
{
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			if (bricks[r][c] == 0)
				continue;
			sf::RectangleShape brick({ brick_w - 2,brick_h - 2 });
			brick.setPosition({ (float)(c * brick_w + 1),(float)(bricks_y + r * brick_h + 1) });
			if (bricks[r][c] == 1)
				brick.setFillColor(sf::Color::Green);
			else if (bricks[r][c] == 2)
				brick.setFillColor(sf::Color::Yellow);
			else
				brick.setFillColor(sf::Color::Red);
			window.draw(brick);
		}
	}
}
void drawGame(sf::RenderWindow& window)
{
	window.clear();
	drawBricks(window);
	window.display();
}
bool loadLevel()
{
	ifstream fin("level.txt");
	if (!fin.is_open())
	{
		cout << "level.txt not found";
		return false;
	}
	int rows, cols;
	fin >> rows >> cols;
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			fin >> bricks[r][c];
		}
	}
	return true;
}
const int WIN_W = 600;
const int WIN_H = 700;
const int HUD_H = 50;
const int PADDLE_W = 100;
const int PADDLE_H = 14;
const int PADDLE_START_X = 250;
const int PADDLE_Y = 650;
const int PADDLE_SPEED = 8;
const int BALL_SIZE = 12;     // 12x12 box 
const int BALL_OFFSET_X = 44;    // ballX = paddleX + 44 while held
const int BALL_START_Y = 638;    // ballY = 638 while held (sits on paddle)
const int LAUNCH_DY = -5;
const int LAUNCH_DX = 3;      // random -3 or +3
const int START_LIVES = 3;
const int BALL_LOST_Y = 700;    // ballY > 700 => life lost
const int ZONE_DX[4] = { -6, -3, 3, 6 };
const int ZONE_W = PADDLE_W / 4;   // 25 px per zone
// for checkWinLose()
const int RESULT_NONE = 0;
const int RESULT_WIN = 1;
const int RESULT_GAME_OVER = 2;
//fatima the names u need
int  paddleX = PADDLE_START_X;
int  ballX = PADDLE_START_X + BALL_OFFSET_X;
int  ballY = BALL_START_Y;
int  ballDX = 0;
int  ballDY = 0;
bool ballLaunched = false;
int  score = 0;
int  lives = START_LIVES;
int  bricksLeft = 80;
//feature 3:
void resetBallOnPaddle()
{
	ballLaunched = false;
	ballDX = 0;
	ballDY = 0;
	ballX = paddleX + BALL_OFFSET_X;
	ballY = BALL_START_Y;
}
void resetPaddleBallHUD()
{
	paddleX = PADDLE_START_X;
	score = 0;
	lives = START_LIVES;
	resetBallOnPaddle();
}
// If the ball has not been launched...it rides on the paddle.
void movePaddle()
{
	bool left = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
	bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);

	if (left)  paddleX -= PADDLE_SPEED;
	if (right) paddleX += PADDLE_SPEED;

	// Clamp so  paddle never leaves the window
	if (paddleX < 0)               paddleX = 0;
	if (paddleX > WIN_W - PADDLE_W) paddleX = WIN_W - PADDLE_W;

	// Ball stays on paddle centre before launch
	if (!ballLaunched)
	{
		ballX = paddleX + BALL_OFFSET_X;
		ballY = BALL_START_Y;
	}
}

// Launch paddle on Space
void launchBall()
{
	if (!ballLaunched && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
	{
		ballLaunched = true;
		ballDY = LAUNCH_DY;
		ballDX = (rand() % 2 == 0) ? -LAUNCH_DX : LAUNCH_DX;
	}
}
bool brickAt(int cx, int cy, int& r, int& c)
{
	if (cy < bricks_y || cy >= bricks_y + rows * brick_h || cx < 0 || cx >= cols * brick_w)
		return false;
	r = (cy - bricks_y) / brick_h;
	c = cx / brick_w;
	return bricks[r][c] > 0;
}
void hitBrick(int r, int c)
{
	bricks[r][c] = bricks[r][c] - 1;
	if (bricks[r][c] == 0) { score += 50; bricksLeft -= 1; }
	else score += 10;
}
bool ballHitsBrick()
{
	int r, c; int lastR = -1, lastC = -1; bool hit = false;
	int cornerX[4] = { ballX, ballX + BALL_SIZE - 1, ballX, ballX + BALL_SIZE - 1 };
	int cornerY[4] = { ballY, ballY, ballY + BALL_SIZE - 1, ballY + BALL_SIZE - 1 };
	for (int i = 0; i <= 3; i++)
		if (brickAt(cornerX[i], cornerY[i], r, c))
		{
			hit = true;
			if (r != lastR || c != lastC) { hitBrick(r, c); lastR = r; lastC = c; }
		}
	return hit;
}
//feature 4: Move the ball in TWO separate steps (x first, then y) so wall bounces
void moveBall()
{
	if (!ballLaunched) return;

	// horizontal move + left/right wall bounce
	ballX += ballDX;
	if (ballHitsBrick())
	{
		ballX -= ballDX;
		ballDX = -ballDX;
	}
	if (ballX < 0)
	{
		ballX = 0;
		ballDX = -ballDX;
	}
	else if (ballX + BALL_SIZE > WIN_W)
	{
		ballX = WIN_W - BALL_SIZE;
		ballDX = -ballDX;
	}

	//vertical move + HUD boundary (y = 50) bounce
	ballY += ballDY;
	if (ballHitsBrick())
	{
		ballY -= ballDY;
		ballDY = -ballDY;
	}
	if (ballY < HUD_H)
	{
		ballY = HUD_H;
		ballDY = -ballDY;
	}
}
void bounceOnPaddle()
{
	if (!ballLaunched || ballDY <= 0) return;
	// Box overlap test btwn ball and paddle
	bool overlapX = (ballX + BALL_SIZE > paddleX) && (ballX < paddleX + PADDLE_W);
	bool overlapY = (ballY + BALL_SIZE > PADDLE_Y) && (ballY < PADDLE_Y + PADDLE_H);
	if (overlapX && overlapY)
	{
		int hitPos = (ballX + BALL_SIZE / 2) - paddleX;
		int zone = hitPos / ZONE_W;
		if (zone < 0) zone = 0;
		if (zone > 3) zone = 3;

		ballDX = ZONE_DX[zone];
		ballDY = -ballDY;                    // now moving up
		ballY = PADDLE_Y - BALL_SIZE;       // sit on top so it can't re-hit
	}
}
//feature 6:
void checkLifeLoss()
{
	if (ballLaunched && ballY > BALL_LOST_Y)
	{
		lives--;
		resetBallOnPaddle();
	}
}

// Returns RESULT_WIN (0 bricks), RESULT_GAME_OVER (0 lives) or RESULT_NONE.
// Abbass switches gameState to GAME_OVER when this is not RESULT_NONE.
int checkWinLose()
{
	if (bricksLeft <= 0) return RESULT_WIN;
	if (lives <= 0)      return RESULT_GAME_OVER;
	return RESULT_NONE;
}
//drawwww
void drawPaddle(sf::RenderWindow& window)
{
	sf::RectangleShape paddle(sf::Vector2f((float)PADDLE_W, (float)PADDLE_H));
	paddle.setPosition(sf::Vector2f((float)paddleX, (float)PADDLE_Y));
	paddle.setFillColor(sf::Color(70, 160, 255));
	window.draw(paddle);
}

void drawBall(sf::RenderWindow& window)
{
	sf::CircleShape ball((float)BALL_SIZE / 2.f);   // radius 6 -> 12x12 box
	ball.setPosition(sf::Vector2f((float)ballX, (float)ballY));
	ball.setFillColor(sf::Color::White);
	window.draw(ball);
}
void drawHUD(sf::RenderWindow& window, const sf::Font& font)
{
	sf::RectangleShape strip(sf::Vector2f((float)WIN_W, (float)HUD_H));
	strip.setFillColor(sf::Color(30, 30, 45));
	window.draw(strip);

	sf::RectangleShape line(sf::Vector2f((float)WIN_W, 2.f));   // boundary at y = 50
	line.setPosition(sf::Vector2f(0.f, (float)HUD_H - 2.f));
	line.setFillColor(sf::Color(120, 120, 150));
	window.draw(line);

	sf::Text scoreText(font, "Score: " + std::to_string(score), 24);
	scoreText.setFillColor(sf::Color::White);
	scoreText.setPosition(sf::Vector2f(15.f, 10.f));
	window.draw(scoreText);

	sf::Text livesText(font, "Lives: " + std::to_string(lives), 24);
	livesText.setFillColor(sf::Color::White);
	float w = livesText.getLocalBounds().size.x;
	livesText.setPosition(sf::Vector2f((float)WIN_W - w - 15.f, 10.f));
	window.draw(livesText);
}

// Centered "YOU WIN" / "GAME OVER" message (Abbass adds "Press Enter").
void drawEndMessage(sf::RenderWindow& window, const sf::Font& font, int result)
{
	if (result == RESULT_NONE) return;

	std::string msg = (result == RESULT_WIN) ? "YOU WIN" : "GAME OVER";
	sf::Color col = (result == RESULT_WIN) ? sf::Color::Green : sf::Color::Red;

	sf::Text text(font, msg, 56);
	text.setFillColor(col);
	sf::FloatRect b = text.getLocalBounds();
	text.setPosition(sf::Vector2f((WIN_W - b.size.x) / 2.f - b.position.x,
		(WIN_H - b.size.y) / 2.f - b.position.y));
	window.draw(text);
}
//Helper function to count the number of bricks left in the game
int countBricksLeft() {
	int count = 0;
	for (int r = 0;r < rows;r++)
		for (int c = 0;c < cols;c++)
			if (bricks[r][c] > 0)
				count++;
	return count;

}
//feature 8:
void savegame() {
	ofstream fout(save_file);
	fout << score << " " << lives << "\n";
	for (int r = 0;r < rows;r++) {
		for (int c = 0;c < cols;c++) {
			fout << bricks[r][c] << " ";
		}
		fout << "\n";
	}
	fout.close();
}

		int main()
		{
			sf::RenderWindow window(sf::VideoMode({ 600u,700u }), "Block Buster");
			window.setFramerateLimit(60);
			srand((unsigned)time(0));
			sf::Font font;
			if (!font.openFromFile("font.ttf"))
				cout << "font.ttf not found";
			bool levelOk = loadLevel();
			int result = RESULT_NONE;
			resetPaddleBallHUD();

			while (window.isOpen())
			{
				handleInput(window);

				if (!levelOk)
				{
					window.clear();
					sf::Text msg(font, "level.txt not found!", 30);
					msg.setFillColor(sf::Color::Red);
					msg.setPosition({ 100.f, 300.f });
					window.draw(msg);
					window.display();
					continue;
				}
				if (result == RESULT_NONE)
				{
					movePaddle();
					launchBall();
					moveBall();
					bounceOnPaddle();
					checkLifeLoss();
					result = checkWinLose();
				}

				window.clear();
				drawBricks(window);
				drawPaddle(window);
				drawBall(window);
				drawHUD(window, font);
				drawEndMessage(window, font, result);
				window.display();
			}
			return 0;
		}
