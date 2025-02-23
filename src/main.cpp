#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <algorithm>

#include "Maze/Maze.h"
#include "Bullet/Bullet.h"
#include "Tank/Tank.h"
#include "AI/AI.h"
#include "UI.cpp"
using namespace std;


int main(int argc, char* argv[]) {
    srand(time(NULL));
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL init error: " << SDL_GetError() << endl;
        return 1;
    }
    
    SDL_Window* window = SDL_CreateWindow("Tank Trouble with Maze",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        cout << "Window creation error: " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cout << "Renderer creation error: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    Maze maze(MAZE_ROWS, MAZE_COLS, CELL_SIZE);
    //dat player va AI vao vi tri open maze
    Tank player(1 * CELL_SIZE + 5, 1 * CELL_SIZE + 5, {0, 255, 0, 255});
    AITank ai((MAZE_COLS - 2) * CELL_SIZE + 5, (MAZE_ROWS - 2) * CELL_SIZE + 5, {255, 0, 0, 255});
    
    player.initTexture(renderer);
    ai.initTexture(renderer);
    
    vector<Bullet> bullets;
    int playerAmmo = 8;  //limit 8 vien dan
    
    Uint32 lastTime = SDL_GetTicks();
    bool quit = false;
    SDL_Event e;
    
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                quit = true;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE)
                    quit = true;
                if (e.key.keysym.sym == SDLK_c && player.alive && playerAmmo > 0) {
                    float bulletSpeed = 200.0f;
                    SDL_Point c = player.getCenter();
                    float bx = c.x - 3;
                    float by = c.y - 3;
                    bullets.push_back(Bullet(bx, by, cos(player.angle)*bulletSpeed, sin(player.angle)*bulletSpeed, 0));
                    playerAmmo--;
                }
            }
        }
        
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        const Uint8* keyState = SDL_GetKeyboardState(NULL);
        if (player.alive)
            player.update(keyState, deltaTime, maze);
        if (ai.alive)
            ai.updateAI(player, maze, deltaTime, bullets);
        
        for (auto &b : bullets)
            b.update(deltaTime, maze);
        
        //dan chi ton tai trong 8s
        for (auto it = bullets.begin(); it != bullets.end();) {
            if (currentTime - it->spawnTime > 8000) {
                if (it->owner == 0) {
                    playerAmmo++;
                    if (playerAmmo > 8) playerAmmo = 8;
                }
                it = bullets.erase(it);
                continue;
            }
            ++it;
        }
        
        //va cham bullet voi tank
        for (auto it = bullets.begin(); it != bullets.end();) {
            SDL_Rect br = it->getRect();
            SDL_Rect aiRect = ai.getRect();
            SDL_Rect playerRect = player.getRect();
            if (it->owner == 0 && ai.alive && SDL_HasIntersection(&br, &aiRect)) {
                ai.alive = false;
                it = bullets.erase(it);
                continue;
            } else if (it->owner == 1 && player.alive && SDL_HasIntersection(&br, &playerRect)) {
                player.alive = false;
                it = bullets.erase(it);
                continue;
            }
            ++it;
        }
        
        if (!player.alive || !ai.alive) {
            if(!player.alive) showNotification(renderer, "Chúc mừng, bạn thắng!");
            else showNotification(renderer, "May thua roi!");
            break;
        }
        
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        
        maze.render(renderer);
        if (player.alive)
            player.render(renderer);
        if (ai.alive)
            ai.render(renderer);
        for (auto &b : bullets)
            b.render(renderer);
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
