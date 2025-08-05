#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

# include "src\\Game.h"

// Windows下的main函数必须带参数，否则会和SDL2的main函数冲突
int main(int argc, char *argv[]) {
    Game &game = Game::getInstance();
    game.init(); // 初始化游戏
    game.run();

    return 0;
}