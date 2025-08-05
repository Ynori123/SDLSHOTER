#ifndef SCENEEND_H
#define SCENEEND_H

#include "Scene.h"
#include <string>
#include <SDL_mixer.h>

class SceneEnd : public Scene{
public:
    // 公有函数
    void init() override;
    void update(float deltaTime) override;
    void render() override;
    void clean() override;
    void handleEvent(SDL_Event* event) override;

private:

    bool isTyping = true;   // 页面开关，用来切换打字和积分榜
    std::string name = "";  // 玩家名字
    float blinkTimer = 1.0f;  // 打字光标闪烁时间
    Mix_Music* bgm = nullptr;  // 背景音乐

    void renderPhaseOne();  // 打字
    void renderPhaseTwo();  // 积分榜
    void removeLastUTF8Char(std::string &str);  // 删除最后一个UTF-8字符
};

#endif // SCENEEND_H