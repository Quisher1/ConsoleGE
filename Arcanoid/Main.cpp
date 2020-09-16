#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include "GameEngine.h"
using namespace std;

class GameObject {
public:
	float positionX=0, positionY=0;
	float sizeX=0, sizeY=0;
};


class PlayerPlatform : public GameObject {
public:
	float limX, limY;
	float speed;
	PlayerPlatform(float x, float y, float sizeX, float sizeY, int ScreenWidth, int ScreenHeight, float speed = 1) {
		positionX = x;
		positionY = y;
		this->sizeX = sizeX;
		this->sizeY = sizeY;
		limX = ScreenWidth;
		limY = ScreenHeight;

		this->speed = speed;
	}

	void MoveXCoord() {
		if (GetAsyncKeyState(short int('A')) & 0x8000) {
			if(positionX - sizeX / 2. > -limX/2.)
				positionX -= speed;
		}
		if (GetAsyncKeyState(short int('D')) & 0x8000) {
			if (positionX + sizeX / 2. < limX / 2. - 1)
				positionX += speed;
		}
	}
	void MoveYCoord() {
		if (GetAsyncKeyState(short int('S')) & 0x8000) {
			if (positionY - sizeY / 2. > -limY / 2.)
				positionY -= speed;
		}
		if (GetAsyncKeyState(short int('W')) & 0x8000) {
			if (positionY + sizeY / 2. < limY / 2. - 1)
				positionY += speed;
		}
	}
};

class Ball : public GameObject {
public:
	float speedX, speedY;
	float limX, limY;
	Ball(float x, float y, int ScreenWidth, int ScreenHeight, float sizeX, float sizeY, float speedX = 0.25, float speedY = 0.25) {
		this->sizeX = sizeX;
		this->sizeY = sizeY;
		positionX = x;
		positionY = y;
		limX = ScreenWidth;
		limY = ScreenHeight;
		this->speedX = speedX;
		this->speedY = speedY;
	}
	void Move() {
		if (positionX + speedX < limX / 2 && positionX + speedX > -limX / 2) positionX += speedX;
		else speedX *= -1;

		if (positionY + speedY < limY / 2 && positionY + speedY > -limY / 2) positionY += speedY;
		else speedY *= -1;
	}
};


class Tile : public GameObject {
public:
	float Hp = 5;
	bool isAlive = true;
	Tile(float x, float y, float sizeX, float sizeY) {
		positionX = x;
		positionY = y;
		this->sizeX = sizeX;
		this->sizeY = sizeY;
	}
};

bool collision(GameObject A, GameObject B) {
	if (A.positionX + A.sizeX / 2. >= B.positionX - B.sizeX / 2. && A.positionX + A.sizeX / 2. <= B.positionX + B.sizeX / 2. &&
		A.positionY + A.sizeY / 2. >= B.positionY - B.sizeY / 2. && A.positionY + A.sizeY / 2. <= B.positionY + B.sizeY / 2. ||
		
		A.positionX - A.sizeX / 2. >= B.positionX - B.sizeX / 2. && A.positionX - A.sizeX / 2. <= B.positionX + B.sizeX / 2. &&
		A.positionY + A.sizeY / 2. >= B.positionY - B.sizeY / 2. && A.positionY + A.sizeY / 2. <= B.positionY + B.sizeY / 2. ||
		
		A.positionX + A.sizeX / 2. >= B.positionX - B.sizeX / 2. && A.positionX + A.sizeX / 2. <= B.positionX + B.sizeX / 2. &&
		A.positionY - A.sizeY / 2. >= B.positionY - B.sizeY / 2. && A.positionY - A.sizeY / 2. <= B.positionY + B.sizeY / 2. ||
		
		A.positionX - A.sizeX / 2. >= B.positionX - B.sizeX / 2. && A.positionX - A.sizeX / 2. <= B.positionX + B.sizeX / 2. &&
		A.positionY - A.sizeY / 2. >= B.positionY - B.sizeY / 2. && A.positionY - A.sizeY / 2. <= B.positionY + B.sizeY / 2.) {
		return true;
	}return false;
}


int main() {
	srand(time(nullptr));

	GameEngine gameEngine(100, 100, 6);
	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();
	float fElapsedTime = 0;

	PlayerPlatform pp(0, -(gameEngine.ScreenHeight/2)*7/8, 15, 3, gameEngine.ScreenWidth, gameEngine.ScreenHeight, 1);
	Ball ballPref(0,-gameEngine.ScreenHeight/3, gameEngine.ScreenWidth, gameEngine.ScreenHeight, 0,0, 0.3, 0.4);
	
	vector<Ball> balls;
	for (int i = 0; i < 1; i++) {
		ballPref.speedX += (10 - rand() % 20) / 1000.;
		ballPref.speedY += (10 - rand() % 20) / 1000.;
		balls.push_back(ballPref);
	}

	
	Tile tilePref(0, 20, 13, 4);
	vector<Tile> tiles;

	vector<COLOR> colors; COLOR color;
	for (int i = 0; i < 6; ++i) {
		for (int j = 0; j < 7; ++j) {
			tilePref.positionX = i*tilePref.sizeX + 2*i - gameEngine.ScreenWidth/2.5;
			tilePref.positionY = j*tilePref.sizeY + 2*j;
			int r = rand() % 4;
			if (r == 0)
				color = WHITE;
			if (r == 1)
				color = BLUE;
			if (r == 2)
				color = GREEN;
			if (r == 3)
				color = RED;

			colors.push_back(color);
			tiles.push_back(tilePref);
		}
	}
	//Tile tile(0, 20, 13, 4);
	int tilesSize = tiles.size();
	using clock = std::chrono::steady_clock;
	auto next_frame = clock::now();


	while (1) {
		next_frame += std::chrono::milliseconds(1000 / 144); // frame lock

		tp2 = chrono::system_clock::now();
		chrono::duration <float> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		fElapsedTime = elapsedTime.count();

		gameEngine.DrawString(-gameEngine.ScreenWidth / 2, gameEngine.ScreenHeight / 2 - 6, to_string(int(1 / fElapsedTime)));
		pp.MoveXCoord();
		pp.MoveYCoord();
		

		for (int i = 0; i < tiles.size(); ++i) {
			for (int j = 0; j < balls.size(); ++j) {
				if (collision(balls[j], tiles[i]) && tiles[i].isAlive) {
					tiles[i].Hp -= 1;
					if (tiles[i].Hp <= 0) {
						tiles[i].isAlive = false;
						tilesSize--;
					}
					balls[j].speedY *= -1;
				}
				if (tiles[i].isAlive) gameEngine.DrawSquare2D(tiles[i].positionX - tiles[i].sizeX / 2, tiles[i].positionY + tiles[i].sizeY / 2, tiles[i].positionX + tiles[i].sizeX / 2, tiles[i].positionY + tiles[i].sizeY / 2, tiles[i].positionX + tiles[i].sizeX / 2, tiles[i].positionY - tiles[i].sizeY / 2, tiles[i].positionX - tiles[i].sizeX / 2, tiles[i].positionY - tiles[i].sizeY / 2, colors[i]);
			}
		}

		gameEngine.DrawString(-gameEngine.ScreenWidth / 2 + 20, gameEngine.ScreenHeight / 2 - 6, to_string(int(tilesSize)));

		//gameEngine.DrawPoint2D(ball.positionX, ball.positionY);
		for (int i = 0; i < balls.size(); i++) {
			balls[i].Move();
			if (collision(balls[i], pp)) {
				if (balls[i].positionX > pp.positionX)
					balls[i].speedX += 0.005f * pp.sizeX / 2;
				if (balls[i].positionX < pp.positionX)
					balls[i].speedX -= 0.005f * pp.sizeX / 2;
				balls[i].speedY *= -1;
			}
			gameEngine.DrawSquare2D(balls[i].positionX - balls[i].sizeX / 2, balls[i].positionY + balls[i].sizeY / 2, balls[i].positionX + balls[i].sizeX / 2, balls[i].positionY + balls[i].sizeY / 2, balls[i].positionX + balls[i].sizeX / 2, balls[i].positionY - balls[i].sizeY / 2, balls[i].positionX - balls[i].sizeX / 2, balls[i].positionY - balls[i].sizeY / 2);
		}
		gameEngine.DrawSquare2D(pp.positionX - pp.sizeX/2, pp.positionY + pp.sizeY/2, pp.positionX + pp.sizeX / 2, pp.positionY + pp.sizeY / 2, pp.positionX + pp.sizeX / 2, pp.positionY - pp.sizeY / 2, pp.positionX - pp.sizeX / 2, pp.positionY - pp.sizeY / 2);

		gameEngine.PrintScreen();
		gameEngine.ClearScreen();


		if (GetAsyncKeyState(unsigned short('C')) & 0x8000) {
			balls.push_back(ballPref);
		}


		std::this_thread::sleep_until(next_frame);
	}

	return 0;
}