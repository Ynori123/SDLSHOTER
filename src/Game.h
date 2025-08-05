#ifndef GAME_H
#define GAME_H

#include "Scene.h"
#include "Object.h"
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <map>

// 单例设计模式
class Game{
public:
    static Game& getInstance(){
        static Game instance;
        return instance;
    }

    ~Game();
    // 游戏循环
    void run();
    void init();
    void clean();
    void changeScene(Scene* scene);

    // 拆分run()函数
    void handleEvent(SDL_Event* event);
    void update(float deltaTime);
    void render();

    // Getters
    SDL_Window* getWindow() {return window;}
    SDL_Renderer* getRenderer() {return renderer;}
    int getWindowWidth() {return windowWidth;}
    int getWindowHeight() {return windowHeight;}
    int getFinalScore() {return finalScore;}
     // 获取得分榜
     std::multimap<int, std::string, std::greater<int>>& getLeaderBoard() {return leaderBoard;} // 返回得分榜的引用，避免拷贝占内存

    // Setters
    void setFinalScore(int score) {finalScore = score;}
    void insertLeaderBoard(int score, std::string name);    // 插入得分榜

    // 渲染文字工具，返回右上角的坐标
    // 下面这个函数同时具备了渲染文字和返回右上角坐标的功能，违反了单一职责原则，但是为了方便，暂时这样写
    SDL_FPoint renderTextCentered(std::string text, float posY_percent, bool isTitle);   // 将文字渲染在正中间，百分比，0在最上方，1在最下方
    void renderTextPos(std::string text, int posX, int posY, bool isLeft = true); // 在指定位置渲染文字，

private:
    Game();
    // 禁止拷贝构造和赋值操作
    Game(const Game&) = delete;
    Game& operator = (const Game&) = delete;

    // 整个游戏通用的字体
    TTF_Font* titleFont = nullptr;
    TTF_Font* textFont = nullptr;

    // 更新背景
    void updateBackground(float deltaTime);
    void renderBackground();

    // 数据
    void saveData();
    void loadData();
   

    int finalScore = 0;
    std::multimap<int, std::string, std::greater<int>> leaderBoard;    // 得分榜，默认由小到大排序，加入std::greater<int>可以使其由大到小排序

    bool isFullScreen = false;
    bool isRunning = true;
    Scene* currentScene = nullptr; // 当前场景指针
    SDL_Window* window = nullptr; // 窗口指针，因为窗口需要经常用到
    SDL_Renderer* renderer = nullptr; // 渲染器指针，用于渲染图形

    // 窗口大小
    int windowWidth = 800;
    int windowHeight = 600;

    // 控制主循环帧率
    int FPS = 60; // 每秒帧数
    Uint32 frameTime; //记录期望的每帧耗时
    float deltaTime; // 帧间隔时间（实际的每帧耗时，s）

    Background nearStars;
    Background farStars;
};



#endif 
