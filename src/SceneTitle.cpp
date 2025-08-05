#include "SceneTitle.h"
#include "SceneMain.h"
#include "Game.h"
#include <string>



void SceneTitle::init()
{
    // 载入bgm并且播放
    bgm = Mix_LoadMUS("assets\\music\\06_Battle_in_Space_Intro.ogg");
    if(bgm == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not load music: %s", Mix_GetError());
    }
    Mix_PlayMusic(bgm, -1);
}

void SceneTitle::update(float deltaTime)
{
    timer += deltaTime; // deltaTime本身就是以秒计时的
    if(timer > 1.5f){
        timer = 0.0f;   // 重置计时器
    }
}

void SceneTitle::render()
{
    // 将渲染文字封装成了一个函数
    // 标题
    std::string title = "SDL 太空战机";
    game.renderTextCentered(title, 0.3f, true);
    // 提示
    std::string prompt = "Enter space to start";    // 唉
/*  
有个叫wva的人遇到过类似问题，他向微软提交了此bug
http://connect.microsoft.com/VisualStudio/feedback/details/341454/compile-error-with-source-file-containing-utf8-strings-in-cjk-system-locale
根据Visual C++ Compiler Team员工的解释：
The compiler when faced with a source file that does not have a BOM the compiler reads ahead a certain distance into the file to see if it can detect any Unicode characters 
- it specifically looks for UTF-16 and UTF-16BE 
- if it doesn't find either then it assumes that it has MBCS. 
I suspect that in this case that in this case it falls back to MBCS and this is what is causing the problem.
看见了吧，对于那些没有BOM的文件设计就是这样的。从语气上看，他们编译器小组也不打算修改设计。所以呢，在VC上使用“无签名的UTF-8”编码的文件，你就是在抱着一颗不定时炸弹玩耍。因为你永远都不敢确定哪些词能通过编译，哪些不能！
 
如果要硬编码字符串，即便是字符编码转换也不一定能帮不上你。一旦你为此增加了字符编码转换的代码，那么也意味着可移植性降低了。因为这从根本上是编译器决定的。
*/

    // 实现文字闪烁效果
    if(timer < 0.8f){
        game.renderTextCentered(prompt, 0.8f, false);
    }


}

void SceneTitle::clean()
{
    if(bgm != NULL){
        Mix_HaltMusic();
        Mix_FreeMusic(bgm);
        bgm = NULL;
    }
}

void SceneTitle::handleEvent(SDL_Event *event)
{
    if(event->type == SDL_KEYDOWN){
        if(event->key.keysym.sym == SDLK_SPACE){
            auto sceneMain = new SceneMain();
            // sceneMain->init();
            // 要先初始化场景再切换，但是已经在changeScene中初始化了
            // 反复初始化可能会导致内存泄漏
            game.changeScene(sceneMain);
        }
    }
}
