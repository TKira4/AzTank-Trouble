#ifndef AI_TANK_H
#define AI_TANK_H

#include "Tank/Tank.h"
#include "Bullet/Bullet.h"
#include "Maze/Maze.h"
#include <vector>
#include <SDL2/SDL.h>

class AITank : public Tank {
public:
    std::vector<SDL_Point> path;
    int currentWaypoint;
    float pathUpdateTimer;
    float shootCooldown;
    int aiOwner; // Mã owner cho đạn của AI này

    AITank(float x, float y, SDL_Color color, int owner);
    void updateAI(const std::vector<Tank*>& targets, Maze &maze, float deltaTime, std::vector<Bullet> &bullets);
    void shoot(std::vector<Bullet> &bullets);
    virtual void render(SDL_Renderer* renderer) override;
};

#endif
