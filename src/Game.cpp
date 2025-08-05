#include "Game.h"
#include "SceneMain.h"
#include "SceneTitle.h"

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <fstream>

void Game::updateBackground(float deltaTime)
{
    // 针对左上角的图片，不断向下方移动
    nearStars.offset += deltaTime * nearStars.speed;
    if(nearStars.offset >= 0){
        // 一旦y坐标到达0，回到屏幕外（使得可以一直运动）
        nearStars.offset -= nearStars.height;
    }
    farStars.offset += deltaTime * farStars.speed;
    if(farStars.offset >= 0){
        farStars.offset -= farStars.height;
    }

    // SDL_Log("now pos: %f \t %f: ", nearStars.offset, farStars.offset);
}

void Game::renderBackground()
{

    // 远处
    // 一直渲染，直到图片的坐标超出窗口
    for(int posY_f = static_cast<int>(farStars.offset); posY_f < windowHeight; posY_f += farStars.height){
        for(int posX = 0; posX < windowWidth; posX += farStars.width){
            // 渲染区域
            SDL_Rect dstRect = {posX, posY_f, farStars.width, farStars.height};
            // 渲染
            SDL_RenderCopy(renderer, farStars.texture, nullptr, &dstRect);
        }
    }

    // 近处
    // 一直渲染，直到图片的坐标超出窗口
    for(int posY_n = static_cast<int>(nearStars.offset); posY_n < windowHeight; posY_n += nearStars.height){
        for(int posX = 0; posX < windowWidth; posX += nearStars.width){
            // 渲染区域
            SDL_Rect dstRect = {posX, posY_n, nearStars.width, nearStars.height};
            // 渲染
            SDL_RenderCopy(renderer, nearStars.texture, nullptr, &dstRect);
        }
    }
}

void Game::saveData()
{
    // 保存得分榜的数据
    std::ofstream file("assets\\save.dat"); // 没有文件会自动创建
    if(!file.is_open()){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to open save.dat\n");
        return;
    }
    for(const auto &pair : leaderBoard){
        file << pair.first << " " << pair.second << std::endl;
    }
}

void Game::loadData()
{
    // 加载得分数据
    std::ifstream file("assets\\save.dat");
    if(!file.is_open()){
        SDL_Log("Failed to open save.dat\n");
        return;
    }
    leaderBoard.clear();    // 清空原先的数据，这样更安全
    int score;
    std::string name;
    while(file >> score >> name){
        leaderBoard.insert({score, name});
    }

}

void Game::insertLeaderBoard(int score, std::string name)
{
    leaderBoard.insert({score, name});  // 插入键值对，所以需要用{}
    // 只保留排名最靠前的八名
    if(leaderBoard.size() > 8){
        // leaderBoard.end()是一个迭代器，指向最后一个元素的后一个位置，自减1后指向倒数第二个
        leaderBoard.erase(--leaderBoard.end()); // 删除最后一个元素
    }
}

SDL_FPoint Game::renderTextCentered(std::string text, float posY_percent, bool isTitle)
{
    // 合法性检查
    if(posY_percent < 0 || posY_percent > 1){
        posY_percent = 0.5;
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "posY_percent must be between 0 and 1, using default value 0.5\n");
    }

    SDL_Color textColor = {255, 255, 255, 255}; // 白色
    SDL_Surface* surface;
    if(isTitle){
        // surface = TTF_RenderText_Solid(titleFont, text.c_str(), textColor);
        surface = TTF_RenderUTF8_Solid(titleFont, text.c_str(), textColor);
        // 一定是要UTF8编码才可以显示中文
    }else{
        surface = TTF_RenderUTF8_Solid(textFont, text.c_str(), textColor);
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    // 计算y坐标
    int y = static_cast<int>((getWindowHeight() - surface->h) * posY_percent);
    SDL_Rect dstRect = {getWindowWidth() / 2 - surface->w / 2, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);

    //  释放资源
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    
    // 返回右上角的坐标
    return {static_cast<float>(dstRect.x + dstRect.w), static_cast<float>(y)};
}

// 默认参数的话，在.h里面写了就不要在.cpp里面再写一次
void Game::renderTextPos(std::string text, int posX, int posY, bool isLeft)
{
    SDL_Color textColor = {255, 255, 255, 255}; // 白色
    SDL_Surface* surface = TTF_RenderUTF8_Solid(textFont, text.c_str(), textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    // 左对齐或右对齐
    SDL_Rect rect;
    if(isLeft){
         rect = {posX, posY, surface->w, surface->h};   // 左对齐
    }else{
         rect = {getWindowWidth() - posX - surface->w, posY, surface->w, surface->h}; // 右对齐，传入右上角的点
    }
    SDL_RenderCopy(renderer, texture, nullptr, &rect);

    //  释放资源
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

}

Game::Game()
{
}

Game::~Game()
{
    saveData(); // 退出之前先保存数据
    clean(); // 清理资源
}

void Game::run()
{
    while(isRunning)
    {
        auto startTime = SDL_GetTicks(); // 获取当前时间戳，毫秒

        SDL_Event event; // 事件指针

        handleEvent(&event); // 处理事件

        update(deltaTime); // 更新游戏状态（逻辑）

        render(); // 渲染游戏画面

        auto endTime = SDL_GetTicks(); // 获取当前时间戳，毫秒
        auto diffTime = endTime - startTime; // 计算时间差，毫秒
        if(diffTime < frameTime){
            SDL_Delay(frameTime - diffTime); // 如果时间差小于每帧时间间隔，则等待剩余时间
            deltaTime = static_cast<float>(frameTime) / 1000.0f; // 将毫秒转换为秒
        }else{
            // 帧数不到60
            deltaTime = static_cast<float>(diffTime) / 1000.0f; // 将毫秒转换为秒
        }
    }
}

void Game::init()
{
    // 设置帧率
    frameTime = 1000 / FPS; // 每帧时间间隔，单位为毫秒
    // 初始化SDL
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_Init Error: %s\n", SDL_GetError());
        isRunning = false; // 初始化失败，停止游戏
    }

    // 创建窗口
    window = SDL_CreateWindow("SDL Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if(window == nullptr){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        isRunning = false; // 创建窗口失败，停止游戏
    }

    // 创建渲染器
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(renderer == nullptr){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        isRunning = false; // 创建渲染器失败，停止游戏
    }
    // 创建逻辑分辨率
    SDL_RenderSetLogicalSize(renderer, windowWidth, windowHeight);  // 无论窗口大小如何变化（缩放），渲染器的大小始终为windowWidth * windowHeight

    // 初始化SDL_Image
    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "IMG_Init Error: %s\n", IMG_GetError());
        isRunning = false; // 初始化SDL_Image失败，停止游戏
    }
    // 初始化SDL_TTF
    if(TTF_Init() == -1){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "TTF_Init Error: %s\n", TTF_GetError());
        isRunning = false; // 初始化SDL_TTF失败，停止游戏
    }
    // 初始化SDL_Mixer
    if(Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) != (MIX_INIT_MP3 | MIX_INIT_OGG)){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Mix_OpenAudio Error: %s\n", Mix_GetError());
        isRunning = false; // 初始化SDL_Mixer失败，停止游戏
    }

    // 打开音频设备
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Mix_OpenAudio Error: %s\n", Mix_GetError());
        isRunning = false; // 打开音频设备失败，停止游戏
    }

    // 初始化背景图片
    nearStars.texture = IMG_LoadTexture(renderer, "assets\\image\\Stars-A.png");
    if(nearStars.texture == nullptr){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "IMG_LoadTexture Error: %s\n", IMG_GetError());
        isRunning = false; // 载入背景失败，停止游戏
    }
    SDL_QueryTexture(nearStars.texture, NULL, NULL, &nearStars.width, &nearStars.height);
    nearStars.width /= 2;
    nearStars.height /= 2;
    nearStars.speed = 40;

    farStars.texture = IMG_LoadTexture(renderer, "assets\\image\\Stars-B.png");
    if(farStars.texture == nullptr){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "IMG_LoadTexture Error: %s\n", IMG_GetError());
        isRunning = false; // 载入背景失败，停止游戏
    }
    SDL_QueryTexture(farStars.texture, NULL, NULL, &farStars.width, &farStars.height);
    farStars.width /= 2;
    farStars.height /= 2;
    farStars.speed = 20;

    // SDL_Log("speed: %d %d", farStars.speed, nearStars.speed);

    // 载入字体
    titleFont = TTF_OpenFont("assets\\font\\VonwaonBitmap-16px.ttf", 64);
    textFont = TTF_OpenFont("assets\\font\\VonwaonBitmap-12px.ttf", 32);
    if(titleFont == nullptr || textFont == nullptr){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "TTF_OpenFont Error: %s\n", TTF_GetError());
        isRunning = false; // 载入字体失败，停止游戏
    }

    // 载入得分
    loadData();


    // 初始化当前场景
    currentScene = new SceneTitle(); // 创建一个主场景对象
    currentScene->init(); // 初始化当前场景
}

void Game::clean()
{
    // 清理场景
    if(currentScene != nullptr)
    {
        currentScene->clean(); // 清理当前场景
        delete currentScene; // 删除当前场景对象
    }

    // 清理SDL_Image
    IMG_Quit();

    SDL_DestroyRenderer(renderer); // 销毁渲染器
    SDL_DestroyWindow(window); // 销毁窗口
    SDL_Quit(); // 退出SDL
    
    // 清理SDL_Mixer
    Mix_CloseAudio(); // 关闭音频设备
    Mix_Quit(); // 退出SDL_Mixer

    // 清理字体
    TTF_CloseFont(titleFont); // 关闭字体
    TTF_CloseFont(textFont); // 关闭字体

    // 清理SDL_TTF
    TTF_Quit();

    // 清理背景
    if(nearStars.texture != nullptr)
    {
        SDL_DestroyTexture(nearStars.texture); // 销毁背景纹理
    }
    if(farStars.texture != nullptr)
    {
        SDL_DestroyTexture(farStars.texture); // 销毁背景纹理
    }
}

void Game::changeScene(Scene *scene)
{
    if(currentScene != nullptr)
    {
        currentScene->clean(); // 清理当前场景
        delete currentScene; // 删除当前场景对象
    }

    currentScene = scene; // 设置当前场景
    currentScene->init(); // 初始化当前场景

}

void Game::handleEvent(SDL_Event *event)
{
    // 处理事件
    while(SDL_PollEvent(event))
    {
        if(event->type  == SDL_QUIT)
        {
            isRunning = false; // 设置游戏运行状态为false，退出游戏循环
        }
        // 是否全屏
        if(event->type == SDL_KEYDOWN){
            if(event->key.keysym.sym == SDLK_F4)
            {
                if(!isFullScreen){
                    // 在设定全屏如果不改变窗口的宽和高，需要在init函数中重新设置
                    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                    isFullScreen = true;
                }else{
                    SDL_SetWindowFullscreen(window, 0); // 退出全屏模式
                    isFullScreen = false;
                }
            }
        }
        currentScene->handleEvent(event);
    }
}

void Game::update(float deltaTime)
{
    updateBackground(deltaTime); // 更新背景
    currentScene->update(deltaTime); // 更新当前场景
}

void Game::render()
{
    SDL_RenderClear(renderer); // 清除渲染器

    // 在渲染场景之前，渲染背景，确保背景在最下层
    renderBackground();

    currentScene->render(); // 调用当前场景的渲染函数

    SDL_RenderPresent(renderer); // 显示渲染器内容
}