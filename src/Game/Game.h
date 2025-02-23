// src/Game.h
#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <vector>
#include "Tank/Tank.h"
#include "Bullet/Bullet.h"
#include "Maze/Maze.h"

class Game {
public:
    Game();
    ~Game();

    bool init(const char* title, int width, int height);
    void run();
    void clean();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;

    Maze maze;
    std::vector<Tank> tanks;
    std::vector<Bullet> bullets;

    // Xử lý input
    void processInput();
    // Cập nhật logic game
    void update();
    // Vẽ mọi thứ
    void render();
};

#endif // GAME_H
