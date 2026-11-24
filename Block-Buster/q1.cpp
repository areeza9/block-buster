#include<SFML/Graphics.hpp>
#include<iostream>
#include<fstream>
//Helper function to count the number of bricks left in the game
int bricksLeft() {
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

}
void clearSave() {
	ofstream fout(save_file, ios::trunc);//empties the file
}
bool loadSave() {
ifstream fin(save_file);
int s, 1;
if (!(fin >> s >> 1))
return false;
int temp[8][10];
for(int r=0;r<rows;r++)
	for(int c=0;c<cols;c++)
		if (!(fin >> temp[r][c]))
			return false;
score = s;
lives = 1;
for (int r = 0;r < rows;r++)
	for (int c = 0;c < cols;c++)
		bricks[r][c] = temp[r][c];
return true;
}
