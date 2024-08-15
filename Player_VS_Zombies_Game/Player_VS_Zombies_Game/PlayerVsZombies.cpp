#include <iostream>
#include <fstream>
#include <Windows.h>
#include <conio.h>
#include <ctime> 
#include "LinkedList.h"

using namespace std;

int getRandomNumber(int min, int max) {
    return min + rand() % (max - min + 1);
}

class Screen {
public:
    int Height;
    int Width;

    Screen() : Height(0), Width(0) {}
    Screen(const int height, const int width) : Height(height), Width(width) {}

    void setCursorPosition(int x, int y) {
        COORD pos;
        pos.X = x;
        pos.Y = y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    }

    void setConsoleColor(int color) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
    }

    void clearScreen() {
        system("cls");
    }

    void drawBorder(const int borderStartPosX, const int borderStartPosY) {

        for (int a = borderStartPosX; a <= Width + borderStartPosX; a++) {
            setCursorPosition(a, borderStartPosY);
            cout << "_";
        }
        for (int a = borderStartPosY; a <= Height + borderStartPosY; a++) {
            setCursorPosition(borderStartPosX, a);
            cout << "|";
            setCursorPosition(Width + borderStartPosX, a);
            cout << "|";
        }
        setCursorPosition(borderStartPosX, Height + borderStartPosY);
        for (int a = borderStartPosX; a <= Width + borderStartPosX; a++) {
            cout << "_";
        }
    }

};

class Player {
public:
    char Symbol;
    int PosX;
    int PosY;
    int Color;

    Player() : Symbol(0), PosX(0), PosY(0), Color(7) {}
    Player(const char symbol, const int posX, const int posY, const int color)
        : Symbol(symbol), PosX(posX), PosY(posY), Color(color) {}
    void draw(Screen& screen) {
        screen.setCursorPosition(PosX, PosY);
        screen.setConsoleColor(Color);
        cout << Symbol;
        screen.setConsoleColor(7);
    }

    void moveUp(const int minScreenHeight) {
        if (PosY > minScreenHeight)
            PosY--;
    }

    void moveDown(const int screenHeight) {
        if (PosY < screenHeight - 1)
            PosY++;
    }

};

class Zombie {
public:
    char Symbol;
    int PosX;
    int PosY;
    int Color;

    Zombie() : Symbol(0), PosX(0), PosY(0), Color(7) {}
    Zombie(const char symbol, const int posX, const int posY, const int color)
        : Symbol(symbol), PosX(posX), PosY(posY), Color(color) {}
    void draw(Screen& screen) {
        screen.setCursorPosition(PosX, PosY);
        screen.setConsoleColor(Color);
        cout << Symbol;
        screen.setConsoleColor(7);
    }

    void moveLeft(const int MaximumLeft) {
        if (PosX > MaximumLeft)
            PosX--;
    }

    bool operator==(const Zombie& other) const {
        return Symbol == other.Symbol && PosX == other.PosX && PosY == other.PosY && Color == other.Color;
    }
};

class Bullet {
public:
    char Symbol;
    int PosX;
    int PosY;
    int Color;

    Bullet() : Symbol(0), PosX(0), PosY(0), Color(7) {}
    Bullet(const char symbol, const int posX, const int posY, const int color)
        : Symbol(symbol), PosX(posX), PosY(posY), Color(color) {}

    void draw(Screen& screen) {
        screen.setCursorPosition(PosX, PosY);
        screen.setConsoleColor(Color);
        cout << Symbol;
        screen.setConsoleColor(7);
    }

    void moveRight(int screenWidth) {
        if (PosX < screenWidth)
            PosX++;
    }

    bool operator==(const Bullet& other) const {
        return Symbol == other.Symbol && PosX == other.PosX && PosY == other.PosY && Color == other.Color;
    }
};

class Wall {
public:
    int PosX;
    int PosY;
    int Height;
    int Health;
    int Color;
    Wall() : PosX(0), PosY(0), Height(0), Health(3), Color(7) {}
    Wall(const int posX, const int posY, const int height, const int health, const int color)
        : PosX(posX), PosY(posY), Height(height), Health(health), Color(color) {}

    void draw(Screen& screen) {
        screen.setConsoleColor(Color);
        for (int a = 0; a < Height; a++) {
            screen.setCursorPosition(PosX, PosY + a);
            cout << "|";
        }
        screen.setConsoleColor(7);
    }

    void reduceHealth() {
        if (Health != 0)
        {
            Health--;
        }
    }
};

class Player_VS_Zombie_Game {
private:
    Screen GameScreen;
    Player player;
    LinkedList<Bullet> bullets;
    LinkedList<Zombie> zombies;
    int spawnRate;
    bool WaveSpawned;
    int Score;
    int HighScore;
    Wall wall;

    void createHighScore()
    {
        ifstream fin("HighScoreFile.txt");
        if (!fin)
        {
            ofstream fout("HighScoreFile.txt");
            fout << 0 << endl;
            fout.close();
        }
        fin.close();
    }

    void readHighScore()
    {
        ifstream fin("HighScoreFile.txt");
        fin >> HighScore;
        fin.close();
    }

    void updateHighScore()
    {
        if (HighScore < Score)
        {
            HighScore = Score;
        }
        ofstream fout("HighScoreFile.txt");
        fout << HighScore << endl;
        fout.close();
    }

    void GameTitle() {
        GameScreen.setCursorPosition(GameScreen.Width / 2 - 12, 0);
        cout << "Player_VS_Zombies_Game";
    }

    void ScoreBoard() {
        GameScreen.setCursorPosition(GameScreen.Width / 2 - 12, 2);
        GameScreen.setConsoleColor(14);
        cout << "Score: " << Score << "     High Score: " << HighScore << endl;
        GameScreen.setConsoleColor(7);
    }

    void healthMeter() {
        GameScreen.setCursorPosition(0, 2);
        GameScreen.setConsoleColor(14);
        cout << "Wall Health: " << wall.Health;
        GameScreen.setConsoleColor(7);
        if (wall.Health <= 0) {
            GameOverScreen();
        }
    }

    void BoxOfMsgs() {
        GameScreen.setConsoleColor(10);
        GameScreen.setCursorPosition(GameScreen.Width / 2 - 25, GameScreen.Height + 5);
        cout << "___________________________________________________";
        GameScreen.setCursorPosition(GameScreen.Width / 2 - 25, GameScreen.Height + 6);
        cout << "|  Press W and S to move Player Up and Down: " << char(2) << "    |";
        GameScreen.setCursorPosition(GameScreen.Width / 2 - 25, GameScreen.Height + 7);
        cout << "|  Press F to fire bullets: " << char('*') << "                     |";
        GameScreen.setCursorPosition(GameScreen.Width / 2 - 25, GameScreen.Height + 8);
        cout << "|  Zombies: " << char(254) << "                                     |";
        GameScreen.setCursorPosition(GameScreen.Width / 2 - 25, GameScreen.Height + 9);
        cout << "|  Press E to exit Game                           |";
        GameScreen.setCursorPosition(GameScreen.Width / 2 - 25, GameScreen.Height + 10);
        cout << "___________________________________________________";
        GameScreen.setConsoleColor(7);
    }

    void spawnZombies() {
        if (!WaveSpawned) {
            int NoOfZombiesSpawn = getRandomNumber(1, spawnRate);
            for (int a = 1; a <= NoOfZombiesSpawn; a++) {
                int SpawnPosX = GameScreen.Width - 1;
                int SpawnPosY = getRandomNumber(4, GameScreen.Height - 1);
                Zombie zombie(254, SpawnPosX, SpawnPosY, 12);
                zombies.insertAtEnd(zombie);
            }
            WaveSpawned = true;
        }

        DNode<Zombie>* curr = zombies.getHead();
        while (curr != nullptr) {
            curr->Data.draw(GameScreen);
            curr = curr->Next;
        }
    }

    void zombiesMove() {
        int maxDistanceToLeft = 5;
        DNode<Zombie>* curr = zombies.getHead();
        while (curr != nullptr) {
            if (curr->Data.PosX <= maxDistanceToLeft) {
                DNode<Zombie>* temp = curr;
                curr = curr->Next;
                zombies.deleteValue(temp->Data);
                wall.Health--;
            }
            else {
                curr->Data.moveLeft(maxDistanceToLeft);
                curr = curr->Next;
            }
        }
        if (zombies.isEmpty()) {
            WaveSpawned = false;
        }
    }

    void bulletsSpawn() {
        Bullet bullet('*', player.PosX + 1, player.PosY, 14);
        bullets.insertAtEnd(bullet);
    }

    void bulletsDraw() {
        DNode<Bullet>* curr = bullets.getHead();
        while (curr != nullptr) {
            curr->Data.draw(GameScreen);
            curr = curr->Next;
        }
    }

    void bulletsMove() {
        int maxDistanceForBullet = GameScreen.Width;
        DNode<Bullet>* curr = bullets.getHead();
        while (curr != nullptr) {
            curr->Data.moveRight(maxDistanceForBullet);

            if (curr->Data.PosX >= maxDistanceForBullet) {
                DNode<Bullet>* temp = curr;
                curr = curr->Next;

                bullets.deleteValue(temp->Data);
            }
            else {
                curr = curr->Next;
            }
        }
    }

    void bulletHitsZombie() {
        LinkedList<Bullet> bulletsToDelete;
        LinkedList<Zombie> zombiesToDelete;

        DNode<Bullet>* currBullet = bullets.getHead();
        while (currBullet != nullptr) {
            DNode<Zombie>* currZombie = zombies.getHead();
            while (currZombie != nullptr) {
                if (currBullet->Data.PosY == currZombie->Data.PosY && currBullet->Data.PosX >= currZombie->Data.PosX) {
                    bulletsToDelete.insertAtEnd(currBullet->Data);
                    zombiesToDelete.insertAtEnd(currZombie->Data);
                    Score += 10;
                    break;
                }
                currZombie = currZombie->Next;
            }
            currBullet = currBullet->Next;
        }

        DNode<Bullet>* bulletNode = bulletsToDelete.getHead();
        while (bulletNode != nullptr) {
            bullets.deleteValue(bulletNode->Data);
            bulletNode = bulletNode->Next;
        }

        DNode<Zombie>* zombieNode = zombiesToDelete.getHead();
        while (zombieNode != nullptr) {
            zombies.deleteValue(zombieNode->Data);
            zombieNode = zombieNode->Next;
        }

        if (zombies.isEmpty()) {
            WaveSpawned = false;
        }
    }


    void increaseSpawnRate() {
        if (Score < 10) {
            spawnRate = 1;
        }
        else if (Score >= 10 && Score < 100) {
            spawnRate = 3;
        }
        else if (Score >= 100 && Score < 200) {
            spawnRate = 6;
        }
        else if (Score >= 200 && Score < 500) {
            spawnRate = 9;
        }
        else if (Score >= 500 && Score < 1000) {
            spawnRate = 12;
        }
        else if (Score >= 1000 && Score < 2000) {
            spawnRate = 15;
        }
        else {
            spawnRate = 20;
        }
    }

    void GameOverScreen() {
        GameScreen.clearScreen();
        GameScreen.drawBorder(0, 3);
        retryGameOrExit();
    }

    void retryGameOrExit() {
        int input;
        updateHighScore();
        GameTitle();
        ScoreBoard();
        GameScreen.setConsoleColor(10);
        GameScreen.setCursorPosition((GameScreen.Width) / 2 - 5, GameScreen.Height / 2);
        cout << "Game Over";
        GameScreen.setCursorPosition((GameScreen.Width) / 2 - 20, GameScreen.Height / 2 + 1);
        cout << "Press Enter to retry or Press E to exit";
        GameScreen.setConsoleColor(7);
        do {
            input = _getch();
        } while (input != '\r' && input != 'e');
        if (input == 'e') {
            EndGame();
            exit(0);
        }
        resetingGame();
        GameScreen.clearScreen();
        update();
    }

    void resetingGame() {
        Score = 0;
        wall.Health = 3;
        spawnRate = 1;

        while (!zombies.isEmpty()) {
            zombies.deleteAtEnd();
        }
        while (!bullets.isEmpty()) {
            bullets.deleteAtEnd();
        }
        WaveSpawned = false;
    }

    void playGamePromt() {
        char inputEnter;
        GameScreen.setCursorPosition((GameScreen.Width) / 2 - 10, GameScreen.Height / 2);
        GameScreen.setConsoleColor(10);
        cout << "Press Enter to Play";
        do {
            inputEnter = _getch();
        } while (inputEnter != '\r');
        GameScreen.setConsoleColor(7);
        GameScreen.clearScreen();
    }

    void EndGame()
    {
        GameScreen.clearScreen();
        GameScreen.setConsoleColor(10);
        GameScreen.setCursorPosition(GameScreen.Width / 2 - 10, GameScreen.Height / 2);
        cout << "_________________________________";
        GameScreen.setCursorPosition(GameScreen.Width / 2 - 10, GameScreen.Height / 2 + 1);
        cout << "|                               |";
        GameScreen.setCursorPosition(GameScreen.Width / 2 - 10, GameScreen.Height / 2 + 2);
        cout << "|  Thanks for Playing Our Game  |";
        GameScreen.setCursorPosition(GameScreen.Width / 2 - 10, GameScreen.Height / 2 + 3);
        cout << "|                               |";
        GameScreen.setCursorPosition(GameScreen.Width / 2 - 10, GameScreen.Height / 2 + 4);
        cout << "_________________________________" << endl;
        GameScreen.setConsoleColor(7);
    }

    void update() {
        int input = -1;

        do {
            GameScreen.clearScreen();
            GameTitle();
            GameScreen.drawBorder(0, 3);
            wall.draw(GameScreen);
            player.draw(GameScreen);
            spawnZombies();
            zombiesMove();
            bulletsDraw();
            bulletsMove();
            bulletHitsZombie();
            increaseSpawnRate();
            ScoreBoard();
            healthMeter();
            BoxOfMsgs();
            if (_kbhit()) {
                input = _getch();
                switch (input) {
                case 'w':
                    player.moveUp(4);
                    break;
                case 's':
                    player.moveDown(GameScreen.Height + 3);
                    break;
                case 'f':
                    bulletsSpawn();
                    break;
                case 'e':
                    EndGame();
                    break;
                }
            }

        } while (input != 'e');
    }

public:
    void start() {
        createHighScore();
        readHighScore();
        GameTitle();
        GameScreen.drawBorder(0, 3);
        playGamePromt();
        update();
    }

    Player_VS_Zombie_Game() : GameScreen(20, 100), player(2, 2, 10, 9), Score(0), wall(5, 3, GameScreen.Height, 3, 5), spawnRate(1), WaveSpawned(false), HighScore(0)
    {
        bullets = LinkedList<Bullet>();
        zombies = LinkedList<Zombie>();
    }
};


int main() {
    srand((time(0)));
    Player_VS_Zombie_Game game;
    game.start();
    return 0;
}
