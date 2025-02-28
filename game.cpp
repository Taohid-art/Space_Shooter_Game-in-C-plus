#include <iostream>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <ctime>
using namespace std;

void gotoxy(int x, int y) {
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

class SpaceShooter {
public:
    int width, height, x, y, score, speed;
    bool gameover;
    struct Coord { int x, y; };
    vector<Coord> bullets, enemies;

public:
    SpaceShooter(int w, int h) : width(w), height(h), x(w / 2), y(h - 4), score(0), gameover(false), speed(200) {}

    void drawChar(int x, int y, char ch) { gotoxy(x, y); cout << ch; }

    void drawPlane() {
        gotoxy(x, y - 2); cout << "  | ";
        gotoxy(x, y - 1); cout << " /O\\ ";
        gotoxy(x, y);     cout << "<===>";
    }

    void erasePlane() {
        gotoxy(x, y - 2); cout << "    ";
        gotoxy(x, y - 1); cout << "    ";
        gotoxy(x, y);     cout << "     ";
    }

    void moveLeft() { if (x > 1) x--; }
    void moveRight() { if (x < width - 5) x++; }
    void shoot() { bullets.push_back({ x + 2, y - 3 }); }
    void generateEnemies() { if (rand() % 10 == 0) enemies.push_back({ rand() % (width - 3) + 1, 1 }); }

    void updateBullets() {
        for (int i = bullets.size() - 1; i >= 0; i--) {
            bullets[i].y -= 2;
            for (int j = enemies.size() - 1; j >= 0; j--) {
                if (bullets[i].x == enemies[j].x && bullets[i].y == enemies[j].y) {
                    bullets.erase(bullets.begin() + i);
                    enemies.erase(enemies.begin() + j);
                    score += 10;
                    speed = max(10, speed - 2);
                    break;
                }
            }
            if (bullets[i].y <= 1) bullets.erase(bullets.begin() + i);
        }
    }

    void updateEnemies() {
        for (int i = enemies.size() - 1; i >= 0; i--) {
            enemies[i].y += 2;
            if ((enemies[i].x >= x && enemies[i].x <= x + 3) && enemies[i].y >= y) {
                gameover = true;
                return;
            }
            if (enemies[i].y >= height - 1) enemies.erase(enemies.begin() + i);
        }
    }

    void drawObjects() {
        drawPlane();
        for (auto& b : bullets) drawChar(b.x, b.y, '|');
        for (auto& e : enemies) drawChar(e.x, e.y, 'V');
    }

    bool isGameOver() const { return gameover; }
    int getScore() const { return score; }
    int getSpeed() const { return speed; }
};

int main() {
    system("mode con: lines=40 cols=40");
    srand(time(0));

    while (true) { // Outer loop for restarting
        SpaceShooter game(40, 40);

        cout << "Instructions:\n1. Arrow keys to move\n2. Spacebar to shoot\n3. ESC to quit\n\n";
        system("pause");
        system("cls");

        while (!game.isGameOver()) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == 75) game.moveLeft();
                else if (ch == 77) game.moveRight();
                else if (ch == ' ') game.shoot();
                else if (ch == 27) return 0; // Quit if ESC is pressed
            }

            game.generateEnemies();
            game.updateBullets();
            game.updateEnemies();

            game.erasePlane();  // Prevents flickering
            game.drawObjects();

            Sleep(game.getSpeed());
            system("cls");
        }

        gotoxy(15, 20);
        cout << "Game Over! Score: " << game.getScore();
        gotoxy(15, 22);
        cout << "Press 'R' to Restart or 'Q' to Quit.";

        while (true) {
            char choice = _getch();
            if (choice == 'r' || choice == 'R') {
                system("cls");
                break; // Restart game
            } else if (choice == 'q' || choice == 'Q') {
                return 0; // Quit game
            }
        }
    }
}

