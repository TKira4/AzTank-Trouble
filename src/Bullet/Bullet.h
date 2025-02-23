#ifndef BULLET_H
#define BULLET_H

#include <SDL2/SDL.h>
#include "Maze/Maze.h"

class Bullet {
public:
    float x, y;     // top-left
    float vx, vy;
    int size;
    int owner;      // 0=player, 1=AI
    Uint32 spawnTime; //(ms)

    Bullet(float x, float y, float vx, float vy, int owner);

    SDL_Rect getRect() const;
    void update(float deltaTime, Maze &maze);
    void render(SDL_Renderer* renderer);
};

#endif // BULLET_H
