#include <cmath>
#include <glut.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

constexpr float PI = 3.14159265359f;
bool checkCollision(float x1, float y1, float r1, float x2, float y2, float r2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float distance = sqrt(dx * dx + dy * dy);
    return distance < r1 + r2;
}
bool checkCollision(float x1, float y1, float x2, float y2) {
    float distance = std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
    const float collisionThreshold = 0.1f;

    return distance < collisionThreshold;
}

float mouthAngle = 60.0f;
bool mouthOpen = true;
float pacManRadius = 0.4f;

float amory;
float amory2;
float amory3;

void drawPacMan() {
    glPushMatrix();
    glRotatef(amory, 0.0f, 0.0f, 1.0f);
    glColor3f(0.0f, 0.3f, 0.7f);
    glBegin(GL_POLYGON);
    glVertex2f(0.0, 0.0);
    for (int i = 0; i <= mouthAngle; i++) {
        float angle = i * 3.14159265 / 180.0;
        float x = pacManRadius * cos(angle);
        float y = pacManRadius * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
    if (mouthOpen) {
        glColor3f(0.0f, 0.3f, 0.7f);
        glBegin(GL_TRIANGLES);
        glVertex2f(0.0, 0.0);
        glVertex2f(pacManRadius, 0.0);
        float angle = mouthAngle * 3.14159265 / 180.0;
        float x = pacManRadius * cos(angle);
        float y = pacManRadius * sin(angle);
        glVertex2f(x, y);
        glEnd();
    }
    glPopMatrix();
}

bool isPointWithinBoundaries(float x, float y) {
    return (x > -1.0f && x < 1.0f && y > -1.0f && y < 1.0f);
}

class ScoreDisplay {
public:
    int score;

    ScoreDisplay(int initialScore = 0) : score(initialScore) {}

    void increaseScore(int points) {
        score += points;
    }

    void draw() {
        glViewport(0, 400, 800, 100);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glRasterPos2f(-0.9, -0.5);

        glColor3f(1.0f, 1.0f, 1.0f);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'S');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'c');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'o');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'r');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'e');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, ':');

        int remainingScore = score;
        int digit;
        do {
            digit = remainingScore % 10;
            remainingScore /= 10;
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, '0' + digit);
        } while (remainingScore > 0);

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }
};
ScoreDisplay scoreDisplay;

class Player {
public:
    float x, y;
    float speed;
    float scale;
    float angle;
    int health;

    Player(float posX = 0.0f, float posY = 0.0f, float s = 0.1f, float sc = 0.2f, int initialHealth = 5)
        : x(posX), y(posY), speed(s), scale(sc), angle(0.0f), health(initialHealth) {}

    void draw() {
        glPushMatrix();
        glTranslatef(x, y, 0);

        angle = atan2(dy, dx) * (180.0f / PI);
        angle -= 90.0f;

        glRotatef(angle, 0, 0, 1);

        glBegin(GL_TRIANGLE_FAN);
        glColor3f(0.8f, 0.65f, 0.5f);
        glVertex2f(0, 0);

        int numSegments = 100;
        float angle;
        for (int i = 0; i <= numSegments; ++i) {
            angle = 2.0f * PI * static_cast<float>(i) / static_cast<float>(numSegments);
            glVertex2f(cos(angle) * 0.5f * scale, sin(angle) * 0.5f * scale);
        }

        glEnd();

        glBegin(GL_QUADS);
        glColor3f(1.0f, 1.0f, 1.0f);

        float eyeSize = 0.1f * scale;

        glVertex2f(-0.2f * scale, 0.15f * scale);
        glVertex2f(-0.2f * scale, 0.05f * scale);
        glVertex2f(-0.1f * scale, 0.05f * scale);
        glVertex2f(-0.1f * scale, 0.15f * scale);

        glVertex2f(0.2f * scale, 0.15f * scale);
        glVertex2f(0.2f * scale, 0.05f * scale);
        glVertex2f(0.1f * scale, 0.05f * scale);
        glVertex2f(0.1f * scale, 0.15f * scale);
        glEnd();


        glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(0, 0.15f * scale);
        glVertex2f(-0.05f * scale, 0.05f * scale);
        glVertex2f(0.05f * scale, 0.05f * scale);
        glEnd();

        glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(-0.1f * scale, -0.05f * scale);
        glVertex2f(0.1f * scale, -0.05f * scale);
        glEnd();

        glPopMatrix();
    }

    void move(float dx, float dy) {
        x += dx;
        y += dy;
    }

    void updateHealth(int newHealth) {
        health = newHealth;
    }

    float dx, dy;

    void setDirection(float dirX, float dirY) {
        dx = dirX;
        dy = dirY;
    }
};
Player player;

class VerticalWall {
public:
    float x;
    float y1, y2;
    float thickness;
    float triangleSize;

    VerticalWall(float posX, float posY1, float posY2, float t = 0.02f, float triangleS = 0.04f)
        : x(posX), y1(posY1), y2(posY2), thickness(t), triangleSize(triangleS) {}

    void draw() {
        glColor3f(0.4f, 0.2f, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(x - thickness, y1);
        glVertex2f(x + thickness, y1);
        glVertex2f(x + thickness, y2);
        glVertex2f(x - thickness, y2);
        glEnd();


        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_TRIANGLES);
        glVertex2f(x, y1 + triangleSize);
        glVertex2f(x + triangleSize, y1);
        glVertex2f(x - triangleSize, y1);

        glVertex2f(x, y2 - triangleSize);
        glVertex2f(x + triangleSize, y2);
        glVertex2f(x - triangleSize, y2);
        glEnd();
    }
};
VerticalWall leftBoundary(-1.0f, -1.0f, 1.0f);
VerticalWall rightBoundary(1.0f, -1.0f, 1.0f);

class HorizontalWall {
public:
    float x1, x2;
    float y;
    float thickness;
    float triangleSize;

    HorizontalWall(float posX1, float posX2, float posY, float t = 0.02f, float triangleS = 0.04f)
        : x1(posX1), x2(posX2), y(posY), thickness(t), triangleSize(triangleS) {}

    void draw() {
        glColor3f(0.4f, 0.2f, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(x1, y - thickness);
        glVertex2f(x2, y - thickness);
        glVertex2f(x2, y + thickness);
        glVertex2f(x1, y + thickness);
        glEnd();

        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_TRIANGLES);
        glVertex2f(x1 + triangleSize, y);
        glVertex2f(x1, y + triangleSize);
        glVertex2f(x1, y - triangleSize);

        glVertex2f(x2 - triangleSize, y);
        glVertex2f(x2, y + triangleSize);
        glVertex2f(x2, y - triangleSize);
        glEnd();
    }
};
HorizontalWall topBoundary(-1.0f, 1.0f, 1.0f);
HorizontalWall bottomBoundary(-1.0f, 1.0f, -1.0f);

class Obstacle {
public:
    float x, y;
    float radius;

    Obstacle(float posX = 0.0f, float posY = 0.0f, float r = 0.1f)
        : x(posX), y(posY), radius(r) {}

    void draw() {
        glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for (int i = 0; i <= 100; ++i) {
            float angle = 2.0f * PI * static_cast<float>(i) / 100;
            float x_i = x + radius * cos(angle);
            float y_i = y + radius * sin(angle);
            glVertex2f(x_i, y_i);
        }
        glEnd();

        glLineWidth(3.0f);
        glColor3f(0.0f, 0.0f, 0.0f);

        glBegin(GL_LINES);

        glVertex2f(x, y + radius);
        glVertex2f(x, y);

        glVertex2f(x, y - radius);
        glVertex2f(x, y);

        glVertex2f(x - radius, y);
        glVertex2f(x, y);

        glVertex2f(x + radius, y);
        glVertex2f(x, y);

        glEnd();
        glLineWidth(1.0f);
    }
    float getTop() {
        return y + radius;
    }

    float getBottom() {
        return y - radius;
    }
};
Obstacle obstacles[4] = {
    Obstacle(0.0f, -0.8f, 0.1f),
    Obstacle(-0.8f, 0.0f, 0.1f),
    Obstacle(0.8f, 0.0f, 0.1f),
    Obstacle(0.0f, 0.8f, 0.1f)
};


class Collectable {
public:
    float x, y;
    float radius;
    float smallCircleRadius;
    float starSize;
    float starVerticesX[5];
    float starVerticesY[5];


    Collectable(float posX = 0.0f, float posY = 0.0f, float r = 0.05f, float smallR = 0.02f, float starS = 0.02f)
        : x(posX), y(posY), radius(r), smallCircleRadius(smallR), starSize(starS) {

        for (int i = 0; i < 5; ++i) {
            float angle = 2.0f * PI * static_cast<float>(i) / 5;
            starVerticesX[i] = x + starSize * cos(angle);
            starVerticesY[i] = y + starSize * sin(angle);
        }
    }

    void draw() {
        glPushMatrix();
        glTranslatef(x, y, 0.0f);

        glRotatef(amory, 0.0f, 0.0f, 1.0f);
        glTranslatef(-x, -y, 0.0f);
        glColor3f(1.0f, 1.0f, 0.0f);

        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for (int i = 0; i <= 100; ++i) {
            float angle = 2.0f * PI * static_cast<float>(i) / 100;
            float x_i = x + radius * cos(angle);
            float y_i = y + radius * sin(angle);
            glVertex2f(x_i, y_i);
        }
        glEnd();

        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for (int i = 0; i <= 100; ++i) {
            float angle = 2.0f * PI * static_cast<float>(i) / 100;
            float x_i = x + smallCircleRadius * cos(angle);
            float y_i = y + smallCircleRadius * sin(angle);
            glVertex2f(x_i, y_i);
        }
        glEnd();

        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < 5; ++i) {
            int nextVertex = (i + 2) % 5;
            glVertex2f(x, y);
            glVertex2f(starVerticesX[i], starVerticesY[i]);
            glVertex2f(starVerticesX[nextVertex], starVerticesY[nextVertex]);
        }
        glEnd();
        glPopMatrix();
    }
};
Collectable collectables[5];

class Goal {
public:
    float x, y;
    float scale;

    Goal(float posX = 0.0f, float posY = 0.0f, float sc = 0.4f)
        : x(posX), y(posY), scale(sc) {}

    void draw() {
        glPushMatrix();
        glTranslatef(x, y, 0.0f);

        glRotatef(amory, 0.0f, 0.0f, 1.0f);
        glTranslatef(-x, -y, 0.0f);
        glColor3f(0.4f, 0.0f, 0.0f);

        glBegin(GL_QUADS);
        glVertex2f(x - 0.2f * scale, y - 0.2f * scale);
        glVertex2f(x + 0.2f * scale, y - 0.2f * scale);
        glVertex2f(x + 0.2f * scale, y + 0.2f * scale);
        glVertex2f(x - 0.2f * scale, y + 0.2f * scale);
        glEnd();

        glColor3f(0.0f, 0.0f, 0.0f);

        glBegin(GL_QUADS);
        glVertex2f(x - 0.05f * scale, y - 0.2f * scale);
        glVertex2f(x + 0.05f * scale, y - 0.2f * scale);
        glVertex2f(x + 0.05f * scale, y);
        glVertex2f(x - 0.05f * scale, y);
        glEnd();

        glColor3f(0.7f, 0.7f, 0.8f);

        glBegin(GL_QUADS);
        glVertex2f(x - 0.15f * scale, y + 0.075f * scale);
        glVertex2f(x - 0.05f * scale, y + 0.075f * scale);
        glVertex2f(x - 0.05f * scale, y + 0.17f * scale);
        glVertex2f(x - 0.15f * scale, y + 0.17f * scale);
        glEnd();

        glColor3f(0.4f, 0.0f, 0.0f);

        glBegin(GL_QUADS);
        glVertex2f(x + 0.05f * scale, y + 0.2f * scale);
        glVertex2f(x + 0.1f * scale, y + 0.2f * scale);
        glVertex2f(x + 0.1f * scale, y + 0.3f * scale);
        glVertex2f(x + 0.05f * scale, y + 0.3f * scale);
        glEnd();
        glPopMatrix();
    }
};

class BottlePowerUp {
public:
    float x, y;
    float scale;

    BottlePowerUp(float posX = 0.0f, float posY = 0.0f, float sc = 0.5f)
        : x(posX), y(posY), scale(sc) {
        x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;
        y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;
    }

    float getMinX() {
        return x - (0.075f * scale);
    }

    float getMaxX() {
        return x + (0.075f * scale);
    }

    float getMinY() {
        return y - (0.075f * scale);
    }

    float getMaxY() {
        return y + (0.075f * scale);
    }

    void draw() {
        glPushMatrix();
        glTranslatef(x, y, 0.0f);

        glRotatef(amory, 0.0f, 0.0f, 1.0f);
        glTranslatef(-x, -y, 0.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
        glBegin(GL_QUADS);
        glVertex2f(x - 0.075f * scale, y - 0.15f * scale);
        glVertex2f(x + 0.075f * scale, y - 0.15f * scale);
        glVertex2f(x + 0.075f * scale, y);
        glVertex2f(x - 0.075f * scale, y);
        glEnd();

        glColor3f(0.7529f, 0.7529f, 0.7529f);
        glBegin(GL_QUADS);
        glVertex2f(x - 0.03f * scale, y);
        glVertex2f(x + 0.03f * scale, y);
        glVertex2f(x + 0.03f * scale, y + 0.075f * scale);
        glVertex2f(x - 0.03f * scale, y + 0.075f * scale);
        glEnd();

        glColor3f(1.0f, 0.0f, 0.0f);

        glBegin(GL_TRIANGLES);
        glVertex2f(x - 0.015f * scale, y + 0.075f * scale);
        glVertex2f(x + 0.015f * scale, y + 0.075f * scale);
        glVertex2f(x, y + 0.125f * scale);


        glVertex2f(x - 0.015f * scale, y - 0.15f * scale);
        glVertex2f(x + 0.015f * scale, y - 0.15f * scale);
        glVertex2f(x, y - 0.1f * scale);
        glEnd();
        glPopMatrix();
    }
};
BottlePowerUp bottlePowerUp;

class StrengthPowerUp {
public:
    float x, y;
    float scale;
    StrengthPowerUp(float posX = 0.0f, float posY = 0.0f, float sc = 0.3f)
        : x(posX), y(posY), scale(sc) {
        x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;
        y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;
    }
    float getMinX() {
        return x - (0.2f * scale);
    }

    float getMaxX() {
        return x + (0.2f * scale);
    }

    float getMinY() {
        return y - (0.2f * scale);
    }

    float getMaxY() {
        return y + (0.2f * scale);
    }
    void draw() {
        glPushMatrix();
        glTranslatef(x, y, 0.0f);

        glRotatef(amory, 0.0f, 0.0f, 1.0f);
        glTranslatef(-x, -y, 0.0f);
        glColor3f(1.0f, 0.5f, 0.0f);
        glBegin(GL_TRIANGLES);
        float angle = 2 * PI / 5.0f;
        for (int i = 0; i < 5; ++i) {
            glVertex2f(x + 0.2f * scale * cos(i * angle), y + 0.2f * scale * sin(i * angle));
            glVertex2f(x, y);
            glVertex2f(x + 0.1f * scale * cos((i + 0.5f) * angle), y + 0.1f * scale * sin((i + 0.5f) * angle));
        }
        glEnd();
        glPopMatrix();
    }
};
StrengthPowerUp powerUp;


class HealthBar {
public:
    int playerHealth;
    const int maxHealth;
    float barWidth;
    float barHeight;
    float rectWidth;
    float offsetX;
    float offsetY;

    HealthBar(int maxHealth = 5, float offset_X = -0.5f, float offset_Y = 0.4f)
        : playerHealth(maxHealth), maxHealth(maxHealth), offsetX(offset_X), offsetY(offset_Y) {
        barWidth = 0.5f;
        barHeight = 0.1f;
        rectWidth = barWidth / maxHealth;
    }

    void decreaseHealth() {
        if (playerHealth > 0) {
            playerHealth--;
        }
    }

    void increaseHealth() {
        if (playerHealth < maxHealth) {
            playerHealth++;
        }
    }

    void draw() {
        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(offsetX - 0.2f, offsetY - 0.025f);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'L');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'i');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'v');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'e');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 's');

        float green[3] = { 0.0f, 1.0f, 0.0f };
        float orange[3] = { 1.0f, 0.5f, 0.0f };
        float red[3] = { 1.0f, 0.0f, 0.0f };
        float gray[3] = { 0.5f, 0.5f, 0.5f };

        float currentWidth = barWidth * (static_cast<float>(playerHealth) / maxHealth);

        for (int i = 0; i < maxHealth; ++i) {
            float* color;
            if (i < playerHealth) {
                if (playerHealth >= 5 || playerHealth == 4) {
                    color = green;
                }
                else if (playerHealth == 3) {
                    color = orange;
                }
                else {
                    color = red;
                }
            }
            else {
                color = gray;
            }

            glColor3fv(color);

            float rectLeft = offsetX + i * rectWidth;
            float rectRight = rectLeft + rectWidth;

            glBegin(GL_QUADS);
            glVertex2f(rectLeft, offsetY);
            glVertex2f(rectRight, offsetY);
            glVertex2f(rectRight, offsetY - barHeight);
            glVertex2f(rectLeft, offsetY - barHeight);
            glEnd();
        }
    }

    void updateShape() {
        float currentWidth = barWidth * (static_cast<float>(playerHealth) / maxHealth);
        float green[3] = { 0.0f, 1.0f, 0.0f };
        float orange[3] = { 1.0f, 0.5f, 0.0f };
        float red[3] = { 1.0f, 0.0f, 0.0f };
        float gray[3] = { 0.5f, 0.5f, 0.5f };

        for (int i = 0; i < maxHealth; ++i) {
            float* color;
            if (i < playerHealth) {
                if (playerHealth >= 5 || playerHealth == 4) {
                    color = green;
                }
                else if (playerHealth == 3) {
                    color = orange;
                }
                else {
                    color = red;
                }
            }
            else {
                color = gray;
            }

            float rectLeft = offsetX + i * rectWidth;
            float rectRight = rectLeft + rectWidth;

            glBegin(GL_QUADS);
            glColor3fv(color);
            glVertex2f(rectLeft, offsetY);
            glVertex2f(rectRight, offsetY);
            glVertex2f(rectRight, offsetY - barHeight);
            glVertex2f(rectLeft, offsetY - barHeight);
            glEnd();
        }
    }
};
HealthBar HB;
void drawHealthBar() {
    glViewport(0, 500, 800, 100);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    HB.draw();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}
bool checkCollision(Player& player, Obstacle& obstacle, HealthBar& healthBar) {
    float playerLeft = player.x;
    float playerRight = player.x + (0.5f * player.scale);
    float playerTop = player.y;
    float playerBottom = player.y - (0.5f * player.scale);

    float obstacleLeft = obstacle.x - obstacle.radius;
    float obstacleRight = obstacle.x + obstacle.radius;
    float obstacleTop = obstacle.getTop();
    float obstacleBottom = obstacle.getBottom();

    if (playerRight >= obstacleLeft && playerLeft <= obstacleRight &&
        playerTop >= obstacleBottom && playerBottom <= obstacleTop) {
        return true;
    }
    return false;
}

float distance(float x1, float y1, float x2, float y2) {
    return std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}
const float collectableSize = 0.1f;
const float obstacleSize = 0.2f;
const float playerSize = 0.1f;

bool collidesWithObstacles(float x, float y) {
    for (int i = 0; i < 4; ++i) {
        if (distance(x, y, obstacles[i].x, obstacles[i].y) < (collectableSize + obstacleSize)) {
            return true;
        }
    }
    return false;
}
bool collectablesActive[5];




void generateRandomCollectables() {

    for (int i = 0; i < 5; ++i) {
        collectablesActive[i] = false;
    }
    for (int i = 0; i < 5; ++i) {
        float posX, posY;
        do {
            posX = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;
            posY = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;

        } while (collidesWithObstacles(posX, posY) || !isPointWithinBoundaries(posX, posY));
        collectables[i] = Collectable(posX, posY);
        collectablesActive[i] = true;

    }
}
void checkCollectableCollisions(Player& player, ScoreDisplay& scoreDisplay) {
    for (int i = 0; i < 5; ++i) {
        if (collectablesActive[i]) {
            float playerXMin = player.x - (0.5f * player.scale);
            float playerXMax = player.x + (0.5f * player.scale);
            float playerYMin = player.y - (0.5f * player.scale);
            float playerYMax = player.y + (0.5f * player.scale);

            float collectableX = collectables[i].x;
            float collectableY = collectables[i].y;

            float collectableXMin = collectableX - 0.05f;
            float collectableXMax = collectableX + 0.05f;
            float collectableYMin = collectableY - 0.05f;
            float collectableYMax = collectableY + 0.05f;

            if (playerXMax >= collectableXMin && playerXMin <= collectableXMax &&
                playerYMax >= collectableYMin && playerYMin <= collectableYMax) {
                collectablesActive[i] = false;
                scoreDisplay.increaseScore(10);
            }
        }
    }
}
void drawCollectables() {
    for (int i = 0; i < 5; ++i) {
        if (collectablesActive[i]) {
            collectables[i].draw();
        }
    }
}



bool GoalReached = false;
bool keyboardInputFrozen = false;
Goal goal;
bool AppearWin;


void generateRandomGoal() {
    float posX, posY;

    do {
        posX = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (rightBoundary.x - leftBoundary.x - 2 * goal.scale - 0.1f) + leftBoundary.x + goal.scale + 0.05f;
        posY = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (topBoundary.y - bottomBoundary.y - 2 * goal.scale - 0.1f) + bottomBoundary.y + goal.scale + 0.05f;
    } while (checkCollision(posX, posY, goal.scale, player.x, player.y, player.scale) ||
        checkCollision(posX, posY, goal.scale, bottlePowerUp.x, bottlePowerUp.y, bottlePowerUp.scale) ||
        checkCollision(posX, posY, goal.scale, powerUp.x, powerUp.y, powerUp.scale) || !isPointWithinBoundaries(posX, posY));
    goal = Goal(posX, posY);
    GoalReached = true;
}
void collidesWithGoal(Player& player) {

    if (GoalReached) {
        float playerXMin = player.x - (0.5f * player.scale);
        float playerXMax = player.x + (0.5f * player.scale);
        float playerYMin = player.y - (0.5f * player.scale);
        float playerYMax = player.y + (0.5f * player.scale);

        float GoalX = goal.x;
        float GoalY = goal.y;

        float GoalXMin = GoalX - 0.05f;
        float GoalXMax = GoalX + 0.05f;
        float GoalYMin = GoalY - 0.05f;
        float GoalYMax = GoalY + 0.05f;
        if (playerXMax >= GoalXMin && playerXMin <= GoalXMax &&
            playerYMax >= GoalYMin && playerYMin <= GoalYMax) {
            GoalReached = true;
            AppearWin = true;
            keyboardInputFrozen = true;
        }
    }
}
void GoalDraw() {
    if (GoalReached) {
        goal.draw();
    }
}

bool PowerUpActive = false;
void generateRandomPowerUp() {
    float posX, posY;

    do {
        posX = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;
        posY = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;
    } while (checkCollision(posX, posY, powerUp.scale, player.x, player.y, player.scale) ||
        checkCollision(posX, posY, powerUp.scale, bottlePowerUp.x, bottlePowerUp.y, bottlePowerUp.scale) ||
        checkCollision(posX, posY, powerUp.scale, goal.x, goal.y, goal.scale) || !isPointWithinBoundaries(posX, posY) || collidesWithObstacles(posX, posY));

    powerUp = StrengthPowerUp(posX, posY);
    PowerUpActive = true;
}
void collidesWithPlayer(Player& player) {
    if (PowerUpActive) {
        float minX = powerUp.getMinX();
        float maxX = powerUp.getMaxX();
        float minY = powerUp.getMinY();
        float maxY = powerUp.getMaxY();

        float playerMinX = player.x - player.scale;
        float playerMaxX = player.x + player.scale;
        float playerMinY = player.y - player.scale;
        float playerMaxY = player.y + player.scale;

        if (minX <= playerMaxX && maxX >= playerMinX && minY <= playerMaxY && maxY >= playerMinY) {
            PowerUpActive = false;
            collectablesActive[0] = false;
            collectablesActive[1] = false;
            collectablesActive[2] = false;
            collectablesActive[3] = false;
            collectablesActive[4] = false;
            scoreDisplay.increaseScore(50);
        }
    }
}
void Magnet() {
    if (PowerUpActive) {
        powerUp.draw();
    }
}

bool drunk = false;
bool BPowerUpActive = false;
void generateRandomBottlePowerUp() {
    float posX, posY;

    do {
        posX = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;
        posY = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;
    } while (checkCollision(posX, posY, bottlePowerUp.scale, player.x, player.y, player.scale) ||
        checkCollision(posX, posY, bottlePowerUp.scale, goal.x, goal.y, goal.scale)
        || checkCollision(posX, posY, bottlePowerUp.scale, powerUp.x, powerUp.y, powerUp.scale) || !isPointWithinBoundaries(posX, posY) || collidesWithObstacles(posX, posY));
    bottlePowerUp = BottlePowerUp(posX, posY);
    BPowerUpActive = true;
}
void bcollidesWithPlayer(Player& player) {
    if (BPowerUpActive) {
        float minX = bottlePowerUp.getMinX();
        float maxX = bottlePowerUp.getMaxX();
        float minY = bottlePowerUp.getMinY();
        float maxY = bottlePowerUp.getMaxY();

        float playerMinX = player.x - player.scale;
        float playerMaxX = player.x + player.scale;
        float playerMinY = player.y - player.scale;
        float playerMaxY = player.y + player.scale;

        if (minX <= playerMaxX && maxX >= playerMinX && minY <= playerMaxY && maxY >= playerMinY) {
            BPowerUpActive = false;
            drunk = true;
        }
    }
}
void Magnetoo() {
    if (BPowerUpActive) {
        bottlePowerUp.draw();
    }
}

bool doesPlayerTouchBoundaries(float x, float y) {
    return (x <= -1.0f || x >= 1.0f || y <= -1.0f || y >= 1.0f);
}

bool isGameOver = false;
void timer(int value) {
    if (!isGameOver) {
        for (int i = 0; i < 4; ++i) {
            if (checkCollision(player, obstacles[i], HB)) {
                HB.updateShape();
                HB.decreaseHealth();
                player.updateHealth(HB.playerHealth);
                HB.updateShape();
                if (HB.playerHealth <= 0) {
                    isGameOver = true;
                    break;
                }
            }
        }
    }
    if (!isGameOver) {
        glutPostRedisplay();
        glutTimerFunc(100, timer, 0);
    }
}

int gameTime = 0;
bool flag30Reached = false;
void updateTimer(int value) {
    gameTime++;
    if (gameTime == 30) {
        flag30Reached = true;
    }
    if (gameTime < 30) {
        glutTimerFunc(1000, updateTimer, 0);
    }

}

void updateamory(int value) {
    amory += 5;
    if (doesPlayerTouchBoundaries(player.x, player.y)) {
        HB.updateShape();
        HB.decreaseHealth();
        player.updateHealth(HB.playerHealth);
        HB.updateShape();
        if (HB.playerHealth <= 0) {
            isGameOver = true;
        }
    }
    Magnetoo();
    glutTimerFunc(16, updateamory, 0);
}

void drawTimer() {
    int remainingTime = 30 - gameTime;
    if (drunk) {
        gameTime += 5;
        drunk = false;
    }

    glRasterPos2f(-0.15f, 1.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'T');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'i');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'm');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'e');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'r');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ':');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '0' + remainingTime / 10);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '0' + remainingTime % 10);
}

void display() {
    glClearColor(0.0f, 0.2f, 0.0f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT);
    drawHealthBar();
    scoreDisplay.draw();

    glViewport(0, 0, 800, 500);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    drawPacMan();

    topBoundary.draw();
    bottomBoundary.draw();
    leftBoundary.draw();
    rightBoundary.draw();

    player.draw();
    bottlePowerUp.draw();

    glutTimerFunc(0, timer, 0);
    drawTimer();


    for (int i = 0; i < 4; ++i) {
        obstacles[i].draw();
    }
    drawCollectables();
    Magnet();
    GoalDraw();


    if (isGameOver || flag30Reached) {
        glColor3f(1.0f, 0.0f, 0.0f);
        glRasterPos2f(-0.2f, 0.0f);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'G');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'a');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'm');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'e');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'O');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'v');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'e');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'r');
        keyboardInputFrozen = true;
    }
    if (AppearWin && !flag30Reached) {
        glColor3f(0.0f, 1.0f, 0.0f);
        glRasterPos2f(-0.2f, 0.0f);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'Y');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'o');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'u');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ' ');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'W');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'i');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'n');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '!');
        keyboardInputFrozen = true;

    }

    glutSwapBuffers();
}

void moveKeys(int key, int x, int y) {
    if (keyboardInputFrozen) {
        return;
    }
    float dx = 0.0f;
    float dy = 0.0f;

    switch (key) {
    case GLUT_KEY_LEFT:
        dx = -player.speed;
        break;
    case GLUT_KEY_RIGHT:
        dx = player.speed;
        break;
    case GLUT_KEY_UP:
        dy = player.speed;
        break;
    case GLUT_KEY_DOWN:
        dy = -player.speed;
        break;
    }

    player.setDirection(dx, dy);
    player.move(dx, dy);
    bcollidesWithPlayer(player);

    bcollidesWithPlayer(player);
    collidesWithGoal(player);
    checkCollectableCollisions(player, scoreDisplay);
    collidesWithPlayer(player);
    glutPostRedisplay();
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Pepsi Man");



    glutTimerFunc(1000, updateTimer, 0);

    srand(static_cast<unsigned>(time(nullptr)));
    generateRandomBottlePowerUp();
    generateRandomGoal();
    generateRandomPowerUp();
    generateRandomCollectables();
    glutTimerFunc(16, updateamory, 0);
    player = Player(0, 0);

    glutDisplayFunc(display);
    glutSpecialFunc(moveKeys);
    glutMainLoop();


    return 0;
}