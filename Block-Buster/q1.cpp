#include<SFML/Graphics.hpp>
#include<iostream>
#include<fstream>{
const int MENU = 0;
const int PLAYING = 1;
const int GAME_OVER = 2;
const int NEW_GAME = 0;
const int CONTINUE_GAME = 1;
const int EXIT_GAME = 2;
int gamestate = MENU;
int menuChoice = 0;
int noSavetimer = 0;
bool gameWon = false;
sf::Font font;
int wideTimer = 0;
void resetRound() {
	paddle_x = 250;
	paddle_w = normal_paddle_w;
	wideTimer = 0;
	powerup_active = false;
	gameWon = false;
}
void startGame() {
	loadlevel();
	score = 0;lives = 3;
	resetRound();
	gameState = PLAYING;
}
void continueGame() {
	if (!loadsave()) {
		noSavetimer = 120;
		return;
	}
	resetRound();
	gamestate = PLAYING;
}
void endGame(bool won) {
	gameWon = won;
	gamestate = GAME_OVER;
	clearSave();//game finished
}
void checkGameOver() {
	if (lives <= 0) {
		endGame(false);
	}
	else if (bricksLeft()==0) {
		endGame(true);
	}
}