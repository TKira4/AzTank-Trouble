#include "Bullet.h"
#include <cmath>

Bullet::Bullet(float x, float y, float vx, float vy, int owner)
    : x(x), y(y), vx(vx), vy(vy), owner(owner)
{
    size = 6;
    spawnTime = SDL_GetTicks();
}

SDL_Rect Bullet::getRect() const {
    return SDL_Rect{ (int)x, (int)y, size, size };
}

void Bullet::update(float deltaTime, Maze &maze) {
    float nextX = x + vx * deltaTime;
    float nextY = y + vy * deltaTime;
    
    SDL_Rect testRectX = { (int)nextX, (int)y, size, size };
    bool collX = maze.collides(testRectX);
    
    SDL_Rect testRectY = { (int)x, (int)nextY, size, size };
    bool collY = maze.collides(testRectY);
    
    if (!collX && !collY) {
        SDL_Rect testRectXY = { (int)nextX, (int)nextY, size, size };
        if (maze.collides(testRectXY)) {
            vx = -vx;
            vy = -vy;
        } else {
            x = nextX;
            y = nextY;
        }
    }
    else if (!collX && collY) {
        x = nextX;
        vy = -vy;
    }
    else if (collX && !collY) {
        y = nextY;
        vx = -vx;
    }
    else {
        vx = -vx;
        vy = -vy;
    }
}

void Bullet::render(SDL_Renderer* renderer) {
    SDL_Rect r = getRect();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &r);
}
