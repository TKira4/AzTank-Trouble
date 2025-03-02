#include "Game/Game.h"
#include "settings.h"
#include <iostream>

int main(int argc, char* argv[]) {
    Game game;
    if (!game.init("Tank Trouble with Maze", SCREEN_WIDTH, SCREEN_HEIGHT)) {
        std::cerr << "Game initialization failed." << std::endl;
        return 1;
    }
    game.run();
    game.clean();
    return 0;
}
