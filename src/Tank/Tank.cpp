#include "Tank.h"
#include <cmath>
#include <iostream>

#ifndef CELL_SIZE
#define CELL_SIZE 48
#endif

static const float PI = 3.14159f;

Tank::Tank(float x, float y, SDL_Color color)
    : x(x), y(y), color(color), alive(true), texture(nullptr), turretTexture(nullptr)
{
    width = CELL_SIZE * 0.6f;
    height = CELL_SIZE * 0.6f;
    speed = 100.0f;
    angle = -PI / 2; //xe tank ban dau huong len tren
}

Tank::~Tank() { }

SDL_Rect Tank::getCollisionRect() const {
    float scale = 0.7f;
    int cWidth = (int)(width * scale);
    int cHeight = (int)(height * scale);
    SDL_Point c = getCenter();
    return SDL_Rect{ c.x - cWidth/2, c.y - cHeight/2, cWidth, cHeight };
}

SDL_Rect Tank::getRect() const {
    return SDL_Rect{ (int)x, (int)y, (int)width, (int)height };
}

SDL_Point Tank::getCenter() const {
    return SDL_Point{ (int)(x + width/2), (int)(y + height/2) };
}

bool Tank::collidesWithMaze(const SDL_Rect &rect, Maze &maze) const {
    return maze.collides(rect);
}

void Tank::update(const Uint8* keyState, float deltaTime, Maze &maze) {
    float turnSpeed = PI;
    if (keyState[SDL_SCANCODE_A])
        angle -= turnSpeed * deltaTime;
    if (keyState[SDL_SCANCODE_D])
        angle += turnSpeed * deltaTime;
    
    float moveX = 0, moveY = 0;
    if (keyState[SDL_SCANCODE_W]) {
        moveX += cos(angle);
        moveY += sin(angle);
    }
    if (keyState[SDL_SCANCODE_S]) {
        moveX -= cos(angle);
        moveY -= sin(angle);
    }
    float mag = sqrt(moveX*moveX + moveY*moveY);
    if (mag > 0) {
        moveX /= mag;
        moveY /= mag;
    }
    float dispX = moveX * speed * deltaTime;
    float dispY = moveY * speed * deltaTime;
    float origX = x, origY = y;
    x += dispX;
    y += dispY;
    if (collidesWithMaze(getCollisionRect(), maze)) {
        x = origX;
        y = origY;
    }
}

void Tank::updatePlayer2(const Uint8* keyState, float deltaTime, Maze & maze){
    float turnSpeed = PI;
    if (keyState[SDL_SCANCODE_LEFT])
        angle -= turnSpeed * deltaTime;
    if (keyState[SDL_SCANCODE_RIGHT])
        angle += turnSpeed * deltaTime;
    
    float moveX = 0, moveY = 0;
    if (keyState[SDL_SCANCODE_UP]) {
        moveX += cos(angle);
        moveY += sin(angle);
    }
    if (keyState[SDL_SCANCODE_DOWN]) {
        moveX -= cos(angle);
        moveY -= sin(angle);
    }
    float mag = sqrt(moveX*moveX + moveY*moveY);
    if (mag>0){
        moveX /= mag;
        moveY /= mag;
    }
    float dispX = moveX * speed * deltaTime;
    float dispY = moveY * speed * deltaTime;
    float origX = x, origY = y;
    x += dispX;
    y += dispY;
    if (collidesWithMaze(getCollisionRect(), maze)) {
        x = origX;
        y = origY;
    }
}


void Tank::initTexture(SDL_Renderer* renderer) {
    SDL_Rect rect = getRect();
    //texture cho tank's body
    texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_RGBA8888,
                                SDL_TEXTUREACCESS_TARGET,
                                rect.w, rect.h);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect border = { 0, 0, rect.w, rect.h };
    SDL_RenderDrawRect(renderer, &border);
    SDL_SetRenderTarget(renderer, nullptr);
    
    //texture cho sung'
    int turretW = rect.w / 2;
    int turretH = rect.h / 4;
    turretTexture = SDL_CreateTexture(renderer,
                                      SDL_PIXELFORMAT_RGBA8888,
                                      SDL_TEXTUREACCESS_TARGET,
                                      turretW, turretH);
    SDL_SetTextureBlendMode(turretTexture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, turretTexture);

    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, nullptr);
}

void Tank::render(SDL_Renderer* renderer) {
    SDL_Rect dest = getRect();
    SDL_Point pivot = { dest.w / 2, dest.h / 2 };
    double degrees = angle * 180.0 / PI;

    //xe xoay theo angle
    SDL_RenderCopyEx(renderer, texture, NULL, &dest, degrees, &pivot, SDL_FLIP_NONE);

    int turretW = dest.w / 2;
    int turretH = dest.h / 4;
    SDL_Rect turretDest;
    turretDest.w = turretW;
    turretDest.h = turretH;

    SDL_Point center = getCenter();
    float offset = dest.h * 0.5f;  
    turretDest.x = (int)(center.x + cos(angle) * offset - turretW / 2);
    turretDest.y = (int)(center.y + sin(angle) * offset - turretH / 2);

    SDL_Point turretPivot = { turretW / 2, turretH / 2 };

    SDL_RenderCopyEx(renderer, turretTexture, NULL, &turretDest, degrees, &turretPivot, SDL_FLIP_NONE);
}
