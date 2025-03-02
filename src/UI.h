#ifndef UI_H
#define UI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include "settings.h"
#include "settings.h"  

struct MenuBox {
    SDL_Rect rect;
    std::string text;
    int mode; //với main menu: 1,2,3,0(quit) | game over: 1 (restart), 2 (menu)
};

void renderText(SDL_Renderer* renderer, TTF_Font* font, 
                const std::string &message, SDL_Color color, int x, int y);

int showModeMenu(SDL_Renderer* renderer);
int showGameOverMenu(SDL_Renderer* renderer, const std::vector<int>& scores, int mode);
#endif
