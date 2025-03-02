#include "Game.h"
#include "UI.h"         
#include "settings.h"   
#include <iostream>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <string>

Game::Game() 
    : window(nullptr), renderer(nullptr), isRunning(false), maze(nullptr),
      //player 1: green, owner = 0
      player1(1 * CELL_SIZE + 5, 1 * CELL_SIZE + 5, {0, 255, 0, 255}),
      player2(nullptr), ai(nullptr), bot1(nullptr), bot2(nullptr),
      player1Ammo(8), player2Ammo(8), mode(0)
{
    srand(static_cast<unsigned>(time(nullptr)));
}

Game::~Game() {
    clean();
}

static bool isPointInWall(Maze* maze, int x, int y) {
    SDL_Rect testRect = { x, y, 1, 1 };
    return maze->collides(testRect);
}

template<typename T>
void safeDelete(T*& ptr) {
    if (ptr) {
        delete ptr;
        ptr = nullptr;
    }
}

void Game::resetGame() {
    std::cout << "[DEBUG] Resetting game, mode = " << mode << std::endl;
    safeDelete(player2);
    safeDelete(ai);
    safeDelete(bot1);
    safeDelete(bot2);
    safeDelete(maze);
    
    maze = new Maze(MAZE_ROWS, MAZE_COLS, CELL_SIZE);
    
    // Reset Player 1
    player1.x = 1 * CELL_SIZE + 5;
    player1.y = 1 * CELL_SIZE + 5;
    player1.alive = true;
    player1.score = 0;
    player1Ammo = 8;
    player1.initTexture(renderer);
    
    if (mode == 1) {
        //Spawn in availble position
        int aiSpawnX, aiSpawnY;
        do {
            aiSpawnX = (rand() % (MAZE_COLS - 2) + 1) * CELL_SIZE + CELL_SIZE / 2;
            aiSpawnY = (rand() % (MAZE_ROWS - 2) + 1) * CELL_SIZE + CELL_SIZE / 2;
        } while (isPointInWall(maze, aiSpawnX, aiSpawnY));
        
        ai = new AITank(aiSpawnX, aiSpawnY, {255, 0, 0, 255}, 2);
        ai->score = 0;
        ai->initTexture(renderer);
        std::cout << "[DEBUG] Mode 1: AI created at (" << aiSpawnX << ", " << aiSpawnY << ")" << std::endl;
    } else if (mode == 2) {
        int p2SpawnX, p2SpawnY;
        do {
            p2SpawnX = (rand() % (MAZE_COLS - 2) + 1) * CELL_SIZE + 5;
            p2SpawnY = (rand() % (MAZE_ROWS - 2) + 1) * CELL_SIZE + 5;
        } while (isPointInWall(maze, p2SpawnX, p2SpawnY));
        
        player2 = new Tank(p2SpawnX, p2SpawnY, {255, 0, 0, 255});
        player2->score = 0;
        player2->initTexture(renderer);
        player2Ammo = 8;
        std::cout << "[DEBUG] Mode 2: Player2 created at (" << p2SpawnX << ", " << p2SpawnY << ")" << std::endl;
    } else if (mode == 3) {
        int p2SpawnX, p2SpawnY, bot1SpawnX, bot1SpawnY, bot2SpawnX, bot2SpawnY;
        do {
            p2SpawnX = (rand() % (MAZE_COLS - 2) + 1) * CELL_SIZE + 5;
            p2SpawnY = (rand() % (MAZE_ROWS - 2) + 1) * CELL_SIZE + 5;
        } while (isPointInWall(maze, p2SpawnX, p2SpawnY));
        
        do {
            bot1SpawnX = (rand() % (MAZE_COLS - 2) + 1) * CELL_SIZE + CELL_SIZE / 2;
            bot1SpawnY = (rand() % (MAZE_ROWS - 2) + 1) * CELL_SIZE + CELL_SIZE / 2;
        } while (isPointInWall(maze, bot1SpawnX, bot1SpawnY));
        
        do {
            bot2SpawnX = (rand() % (MAZE_COLS - 2) + 1) * CELL_SIZE + CELL_SIZE / 2;
            bot2SpawnY = (rand() % (MAZE_ROWS - 2) + 1) * CELL_SIZE + CELL_SIZE / 2;
        } while (isPointInWall(maze, bot2SpawnX, bot2SpawnY));
        
        player2 = new Tank(p2SpawnX, p2SpawnY, {255, 0, 0, 255});
        bot1 = new AITank(bot1SpawnX, bot1SpawnY, {0, 0, 255, 255}, 2);
        bot2 = new AITank(bot2SpawnX, bot2SpawnY, {128, 0, 128, 255}, 3);
        
        player2->score = 0;
        bot1->score = 0;
        bot2->score = 0;
        player2->initTexture(renderer);
        bot1->initTexture(renderer);
        bot2->initTexture(renderer);
        
        std::cout << "[DEBUG] Mode 3: Player2 at (" << p2SpawnX << ", " << p2SpawnY << ")" 
                  << ", bot1 at (" << bot1SpawnX << ", " << bot1SpawnY << ")" 
                  << ", bot2 at (" << bot2SpawnX << ", " << bot2SpawnY << ")" << std::endl;
    }
    
    bullets.clear();
}

bool Game::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL init error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "Window creation error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << "Renderer creation error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "SDL_mixer could not initialize! SDL_mixer Error: "
                  << Mix_GetError() << std::endl;
        return false;
    }

    Mix_Volume(-1, MIX_MAX_VOLUME);

    gunshotSound = Mix_LoadWAV("assets/sounds/tank-shoot.wav");
    if (!gunshotSound) {
        std::cout << "Failed to load gunshot sound! Mix_Error: "
                << Mix_GetError() << std::endl;
}
    
    //Menu game
    mode = showModeMenu(renderer);
    if (mode == 0) {  //Quit
        isRunning = false;
        return true;
    }
    
    maze = new Maze(MAZE_ROWS, MAZE_COLS, CELL_SIZE);
    player1.initTexture(renderer);
    
    if (mode == 1) {
        //Mode 1: player + Bot, bot: red, owner = 2
        ai = new AITank((MAZE_COLS - 2) * CELL_SIZE + 5,
                        (MAZE_ROWS - 2) * CELL_SIZE + 5,
                        {255, 0, 0, 255}, 2);
        ai->initTexture(renderer);
    } else if (mode == 2) {
        player2 = new Tank((MAZE_COLS - 2) * CELL_SIZE + 5,
                           (MAZE_ROWS - 2) * CELL_SIZE + 5,
                           {255, 0, 0, 255});
        player2->initTexture(renderer);
    } else if (mode == 3) {
        player2 = new Tank((MAZE_COLS - 2) * CELL_SIZE + 5,
                           (MAZE_ROWS - 2) * CELL_SIZE + 5,
                           {255, 0, 0, 255});
        player2->initTexture(renderer);
        bot1 = new AITank((MAZE_COLS - 2) * CELL_SIZE + 5, 1 * CELL_SIZE + 5,
                          {0, 0, 255, 255}, 2);
        bot2 = new AITank(1 * CELL_SIZE + 5, (MAZE_ROWS - 2) * CELL_SIZE + 5,
                          {128, 0, 128, 255}, 3);
        bot1->initTexture(renderer);
        bot2->initTexture(renderer);
    }
    player1Ammo = 8;
    player2Ammo = 8;
    isRunning = true;
    return true;
}

void Game::processEvents(float deltaTime) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT)
            isRunning = false;
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE)
                isRunning = false;
            //player 1 shoot by keyword "t"
            if (e.key.keysym.sym == SDLK_t && player1.alive && player1Ammo > 0) {
                float bulletSpeed = 200.0f;
                SDL_Point c = player1.getCenter();
                float bx = c.x - 3;
                float by = c.y - 3;
                bullets.push_back(Bullet(bx, by, cos(player1.angle) * bulletSpeed,
                                           sin(player1.angle) * bulletSpeed, 0));
                player1Ammo--;
                Mix_PlayChannel(-1, gunshotSound, 0);
            }
            //player 1 shoot by keyword "m"
            if (player2 && e.key.keysym.sym == SDLK_m && player2->alive && player2Ammo > 0) {
                float bulletSpeed = 200.0f;
                SDL_Point c = player2->getCenter();
                float bx = c.x - 3;
                float by = c.y - 3;
                bullets.push_back(Bullet(bx, by, cos(player2->angle) * bulletSpeed,
                                           sin(player2->angle) * bulletSpeed, 1));
                player2Ammo--;
                Mix_PlayChannel(-1, gunshotSound, 0);
            }
        }
    }
}

void Game::update(float deltaTime) {
    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    if (player1.alive)
        player1.update(keyState, deltaTime, *maze);
    
    // Xây dựng vector các đối tượng mục tiêu cho AI (chỉ gồm các tank còn sống)
    std::vector<Tank*> targets;
    if (player1.alive)
        targets.push_back(&player1);
    if (player2 && player2->alive)
        targets.push_back(player2);
    
    if (mode == 1 && ai && ai->alive) {
        ai->updateAI(targets, *maze, deltaTime, bullets);
    } else if (mode == 2) {
        if (player2 && player2->alive)
            player2->updatePlayer2(keyState, deltaTime, *maze);
    } else if (mode == 3) {
        if (player2 && player2->alive)
            player2->updatePlayer2(keyState, deltaTime, *maze);
        if (bot1 && bot1->alive)
            bot1->updateAI(targets, *maze, deltaTime, bullets);
        if (bot2 && bot2->alive)
            bot2->updateAI(targets, *maze, deltaTime, bullets);
    }
    
    Uint32 currentTime = SDL_GetTicks();
    for (auto &b : bullets)
        b.update(deltaTime, *maze);
    
    for (auto it = bullets.begin(); it != bullets.end();) {
        if (currentTime - it->spawnTime > 8000) {
            if (it->owner == 0)
                player1Ammo = std::min(player1Ammo + 1, 8);
            else if (it->owner == 1)
                player2Ammo = std::min(player2Ammo + 1, 8);
            it = bullets.erase(it);
            continue;
        }
        ++it;
    }
    
    // xu li va cham
    std::vector<std::pair<Tank*, int>> allTanks;
    if (player1.alive)
        allTanks.push_back({&player1, 0});
    if (player2 && player2->alive)
        allTanks.push_back({player2, 1});
    if (mode == 1 && ai && ai->alive)
        allTanks.push_back({ai, ai->aiOwner});
    if (mode == 3) {
        if (bot1 && bot1->alive)
            allTanks.push_back({bot1, bot1->aiOwner});
        if (bot2 && bot2->alive)
            allTanks.push_back({bot2, bot2->aiOwner});
    }
    
    for (auto it = bullets.begin(); it != bullets.end();) {
        SDL_Rect br = it->getRect();
        bool bulletErased = false;
        for (auto& targetPair : allTanks) {
            Tank* target = targetPair.first;
            int targetOwner = targetPair.second;
            if (it->owner == targetOwner)
                continue;  //friendly fire
            SDL_Rect tr = target->getRect();
            if (SDL_HasIntersection(&br, &tr)) {
                //tang point cho nguoi ban
                switch (it->owner) {
                    case 0: player1.score++; break;
                    case 1: if (player2) player2->score++; break;
                    case 2:
                        if (mode == 1 && ai) { ai->score++; }
                        else if (mode == 3 && bot1) { bot1->score++; }
                        break;
                    case 3: if (mode == 3 && bot2) { bot2->score++; } break;
                }
                target->alive = false;
                it = bullets.erase(it);
                bulletErased = true;
                break;
            }
        }
        if (!bulletErased)
            ++it;
    }
    
    std::vector<int> scores;
    if (mode == 1) {
        //Mode 1: Player vs Bot
        scores.push_back(player1.score);
        scores.push_back(ai ? ai->score : 0);
    } else if (mode == 2) {
        //Mode 2: 2 Player
        scores.push_back(player1.score);
        scores.push_back(player2 ? player2->score : 0);
    } else if (mode == 3) {
        //Mode 3: 2 Player vs 2 Bot
        scores.push_back(player1.score);
        scores.push_back(player2 ? player2->score : 0);
        scores.push_back(bot1 ? bot1->score : 0);
        scores.push_back(bot2 ? bot2->score : 0);
    }
    
    // Game finish when only one tank alives
    int aliveCount = 0;
    if (player1.alive) aliveCount++;
    if (player2 && player2->alive) aliveCount++;
    if (mode == 1 && ai && ai->alive) aliveCount++;
    if (mode == 3) {
        if (bot1 && bot1->alive) aliveCount++;
        if (bot2 && bot2->alive) aliveCount++;
    }
    
    if (aliveCount <= 1) {
        int choice = showGameOverMenu(renderer, scores, mode); // 1: Restart, 2: Return to Menu
        if (choice == 1) {
            resetGame();
        } else if (choice == 2) {
            mode = showModeMenu(renderer);
            if (mode == 0) {
                isRunning = false;
            } else {
                resetGame();
            }
        }
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    maze->render(renderer);
    if (player1.alive)
        player1.render(renderer);
    if (mode == 1 && ai && ai->alive)
        ai->render(renderer);
    if (mode == 2 && player2 && player2->alive)
        player2->render(renderer);
    if (mode == 3) {
        if (player2 && player2->alive)
            player2->render(renderer);
        if (bot1 && bot1->alive)
            bot1->render(renderer);
        if (bot2 && bot2->alive)
            bot2->render(renderer);
    }
    for (auto &b : bullets)
        b.render(renderer);
    SDL_RenderPresent(renderer);
}

void Game::run() {
    Uint32 lastTime = SDL_GetTicks();
    while (isRunning) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        processEvents(deltaTime);
        update(deltaTime);
        render();
        SDL_Delay(16);
    }
}

void Game::clean() {
    if (player2) { delete player2; player2 = nullptr; }
    if (ai) { delete ai; ai = nullptr; }
    if (bot1) { delete bot1; bot1 = nullptr; }
    if (bot2) { delete bot2; bot2 = nullptr; }
    if (maze) { delete maze; maze = nullptr; }
    if (gunshotSound) {
        Mix_FreeChunk(gunshotSound);
        gunshotSound = nullptr;
    }
    Mix_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
