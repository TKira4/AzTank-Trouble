#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include "Maze/Maze.h"
#include "Tank/Tank.h"
#include "AI/AI.h"
#include "Bullet/Bullet.h"

// Lớp Game sẽ quản lý toàn bộ vòng lặp chính, khởi tạo SDL, đối tượng game, v.v.
class Game {
public:
    Game();
    ~Game();

    bool init(const char* title, int width, int height);
    void run();
    void clean();
    Mix_Chunk* gunshotSound = nullptr;
    Mix_Chunk* explosionSound = nullptr;
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;

    Maze* maze;
    Tank player1;         
    Tank* player2;        
    AITank* ai;           
    AITank* bot1;         
    AITank* bot2;         
    std::vector<Bullet> bullets;

    int mode;             // 1: 1 Player + Bot, 2: 2 Players, 3: 2 Players + 2 Bots
    int player1Ammo;      
    int player2Ammo;      

    void processEvents(float deltaTime);
    void update(float deltaTime);
    void render();
    void resetGame();
};

#endif
