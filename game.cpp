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
    int width, height, x, y, score, speed, health;
    bool gameover;
    struct Coord { int x, y; };
    vector<Coord> bullets, enemies;

    // Boss
    bool bossActive = false;
    int bossX = 0, bossY = 2, bossHealth = 10;
    int bossDirection = 1;

public:
    SpaceShooter(int w, int h) : width(w), height(h), x(w / 2), y(h - 4),
        score(0), gameover(false), speed(200), health(3) {}

    void drawChar(int x, int y, char ch) { gotoxy(x, y); cout << ch; }

    void drawPlane() {
        gotoxy(x, y - 2); cout << "  | ";
        gotoxy(x, y - 1); cout << " /O\\ ";
        gotoxy(x, y);     cout << "<===>";
    }

    void erasePlane() {
        gotoxy(x, y - 2); cout << "     ";
        gotoxy(x, y - 1); cout << "     ";
        gotoxy(x, y);     cout << "     ";
    }

    void moveLeft() { if (x > 1) x--; }
    void moveRight() { if (x < width - 5) x++; }

    void shoot() {
        bullets.push_back({ x + 2, y - 3 });
        Beep(1000, 50); // Shooting sound
    }

    void generateEnemies() {
        if (bossActive) return;
        int difficulty = max(5, speed / 40);
        if (rand() % difficulty == 0)
            enemies.push_back({ rand() % (width - 3) + 1, 1 });
    }

    void updateBullets() {
        for (int i = bullets.size() - 1; i >= 0; i--) {
            bullets[i].y -= 2;

            if (bossActive &&
                bullets[i].x >= bossX && bullets[i].x <= bossX + 6 &&
                bullets[i].y == bossY + 1) {
                bossHealth--;
                Beep(500, 100);
                bullets.erase(bullets.begin() + i);
                if (bossHealth <= 0) {
                    bossActive = false;
                    score += 100; // Big bonus!
                }
                continue;
            }

            for (int j = enemies.size() - 1; j >= 0; j--) {
                if (bullets[i].x == enemies[j].x && bullets[i].y == enemies[j].y) {
                    bullets.erase(bullets.begin() + i);
                    enemies.erase(enemies.begin() + j);
                    Beep(400, 150);
                    score += 10;
                    speed = max(10, speed - 2);
                    break;
                }
            }

            if (i < bullets.size() && bullets[i].y <= 1)
                bullets.erase(bullets.begin() + i);
        }

        // 🟡 Activate boss at score 50
        if (!bossActive && score >= 10) {
            bossActive = true;
            bossX = 10;
            bossHealth = 10;
            enemies.clear(); // 💥 Clear all regular enemies!
            Beep(800, 300);  // Boss appears
        }
    }

    void updateEnemies() {
        if (bossActive) {
            bossX += bossDirection;
            if (bossX <= 1 || bossX >= width - 10)
                bossDirection *= -1;

            // Collision with player
            if ((bossX <= x + 4 && bossX + 6 >= x) && bossY + 1 >= y) {
                health = 0;
                gameover = true;
            }

            return;
        }

        for (int i = enemies.size() - 1; i >= 0; i--) {
            enemies[i].y += 2;
            if ((enemies[i].x >= x && enemies[i].x <= x + 3) && enemies[i].y >= y) {
                enemies.erase(enemies.begin() + i);
                health--;
                Beep(200, 300);
                if (health <= 0) {
                    gameover = true;
                    return;
                }
            } else if (enemies[i].y >= height - 1) {
                enemies.erase(enemies.begin() + i);
            }
        }
    }

    void drawObjects() {
        drawPlane();
        for (auto& b : bullets) drawChar(b.x, b.y, '|');
        for (auto& e : enemies) drawChar(e.x, e.y, 'V');

        if (bossActive) {
            gotoxy(bossX, bossY);     cout << " [BOSS] ";
            gotoxy(bossX, bossY + 1); cout << "[#####]";
        }

        gotoxy(1, 1); cout << "Score: " << score << "  Health: " << health;
        if (bossActive) {
            gotoxy(1, 2); cout << "Boss HP: " << bossHealth << "      ";
        }
    }

    bool isGameOver() const { return health <= 0 || gameover; }
    int getScore() const { return score; }
    int getSpeed() const { return speed; }
};

int main() {
    system("mode con: lines=40 cols=40");
    srand(time(0));

    while (true) {
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
                else if (ch == 27) return 0;
            }

            game.generateEnemies();
            game.updateBullets();
            game.updateEnemies();

            game.erasePlane();
            game.drawObjects();

            Sleep(game.getSpeed());
            system("cls");
        }

        gotoxy(12, 18);
        cout << "🔥 Game Over! Score: " << game.getScore() << " 🔥";
        gotoxy(10, 20);
        cout << "Press 'R' to Restart or 'Q' to Quit.";

        while (true) {
            char choice = _getch();
            if (choice == 'r' || choice == 'R') {
                system("cls");
                break;
            } else if (choice == 'q' || choice == 'Q') {
                return 0;
            }
        }
    }
}
