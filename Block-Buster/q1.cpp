#include<SFML\Graphics.hpp>
#include<iostream>
#include<fstream>
void startGame(bool fresh) {
	if (fresh) {
		loadlevel();//member 1 feature
		score = 0;
		lives = 3;
	}
	else if (!loadsave()) {
		noSavetimer = 120;
		return;
	}
	paddle_x = 250;
	paddle_w = normal_w;
	wideTimer = 0;
	powerup_active = false;
	gameWon = false;
	gameState = PLAYING;
}
void endGame(bool won) {
	gameWon = won;
	gameState = Game_Over;
	clearSave();
}
bool noBricksleft() {
	for (int r = 0;r < rows;r++) {
		for (int c = 0;c < cols;c++)
			if (bricks[r][c] > 0)
				return false;
		return true;
	}
}
	