#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
using namespace std;
int bricks[8][10];
const int rows = 8;
const int cols = 10;
const int brick_w = 60;
const int brick_h = 25;
const int bricks_y = 100;
const int BALL_SIZE = 12;
int ballX = 300, ballY = 400, ballDX = 3, ballDY = -5, score = 0, bricksLeft = 80;
void handleInput(sf::RenderWindow& window)
{
	while (const std::optional event = window.pollEvent())
		if (event->is<sf::Event::Closed>())
			window.close();
}
bool brickAt(int cx, int cy, int& r, int& c)
{
	if (cy < bricks_y || cy >= bricks_y + rows * brick_h || cx < 0 || cx >= cols * brick_w)
		return false;
	r = (cy - bricks_y) / brick_h;
	c = cx / brick_w;
	return bricks[r][c]>0;
}
void hitBrick(int r, int c)
{
	bricks[r][c] = bricks[r][c] - 1;
	if (bricks[r][c] == 0)
	{
		score += 50;
		bricksLeft -= 1;
	}
	else
		score += 10;
}
bool ballHitsBrick()
{
	int r, c;
	int lastR = -1, lastC = -1;
	bool hit = false;
	int cornerX[4] = { ballX,ballX + BALL_SIZE - 1,ballX,ballX + BALL_SIZE - 1 };
	int cornerY[4] = { ballY,ballY,ballY + BALL_SIZE - 1,ballY + BALL_SIZE - 1 };
	for (int i = 0; i <= 3;i++)
	{
		if (brickAt(cornerX[i], cornerY[i], r, c))
		{
			hit = true;
			if (r != lastR || c != lastC)
			{
				hitBrick(r, c);
				lastR = r;
				lastC = c;
			}
		}

	}
	return hit;
}
void updateGame()
{
	ballX += ballDX;
	if (ballHitsBrick())
	{
		ballX -= ballDX;
		ballDX = -ballDX;
	}
	ballY += ballDY;
	if (ballHitsBrick())
	{
		ballY -= ballDY;
		ballDY = -ballDY;
	}
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
	sf::RectangleShape ball({ (float)BALL_SIZE,(float)BALL_SIZE });
	ball.setPosition({ (float)ballX,(float)ballY });
	ball.setFillColor(sf::Color::White);
	window.draw(ball);
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
int main()
{
	sf::RenderWindow window(sf::VideoMode({ 600,700 }), "Block Buster");
	window.setFramerateLimit(60);
	if (!loadLevel())
		return 1;
	while (window.isOpen())
	{
		handleInput(window);
		updateGame();
		drawGame(window);
	}
}