#ifndef TANK_H
#define TANK_H

#include <SDL2/SDL.h>
#include "Maze/Maze.h"

class Tank {
public:
    float x, y;
    float width, height;
    float speed;
    float angle; 
    SDL_Color color;
    bool alive;
    
    Tank(float x, float y, SDL_Color color);
    virtual ~Tank();
    
    SDL_Rect getCollisionRect() const;
    SDL_Rect getRect() const;
    SDL_Point getCenter() const;
    bool collidesWithMaze(const SDL_Rect &rect, Maze &maze) const;
    

    virtual void update(const Uint8* keyState, float deltaTime, Maze &maze);

    virtual void render(SDL_Renderer* renderer);
    
    void initTexture(SDL_Renderer* renderer);
    
protected:
    SDL_Texture* texture;
    SDL_Texture* turretTexture;
};

#endif
