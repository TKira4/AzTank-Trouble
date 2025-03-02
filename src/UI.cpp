#include "UI.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

//render text căn giữa
void renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string &message, SDL_Color color, int x, int y) {
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, message.c_str(), color);
    if (!surface) {
        SDL_Log("TTF_RenderUTF8_Blended error: %s", TTF_GetError());
        return;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        SDL_Log("SDL_CreateTextureFromSurface error: %s", SDL_GetError());
        return;
    }
    int tw, th;
    SDL_QueryTexture(texture, nullptr, nullptr, &tw, &th);
    SDL_Rect dest = { x - tw / 2, y - th / 2, tw, th };
    SDL_RenderCopy(renderer, texture, nullptr, &dest);
    SDL_DestroyTexture(texture);
}

// Hiển thị Main Menu với 4 box: "1 Player + Bots", "2 Players", "2 Players + 2 Bots", "Quit"
// 0: Quit, Mode: 1,2,3
int showModeMenu(SDL_Renderer* renderer) {
    if (TTF_WasInit() == 0) {
        if (TTF_Init() == -1) {
            SDL_Log("TTF_Init error: %s", TTF_GetError());
            return 0;
        }
    }
    TTF_Font* font = TTF_OpenFont("assets/fonts/Arial.ttf", 24);
    if (!font) {
        SDL_Log("TTF_OpenFont error: %s", TTF_GetError());
        return 0;
    }
    
    const int boxWidth = 300;
    const int boxHeight = 80;
    const int gap = 20;
    int totalHeight = 4 * boxHeight + 3 * gap;
    int startY = (SCREEN_HEIGHT - totalHeight) / 2;
    
    //4 box menu
    std::vector<MenuBox> boxes(4);
    boxes[0].rect = { (SCREEN_WIDTH - boxWidth) / 2, startY, boxWidth, boxHeight };
    boxes[0].text = "1 Player + Bots";
    boxes[0].mode = 1;
    
    boxes[1].rect = { (SCREEN_WIDTH - boxWidth) / 2, startY + boxHeight + gap, boxWidth, boxHeight };
    boxes[1].text = "2 Players";
    boxes[1].mode = 2;
    
    boxes[2].rect = { (SCREEN_WIDTH - boxWidth) / 2, startY + 2 * (boxHeight + gap), boxWidth, boxHeight };
    boxes[2].text = "2 Players + 2 Bots";
    boxes[2].mode = 3;
    
    boxes[3].rect = { (SCREEN_WIDTH - boxWidth) / 2, startY + 3 * (boxHeight + gap), boxWidth, boxHeight };
    boxes[3].text = "Quit";
    boxes[3].mode = 0;
    
    SDL_Event e;
    int selectedMode = 0;
    int mouseX = 0, mouseY = 0;
    bool done = false;
    
    while (!done) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                selectedMode = 0;
                done = true;
            } else if (e.type == SDL_MOUSEMOTION) {
                SDL_GetMouseState(&mouseX, &mouseY);
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    for (const auto &box : boxes) {
                        SDL_Point p = { mouseX, mouseY };
                        if (SDL_PointInRect(&p, &box.rect)) {
                            selectedMode = box.mode;
                            done = true;
                            break;
                        }
                    }
                }
            }
        }
        
        // background
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);
        
        // title
        renderText(renderer, font, "Tank Trouble", {255, 255, 255, 255}, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4);
        
        // box menu and hover
        for (const auto &box : boxes) {
            SDL_Color boxColor = {70, 70, 70, 255};
            SDL_Point p = { mouseX, mouseY };
            if (SDL_PointInRect(&p, &box.rect)) {
                boxColor = {100, 100, 100, 255};
            }
            SDL_SetRenderDrawColor(renderer, boxColor.r, boxColor.g, boxColor.b, boxColor.a);
            SDL_RenderFillRect(renderer, &box.rect);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &box.rect);
            renderText(renderer, font, box.text, {255, 255, 255, 255},
                       box.rect.x + boxWidth / 2, box.rect.y + boxHeight / 2);
        }
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    
    TTF_CloseFont(font);
    return selectedMode;
}

// Hiển thị Game Over Menu với thông báo chiến thắng và 2 box:1 "Restart", 2 "Return to Menu"
int showGameOverMenu(SDL_Renderer* renderer, const std::vector<int> &scores, int mode) {
    if (TTF_WasInit() == 0) {
        if (TTF_Init() == -1) {
            SDL_Log("TTF_Init error: %s", TTF_GetError());
            return 2;
        }
    }
    TTF_Font* font = TTF_OpenFont("assets/fonts/Arial.ttf", 24);
    if (!font) {
        SDL_Log("TTF_OpenFont error: %s", TTF_GetError());
        return 2;
    }
    
    SDL_Color textColor = {255, 255, 255, 255};
    renderText(renderer, font, "Game Over", textColor, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 6);
    
    std::vector<std::string> scoreTexts;
    // Sử dụng mode để xác định nhãn và số lượng dòng điểm số
    if (mode == 1) {
        // Mode 1: 1 Player + Bot
        scoreTexts.push_back("Player 1 Score: " + std::to_string(scores[0]));
        scoreTexts.push_back("AI Score: " + std::to_string(scores[1]));
    } else if (mode == 2) {
        // Mode 2: 2 Players
        scoreTexts.push_back("Player 1 Score: " + std::to_string(scores[0]));
        scoreTexts.push_back("Player 2 Score: " + std::to_string(scores[1]));
    } else if (mode == 3) {
        // Mode 3: 2 Players + 2 Bots
        scoreTexts.push_back("Player 1 Score: " + std::to_string(scores[0]));
        scoreTexts.push_back("Player 2 Score: " + std::to_string(scores[1]));
        scoreTexts.push_back("Bot 1 Score: " + std::to_string(scores[2]));
        scoreTexts.push_back("Bot 2 Score: " + std::to_string(scores[3]));
    }
    
    int textY = SCREEN_HEIGHT / 4;
    for (const auto &s : scoreTexts) {
        renderText(renderer, font, s, textColor, SCREEN_WIDTH / 2, textY);
        textY += 40;
    }
    
    const int boxWidth = 300;
    const int boxHeight = 80;
    const int gap = 20;
    int startY = textY + 40;
    
    std::vector<MenuBox> boxes(2);
    boxes[0].rect = { (SCREEN_WIDTH - boxWidth) / 2, startY, boxWidth, boxHeight };
    boxes[0].text = "Restart";
    boxes[0].mode = 1;
    
    boxes[1].rect = { (SCREEN_WIDTH - boxWidth) / 2, startY + boxHeight + gap, boxWidth, boxHeight };
    boxes[1].text = "Return to Menu";
    boxes[1].mode = 2;
    
    SDL_Event e;
    int selectedChoice = 0;
    int mouseX = 0, mouseY = 0;
    bool done = false;
    
    while (!done) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                selectedChoice = 2;
                done = true;
            } else if (e.type == SDL_MOUSEMOTION) {
                SDL_GetMouseState(&mouseX, &mouseY);
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    for (const auto &box : boxes) {
                        SDL_Point p = { mouseX, mouseY };
                        if (SDL_PointInRect(&p, &box.rect)) {
                            selectedChoice = box.mode;
                            done = true;
                            break;
                        }
                    }
                }
            }
        }
        
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);
        
        renderText(renderer, font, "Game Over", textColor, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 6);
        int currentY = SCREEN_HEIGHT / 4;
        for (const auto &s : scoreTexts) {
            renderText(renderer, font, s, textColor, SCREEN_WIDTH / 2, currentY);
            currentY += 40;
        }
        
        for (const auto &box : boxes) {
            SDL_Color boxColor = {70, 70, 70, 255};
            SDL_Point p = { mouseX, mouseY };
            if (SDL_PointInRect(&p, &box.rect)) {
                boxColor = {100, 100, 100, 255};
            }
            SDL_SetRenderDrawColor(renderer, boxColor.r, boxColor.g, boxColor.b, boxColor.a);
            SDL_RenderFillRect(renderer, &box.rect);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &box.rect);
            renderText(renderer, font, box.text, {255, 255, 255, 255},
                       box.rect.x + boxWidth / 2, box.rect.y + boxHeight / 2);
        }
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    
    TTF_CloseFont(font);
    return selectedChoice;
}
