#ifndef SCENE_TITLE_H
#define SCENE_TITLE_H



#include "Scene.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>


class SceneTitle : public Scene
{
public:
    // 公有函数
    void init() override;
    void update(float deltaTime) override;
    void render() override;
    void clean() override;
    void handleEvent(SDL_Event* event) override;

private:
    Mix_Music* bgm; // 背景音乐
    float timer = 0.0f; // 计时器

};




#endif // SCENE_TITLE_H