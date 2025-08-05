#ifndef SCENE_H
#define SCENE_H

#include <SDL.h>

class Game; // 前向声明

// Scene base class
class Scene
{
public:
    // 构造函数和析构函数
    Scene();
    virtual ~Scene() = default;

    // 定义接口（真正的共有函数）
    // 虚函数， = 0 表示纯虚函数，子类必须实现这些函数
    virtual void init() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void clean() = 0;
    virtual void handleEvent(SDL_Event* event) = 0;

protected:
    Game& game;

};



#endif