#include "SDL2/SDL_ttf.h"
#include <string>
const int CELL_SIZE   = 48;
const int MAZE_ROWS   = 13;
const int MAZE_COLS   = 13;
const int SCREEN_WIDTH  = MAZE_COLS * CELL_SIZE;
const int SCREEN_HEIGHT = MAZE_ROWS * CELL_SIZE;

void showNotification(SDL_Renderer* renderer, const std::string &message) {
    // Khởi tạo SDL_ttf nếu chưa khởi tạo
    if (TTF_WasInit() == 0) {
        if (TTF_Init() == -1) {
            SDL_Log("TTF_Init: %s", TTF_GetError());
            return;
        }
    }
    
    // Load font (chọn font bạn muốn, ví dụ: Arial.ttf ở cùng thư mục, kích thước 24)
    TTF_Font* font = TTF_OpenFont("assets/fonts/Arial.ttf", 24);
    if (!font) {
        SDL_Log("TTF_OpenFont: %s", TTF_GetError());
        return;
    }
    
    // Tạo surface từ text
    SDL_Color textColor = { 255, 0, 0, 255 }; // Màu đỏ, thay đổi theo ý muốn
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, message.c_str(), textColor);
    if (!textSurface) {
        SDL_Log("TTF_RenderUTF8_Blended: %s", TTF_GetError());
        TTF_CloseFont(font);
        return;
    }
    
    // Tạo texture từ surface
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    
    if (!textTexture) {
        SDL_Log("SDL_CreateTextureFromSurface: %s", SDL_GetError());
        TTF_CloseFont(font);
        return;
    }
    
    // Lấy kích thước text
    int texW = 0;
    int texH = 0;
    SDL_QueryTexture(textTexture, NULL, NULL, &texW, &texH);
    
    // Vẽ background bán trong suốt (overlay) cho thông báo
    SDL_Rect overlayRect = { (SCREEN_WIDTH - texW) / 2 - 10, (SCREEN_HEIGHT - texH) / 2 - 10, texW + 20, texH + 20 };
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180); // Màu đen với alpha 180\n    SDL_RenderFillRect(renderer, &overlayRect);
    
    // Vẽ text lên overlay
    SDL_Rect dstRect = { (SCREEN_WIDTH - texW) / 2, (SCREEN_HEIGHT - texH) / 2, texW, texH };
    SDL_RenderCopy(renderer, textTexture, NULL, &dstRect);
    
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
    
    // Cập nhật renderer để hiển thị thông báo
    SDL_RenderPresent(renderer);
    
    // Đợi 2 giây rồi xóa thông báo (có thể điều chỉnh thời gian)\n    SDL_Delay(2000);
}