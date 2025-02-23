#include "Game/Game.h"
#include <iostream>
#include "Collision/CollisionManager.h"
#include <algorithm>

Game::Game() : window(nullptr), renderer(nullptr), isRunning(false) {}

Game::~Game() {
    clean();
}

bool Game::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL khởi tạo thất bại: " << SDL_GetError() << std::endl;
        return false;
    }
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Không thể tạo cửa sổ: " << SDL_GetError() << std::endl;
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Không thể tạo renderer: " << SDL_GetError() << std::endl;
        return false;
    }

    // Sinh mê cung, ví dụ 25x20 cell, mỗi cell 32px
    maze.generateMaze(12, 8, 40);

    // Tạo 1 tank ở (50,50) kích thước 30x30
    tanks.push_back(Tank(50, 50, 20, 20));

    isRunning = true;
    return true;
}

void Game::run() {
    while (isRunning) {
        processInput();
        update();
        render();
        SDL_Delay(16); // ~60fps
    }
}

void Game::processInput() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            isRunning = false;
        }
        // Xử lý input cho tank
        for (auto& t : tanks) {
            t.handleEvent(e);
        }
        // Xử lý phím bắn đạn (ví dụ SPACE)
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
            // Lấy tank[0] bắn đạn, tùy hướng
            auto rect = tanks[0].getRect();
            int dir = tanks[0].getDirection();
            int vx = 0, vy = 0;
            switch(dir) {
                case 0: vy = -8; break; // up
                case 1: vx = 8;  break; // right
                case 2: vy = 8;  break; // down
                case 3: vx = -8; break; // left
            }
            // Tạo bullet ở giữa tank
            bullets.push_back(Bullet(rect.x + rect.w/2, rect.y + rect.h/2, 8, 8, vx, vy));
        }
    }
}

void Game::update() {
    // Cập nhật tank + va chạm tường
    for (auto& t : tanks) {
        int oldX = t.getX();
        int oldY = t.getY();
        t.update();
        // Kiểm tra va chạm
        CollisionManager::handleTankWallCollision(t, maze.getWalls(), oldX, oldY);
    }

    // Cập nhật bullet
    unsigned int now = SDL_GetTicks();
    for (auto& b : bullets) {
        // Kiểm tra tuổi thọ (8 giây = 8000 ms)
        if (now - b.getSpawnTime() > 8000) {
            b.setDead(true);
            continue;
        }
        // Di chuyển + va chạm tường
        // (Trong handleBulletWallCollision, ta gọi b.update() => cẩn thận double-update)
        // Hoặc ta update trước, rồi collision sau
        b.update();
        CollisionManager::handleBulletWallCollision(b, maze.getWalls());
    }

    // Check bullet-tank
    for (auto& b : bullets) {
        for (auto& t : tanks) {
            if (CollisionManager::handleBulletTankCollision(b, t)) {
                // Đạn trúng tank
                b.setDead(true);
                // Xử lý tank (VD: game over, -hp, v.v.)
            }
        }
    }

    // Xóa bullet dead
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
                       [](const Bullet& bb){ return bb.isDead(); }),
        bullets.end()
    );
}

void Game::render() {
    // Xóa màn hình (màu đen)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Vẽ mê cung
    maze.render(renderer);

    // Vẽ tank
    for (auto& t : tanks) {
        t.render(renderer);
    }

    // Vẽ bullet
    for (auto& b : bullets) {
        b.render(renderer);
    }

    // Present
    SDL_RenderPresent(renderer);
}

void Game::clean() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}
