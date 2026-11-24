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