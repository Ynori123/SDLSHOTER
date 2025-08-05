#include "SceneEnd.h"
#include "Game.h"
#include "SceneMain.h"


void SceneEnd::init()
{
    // 载入音乐
    bgm = Mix_LoadMUS("assets\\music\\hhhh.mp3");
    if(!bgm){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load music: %s", Mix_GetError());
    }
    // 播放音乐
    Mix_PlayMusic(bgm, -1);

    isTyping = true;
    // 打开文字输入模式
    // 做个if判断，更加安全
    if(!SDL_IsTextInputActive()){   // 如果不是可用状态，尝试启动并且激活
        // 启动文本输入模式的时候会顺带激活
        SDL_StartTextInput();
        if(!SDL_IsTextInputActive()){   //如果还是不可用，则输出错误信息
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to start text input: %s", SDL_GetError());
        }
    }
}

void SceneEnd::update(float deltaTime)
{
    // 减去deltaTime
    blinkTimer -= deltaTime;
    // 如果blinkTimer小于0，则重新赋值为1.0f
    if(blinkTimer < 0){
        blinkTimer = 1.0f;
    }

}

void SceneEnd::render()
{
    if(isTyping){
        renderPhaseOne();
    }else{
        renderPhaseTwo();
    }

}

void SceneEnd::clean()
{
    // 清理音乐
    if(bgm != nullptr){
        Mix_HaltMusic();
        Mix_FreeMusic(bgm);
    }
}

void SceneEnd::handleEvent(SDL_Event* event)
{
    if(isTyping){
        // 获取事件的文字内容
        if(event->type == SDL_TEXTINPUT){
            name += event->text.text;   // text事件的text字段是一个UTF-8编码的字符串
        }
        // 回车关闭
        if(event->type == SDL_KEYDOWN){
            if(event->key.keysym.scancode == SDL_SCANCODE_RETURN){
                if(name == ""){
                    name = "匿名玩家";
                }
                // 添加到得分榜
                game.insertLeaderBoard(game.getFinalScore(), name);
                isTyping = false;
                SDL_StopTextInput();
            }
        }
        // 删除键
        if(event->type == SDL_KEYDOWN){
            if(event->key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
                // 只有不为空的时候才能删除，不然会崩溃
                // if(name != ""){
                //     name.pop_back();
                // }
                // 还是利用新的函数吧
                if(name != ""){
                    removeLastUTF8Char(name);
                }
            }
        }
    }else{
        if(event->type == SDL_KEYDOWN){
            if(event->key.keysym.scancode == SDL_SCANCODE_SPACE){
                auto scene = new SceneMain();
                game.changeScene(scene);
            }
        }
    }
}

void SceneEnd::renderPhaseOne()
{
    auto score = game.getFinalScore();
    std::string scoreText = "你的分数是：" + std::to_string(score);
    std::string gameOverText = "游戏结束";
    std::string hintText = "游戏结束，请输入你的名字，按回车键确认：";

    game.renderTextCentered(scoreText, 0.1, false);
    game.renderTextCentered(gameOverText, 0.35, true);
    game.renderTextCentered(hintText, 0.5, false);

    // 渲染名字以及光标
    if(name != ""){
        // 接收名字部分的右上角坐标，用来渲染名字后面的 _ 光标
        SDL_FPoint p = game.renderTextCentered(name, 0.75, false);
        // 渲染光标
        if(blinkTimer > 0.5f){
            game.renderTextPos("_", p.x, p.y);
        }
    }else{
        // 渲染光标
        if(blinkTimer > 0.5f){
            game.renderTextCentered("_", 0.75, false);
        }   
    }
}

void SceneEnd::renderPhaseTwo()
{
    // 渲染得分榜
    game.renderTextCentered("Rank", 0.05, true);
    auto posY = 0.1 * game.getWindowHeight();   //第一个名字的位置 
    int i = 1;  // 序号
    int offset = 45;
    for(auto item : game.getLeaderBoard()){
        // 获取文字
        std::string name = std::to_string(i) + " . " + item.second;
        std::string score = std::to_string(item.first);
        // 渲染文字
        game.renderTextPos(name, 100, posY + i * offset);   // 左对齐
        game.renderTextPos(score, 100, posY + i * offset, false);   // 右对齐，x从右边开始数
        i++;
    }
    // Hint: Restart Game by enter space key
    if(blinkTimer > 0.5f){
    game.renderTextCentered("Enter space key to restart game.", 0.85, false);
    }
}

void SceneEnd::removeLastUTF8Char(std::string &str)
{
    if(str.empty()) return;

    auto lastchar = str.back(); // 获取最后一个字符
    // 删除中文后续字节
    // 为了防止运算优先级出现错误，记得加括号
    if((lastchar & 0b10000000) == 0b10000000){  // 0b开头是表示二进制格式
        // 是中文字符，先删掉
        str.pop_back();
        // 一直删直到是中文字符的首字节
        while((str.back() & 0b11000000) != 0b11000000){ // 11xxxxxx是中文字符的首字节
            // 当找到中文的首字节时，跳出循环，不会在循环里面被删除，但是在接下来会被删除
            str.pop_back();
        }

    }
    // 否则，不是中文后续字节或者是英文，都是单字节，直接删除
    str.pop_back();

}
