#include "SceneMain.h"
#include "SceneTitle.h"
#include "SceneEnd.h"
#include "Game.h"
#include <string>


// 随机数生成，在头文件里面有了
// #include <random>

// 采用初始化列表来初始化成员变量
// 这样可以避免在构造函数体内进行赋值操作
// 在基类里面已经初始化过了
// SceneMain::SceneMain() : game(Game::getInstance())
// {
// }

// class SceneTitle;
// 你以为需要前向声明？不需要()，因为这里是cpp文件

SceneMain::~SceneMain()
{
}

void SceneMain::init()
{
    // 初始化：生成随机数
    std::random_device rd;  // 种子
    gen = std::mt19937(rd());  // 引擎
    std::uniform_real_distribution<float> dist(0.0f, 1.0f); // 均匀分布
    // auto r = dist(gen);  // 生成随机数


    // 玩家初始化
    player.shieldTexture = IMG_LoadTexture(game.getRenderer(), "assets\\image\\shield.png");
    player .texture = IMG_LoadTexture(game.getRenderer(), "assets\\image\\SpaceShip.png");
    // 将传入图片的大小设置为玩家纹理的宽高
    SDL_QueryTexture(player.texture, NULL, NULL, &player.width, &player.height);
    player.width /= 5;  // 玩家太大，缩小一点
    player.height /= 5;
    // 初始化玩家坐标，在屏幕下方的中心位置
    player.position.x = (game.getWindowWidth() - player.width) / 2;
    player.position.y = game.getWindowHeight() - player.height;

    // 初始化模板
    // 子弹
    projectilePlayerTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets\\image\\laser-1.png");
    SDL_QueryTexture(projectilePlayerTemplate.texture, NULL, NULL, &projectilePlayerTemplate.width, &projectilePlayerTemplate.height);
    projectilePlayerTemplate.width /= 4;  // 玩家缩小了，建议子弹也缩小
    projectilePlayerTemplate.height /= 4;
    // 敌机
    enemyTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets\\image\\insect-2.png");
    SDL_QueryTexture(enemyTemplate.texture, NULL, NULL, &enemyTemplate.width, &enemyTemplate.height);
    enemyTemplate.width /= 3;  // 缩小
    enemyTemplate.height /= 3;
    // 敌机子弹
    enemyProjectileTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets\\image\\laser-2.png");
    SDL_QueryTexture(enemyProjectileTemplate.texture, NULL, NULL, &enemyProjectileTemplate.width, &enemyProjectileTemplate.height);
    enemyProjectileTemplate.width /= 3;  // 缩小
    enemyProjectileTemplate.height /= 3;
    // 爆炸
    explosionTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets\\effect\\explosion.png");
    SDL_QueryTexture(explosionTemplate.texture, NULL, NULL, &explosionTemplate.width, &explosionTemplate.height);
    explosionTemplate.totalFrames = explosionTemplate.width / explosionTemplate.height; // 获取总帧数
    // SDL_Log("explosionTemplate.totalFrames = %d", explosionTemplate.totalFrames);
    explosionTemplate.height *= 2; // 想放大，对序列帧，要修改渲染区域的函数
    explosionTemplate.width = explosionTemplate.height; // 每一帧都是正方形
    // 生命道具
    itemLifeTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets\\image\\bonus_life.png");
    SDL_QueryTexture(itemLifeTemplate.texture, NULL, NULL, &itemLifeTemplate.width, &itemLifeTemplate.height);
    itemLifeTemplate.width /= 4;
    itemLifeTemplate.height /= 4;
    itemLifeTemplate.type = ItemType::Life;  // 设置道具类型，注意这步很重要
    // 护盾道具
    itemShieldTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets\\image\\bonus_shield.png");
    SDL_QueryTexture(itemShieldTemplate.texture, NULL, NULL, &itemShieldTemplate.width, &itemShieldTemplate.height);
    itemShieldTemplate.width /= 4;
    itemShieldTemplate.height /= 4;
    itemShieldTemplate.type = ItemType::Shield;  // 设置道具类型，注意这步很重要

    // if(itemShieldTemplate.texture == nullptr){
    //     SDL_Log("itemShieldTemplate.texture == nullptr");
    // }


    // BGM初始化
    bgm = Mix_LoadMUS("assets\\music\\03_Racing_Through_Asteroids_Loop.ogg");
    if(bgm == nullptr){
        SDL_Log("Mix_LoadMUS Error: %s", Mix_GetError());
    }
    Mix_PlayMusic(bgm, -1);  // -1表示循环播放

    // 音效初始化，直接插入键值对
    sounds["player_shoot"] = Mix_LoadWAV("assets\\sound\\laser_shoot4.wav");
    sounds["enemy_shoot"] = Mix_LoadWAV("assets\\sound\\xs_laser.wav");
    sounds["player_explode"] = Mix_LoadWAV("assets\\sound\\explosion1.wav");
    sounds["enemy_explode"] = Mix_LoadWAV("assets\\sound\\explosion3.wav");
    sounds["hit"] = Mix_LoadWAV("assets\\sound\\eff11.wav");
    sounds["item"] = Mix_LoadWAV("assets\\sound\\eff5.wav");

    // 设置音效最多频道（最多同时播放多少个）
    Mix_AllocateChannels(32);

    Mix_VolumeMusic(30);  // 针对bgm设置音量，范围0-128
    Mix_Volume(-1, 30);  // 针对所有音效设置音量，范围0-128

    // ui初始化
    uiHealth = IMG_LoadTexture(game.getRenderer(), "assets\\image\\Health UI Black.png");
    uiShield = IMG_LoadTexture(game.getRenderer(), "assets\\image\\Shield UI Black.png");

    // 字体初始化
    scorefont = TTF_OpenFont("assets\\font\\VonwaonBitmap-12px.ttf", 24);

}

void SceneMain::update(float deltaTime)
{
    keyboardControl(deltaTime);    // 处理键盘输入
    updatePlayerProjectiles(deltaTime); // 更新玩家子弹

    // 生成敌机
    spawnEnemy();
    updateEnemise(deltaTime); // 更新敌机
    updateEnemyProjectiles(deltaTime); // 更新敌机子弹

    // 更新玩家状态
    updatePlayer(deltaTime);

    // 更新动画效果
    updateExplosions(deltaTime);

    // 更新道具
    updateItems(deltaTime);

    // 玩家死亡以后延迟切换结束场景
    if(isDead){
        ChangeSceneDelay(deltaTime, 3.0f); // 3秒后切换场景
        
    }

}

void SceneMain::render()
{
    // 谁先渲染，谁在图层下方
    
    // 渲染玩家子弹
    renderPlayerProjectiles();
    // 渲染敌机子弹
    renderEnemyProjectiles();
    // 渲染敌机
    renderEnemies();

    // 渲染玩家，注意，SDL_Rect需要的是int，玩家不死才渲染
    if(!isDead){
    SDL_Rect playerRect = {
            static_cast<int>(player.position.x), 
            static_cast<int>(player.position.y), 
            player.width, 
            player.height
        };  // 渲染的范围（玩家纹理大小）
    SDL_RenderCopy(game.getRenderer(), player.texture, NULL, &playerRect); // 渲染玩家纹理到渲染器上
    // 护盾纹理
    SDL_Rect shieldRect = {
            static_cast<int>(player.position.x), 
            static_cast<int>(player.position.y + player.shieldOffsetY), 
            player.width, 
            player.height
    };
    if(player.currentShield > 0){
        SDL_RenderCopy(game.getRenderer(), player.shieldTexture, NULL, &shieldRect);
    }

    }

    // 渲染道具
    renderItems();
    // 渲染动画效果
    renderExplosions();
    // 渲染ui
    renderUI();
}

void SceneMain::clean()
{
    // 清理玩家资源
    if(player.texture != nullptr){
        SDL_DestroyTexture(player.texture);
    }

    // 清理玩家子弹资源

    // 清理容器
    for(auto &projectile : projectilesPlayer){  // 注意，声明的projectilesPlayer是指针
        delete projectile;
    }
    projectilesPlayer.clear();
    // 清理模板的纹理
    if(projectilePlayerTemplate.texture != nullptr){
        SDL_DestroyTexture(projectilePlayerTemplate.texture);
    }

    // 清理敌机资源

    // 清理容器
    for(auto &enemy : enemies){
        delete enemy;
    }
    enemies.clear();
    // 清理模板的纹理
    if(enemyTemplate.texture != nullptr){
        SDL_DestroyTexture(enemyTemplate.texture);
    }

    // 清理敌机子弹资源
    // 清理容器
    for(auto &projectile : projectilesEnemies){
        delete projectile;
    }
    projectilesEnemies.clear();
    // 清理模板的纹理
    if(enemyProjectileTemplate.texture != nullptr){
        SDL_DestroyTexture(enemyProjectileTemplate.texture);
    }

    // 清理动画效果资源
    // 清理容器
    for(auto &explosion : explosions){
        delete explosion;
    }
    explosions.clear();
    // 清理模板的纹理
    if(explosionTemplate.texture != nullptr){
        SDL_DestroyTexture(explosionTemplate.texture);
    }
    
    // 清理道具
    // 清理容器
    for(auto &item : items){
        delete item;
    }
    items.clear();
    // 清理模板的纹理
    if(itemLifeTemplate.texture != nullptr){
        SDL_DestroyTexture(itemLifeTemplate.texture);
    }
    if(itemShieldTemplate.texture != nullptr){
        SDL_DestroyTexture(itemShieldTemplate.texture);
    }

    // 清理BGM
    if(bgm != nullptr){
        Mix_HaltMusic();    // 建议先停止播放，再清理资源，这样更加安全
        Mix_FreeMusic(bgm);
    }

    // 清理音效
    for(auto &sound : sounds){
        if(sound.second != nullptr){
            // second指的是值，即Mix_Chunk*
            Mix_FreeChunk(sound.second);
        }
    }

    // 清理ui
    if(uiHealth != nullptr){
        SDL_DestroyTexture(uiHealth);
    }
    if(uiShield != nullptr){
        SDL_DestroyTexture(uiShield);
    }

    // 清理字体
    if(scorefont != nullptr){
        TTF_CloseFont(scorefont);
    }

}

void SceneMain::handleEvent(SDL_Event *event)
{
    if(event->type == SDL_KEYDOWN){
        if(event->key.keysym.scancode == SDL_SCANCODE_ESCAPE){
            auto scene = new SceneTitle();
            game.changeScene(scene);
        }
    }
}

void SceneMain::keyboardControl(float deltaTime)
{
    // 如果玩家死亡，则不处理键盘输入
    if(isDead){
        return;
    }

    auto keyboardState = SDL_GetKeyboardState(NULL); // 获取当前键盘状态，返回值是一个Uint8类型的数组，表示每个按键的状态

    if(keyboardState[SDL_SCANCODE_W]){  // 按下W键，向上移动
        player.position.y -= deltaTime * player.speed; // deltaTime是帧间隔时间，speed是玩家速度
    }
    if(keyboardState[SDL_SCANCODE_S]){  // 按下S键，向下移动
        player.position.y += deltaTime * player.speed;
    }
    if(keyboardState[SDL_SCANCODE_A]){  // 按下A键，向左移动
        player.position.x -= deltaTime * player.speed;
    }
    if(keyboardState[SDL_SCANCODE_D]){  // 按下D键，向右移动
        player.position.x += deltaTime * player.speed;
    }

    // 边界检测
    if(player.position.x < 0) player.position.x = 0; // 左边界
    if(player.position.x > game.getWindowWidth() - player.width) player.position.x = game.getWindowWidth() - player.width; // 右边界
    if(player.position.y < 0) player.position.y = 0; // 上边界
    if(player.position.y > game.getWindowHeight() - player.height) player.position.y = game.getWindowHeight() - player.height; // 下边界

    // 子弹发射
    if(keyboardState[SDL_SCANCODE_SPACE]){  // 按下空格键，发射子弹
        auto currentTime = SDL_GetTicks(); // 获取当前时间，毫秒
        if(currentTime - player.lastShootTime >= player.coolDown){
            shootPlayer();
            player.lastShootTime = currentTime; // 更新
        }
    }
}

void SceneMain::shootPlayer()
{
    // 利用模板新建子弹，注意，是指针
    auto projectile = new projectilePlayer(projectilePlayerTemplate);
    // 子弹的发射位置至少要在玩家的中间位置
    projectile->position.x = player.position.x + player.width / 2 - projectile->width / 2;
    projectile->position.y = player.position.y;
    // 将子弹添加到子弹列表中
    projectilesPlayer.push_back(projectile);

    // 播放音效
    Mix_PlayChannel(0, sounds["player_shoot"], 0); // -1表示播放到下一个空闲的通道，这里设定在0号通道播放，循环0次（不循环，只播放一次）
}

void SceneMain::updatePlayerProjectiles(float deltaTime)
{
    int margin = 32; // 子弹超出屏幕上边界多少距离后删除 
    // 使用循环和迭代器，it是一个迭代器
    for(auto it = projectilesPlayer.begin(); it != projectilesPlayer.end();){

        auto projectile = *it; // 获取列表里的一个子弹

        projectile->position.y -= deltaTime * projectile->speed; // 子弹向上移动

        // 边界检测
        if(projectile->position.y + margin < 0){
            delete projectile;            // 子弹超出屏幕上边界，删除子弹
            // 注意，必须有这一句，否则it会指向一个已经删除的对象，会导致崩溃
            it = projectilesPlayer.erase(it); // 删除子弹，并返回下一个子弹的迭代器
            // 输出信息
            // SDL_Log("Player projectile was removed");
        }else{
            // 子弹碰撞检测
            bool isHit = false; // 是否击中，用于调整迭代器
            for(auto enemy : enemies){
                // 先创建矩形
                SDL_Rect projectileRect = {
                    static_cast<int>(projectile->position.x), 
                    static_cast<int>(projectile->position.y), 
                    projectile->width, 
                    projectile->height
                };
                SDL_Rect enemyRect = {
                    static_cast<int>(enemy->position.x), 
                    static_cast<int>(enemy->position.y), 
                    enemy->width,
                    enemy->height
                };
                // 判断是否碰撞
                if(SDL_HasIntersection(&projectileRect, &enemyRect)){
                    // 注意，先减少目标血量，再删除子弹和敌机，因为减少血量的时候要用到子弹的指针
                    enemy->currentHealth -= projectile->damage; // 减少敌机血量
                    delete projectile; // 碰撞，删除子弹
                    it = projectilesPlayer.erase(it);   // 删除子弹，并返回下一个子弹的迭代器
                    isHit = true;
                    // 播放音效
                    Mix_PlayChannel(-1, sounds["hit"], 0);
                    break; // 子弹销毁了，跳出循环
                }
            }
            if(!isHit){
                // 子弹没有击中而且没有超出屏幕，就继续迭代
                it++; 
            }
        }
    }
}

void SceneMain::updatePlayer(float deltaTime)
{
    if(isDead){
        return; // 如果玩家已经死亡，则不处理
    }
    // 玩家死亡
    if(player.currentHealth <= 0){

        // 爆炸
        auto currentTime = SDL_GetTicks();
        auto explosion = new Explosion(explosionTemplate);
        explosion->position.x = player.position.x - explosion->width / 2;
        explosion->position.y = player.position.y - explosion->height / 2;
        explosion->startTime = currentTime;
        explosions.push_back(explosion);

        // 播放音效
        Mix_PlayChannel(-1, sounds["player_explode"], 0);   // -1表示播放到下一个空闲的通道，循环0次（不循环，只播放一次）

        // 玩家死亡
        isDead = true;
        // 设置最终得分
        game.setFinalScore(score);

        return;
    }
    // 进行玩家和敌机的碰撞检测
    for(auto enemy : enemies){
        SDL_Rect playerRect = {
            static_cast<int>(player.position.x), 
            static_cast<int>(player.position.y), 
            player.width, 
            player.height
        };
        SDL_Rect enemyRect = {
            static_cast<int>(enemy->position.x), 
            static_cast<int>(enemy->position.y), 
            enemy->width, 
            enemy->height
        };
        if(SDL_HasIntersection(&playerRect, &enemyRect)){
            // 不怕死
            score += 1;
            if(player.currentShield == 0){
            player.currentHealth -= 1;
            enemy->currentHealth = 0;
            }else{
                player.currentShield -= 1;
                enemy->currentHealth = 0;
            }
        }
    }
}   

void SceneMain::renderPlayerProjectiles()
{
    for(auto projectile : projectilesPlayer){

        SDL_Rect projectileRect = {
            static_cast<int>(projectile->position.x), 
            static_cast<int>(projectile->position.y), 
            projectile->width, 
            projectile->height
        };
        SDL_RenderCopy(game.getRenderer(), projectile->texture, NULL, &projectileRect);
    }
}

void SceneMain::spawnEnemy()
{
    // 每秒判断一次，每帧调用
    if(dist(gen) > 1 / 60.0){
        return;
    }
    // 生成敌机
    Enemy* enemy = new Enemy(enemyTemplate);
    enemy->position.x = dist(gen) * (game.getWindowWidth() - enemy->width); //在屏幕范围内
    enemy->position.y = -enemy->height; // 在屏幕外（上方）
    enemies.push_back(enemy); // 添加到敌机列表
}

void SceneMain::updateEnemise(float deltaTime)
{
    // 获取时间戳
    auto currentTime = SDL_GetTicks();
    // 类似子弹的更新
    for(auto it = enemies.begin(); it != enemies.end();){
        auto enemy = *it;
        enemy->position.y += deltaTime * enemy->speed;
        if(enemy->position.y > game.getWindowHeight()){
            delete enemy;
            it = enemies.erase(it);
        }else{  
            // 如果没有超出屏幕就判断是否发射子弹
            // 玩家存活的时候才发射子弹
            if(enemy->lastShootTime == 0 || SDL_GetTicks() - enemy->lastShootTime > enemy->coolDown && !isDead){
                shootEnemy(enemy);
                enemy->lastShootTime = currentTime;
            }
            // 判断敌机是否爆炸
            if(enemy->currentHealth <= 0){
                enemyExplode(enemy);
                it = enemies.erase(it); // 删除敌机，并返回下一个敌机的迭代器
            }else{
                // 没有爆炸，就继续迭代
                it++;
            }
        }
    }
}


void SceneMain::renderEnemies()
{
    for(auto enemy : enemies){
        SDL_Rect enemyRect = {
            static_cast<int>(enemy->position.x), 
            static_cast<int>(enemy->position.y), 
            enemy->width, 
            enemy->height
        };
        SDL_RenderCopy(game.getRenderer(), enemy->texture, NULL, &enemyRect);
    }
}

void SceneMain::enemyExplode(Enemy *enemy)
{
    // 创建爆炸
    auto currentTime = SDL_GetTicks();
    auto explosion = new Explosion(explosionTemplate);
    explosion->position.x = enemy->position.x + enemy->width / 2 - explosion->width / 2;
    explosion->position.y = enemy->position.y + enemy->height / 2 - explosion->height / 2;
    explosion->startTime = currentTime;
    explosions.push_back(explosion);

    // 有概率掉落物品
    if(dist(gen) < 0.9f){
        dropItem(enemy);
    }

    // 播放音效
    Mix_PlayChannel(-1, sounds["enemy_explode"], 0);   // -1表示播放到下一个空闲的通道，循环0次（不循环，只播放一次）

    // 击落敌机得分
    score += 10;

    delete enemy;
}

void SceneMain::updateExplosions(float deltaTime)
{
    auto currentTime = SDL_GetTicks();
    for(auto it = explosions.begin(); it != explosions.end();){

        auto explosion = *it;
        // 计算当前帧，注意单位转化
        explosion->currentFrame = (currentTime - explosion->startTime) * explosion->FPS / 1000;

        if(explosion->currentFrame >= explosion->totalFrames){
            delete explosion;   // 爆炸播放完毕，删除
            it = explosions.erase(it);
        }else{
            it++;   // 没有播放完毕，继续迭代
        }
    }
}

void SceneMain::renderExplosions()
{
    // 要确定渲染哪一个方块（帧）
    for(auto explosion : explosions){
        // 图片帧区域
        // 由于初始化时，图片的宽高是2倍，所以这里要除以2，确保渲染的帧是一帧
        SDL_Rect explosionRect = {
            explosion->currentFrame * explosion->width, 
            0, 
            explosion->width / 2, 
            explosion->height/ 2
        };
        // 渲染区域
        SDL_Rect dstRect = {
            static_cast<int>(explosion->position.x), 
            static_cast<int>(explosion->position.y), 
            explosion->width, 
            explosion->height
        };
        // SDL_Log("explosionRect: %d\n", explosionRect.x);
        SDL_RenderCopy(game.getRenderer(), explosion->texture, &explosionRect, &dstRect);
    }
}

void SceneMain::dropItem(Enemy * enemy)
{
    auto item = new Item(itemLifeTemplate);
    // 指定物品类型
    if(dist(gen) < 0.5f){
        item = new Item(itemShieldTemplate);
        //SDL_Log("shield\n");
    }
    item->position.x = enemy->position.x + enemy->width / 2 - item->width / 2;
    item->position.y = enemy->position.y + enemy->height / 2 - item->height / 2;
    // 随机生成一个角度
    float angle = dist(gen) * 2 * M_PI;
    item->direction.x = cos(angle); // 传入弧度
    item->direction.y = sin(angle);
    items.push_back(item);
}

void SceneMain::updateItems(float deltaTime)
{
    for(auto it = items.begin(); it != items.end();){

        auto item = *it;
        // 物品移动
        item->position.x += deltaTime * item->speed * item->direction.x;
        item->position.y += deltaTime * item->speed * item->direction.y;
        // 边界反弹
        if(item->bounceCount > 0){
            if(item->position.x < 0 || item->position.x + item->width > game.getWindowWidth()){
                item->direction.x *= -1;
                item->bounceCount--;
            }
            if(item->position.y < 0 || item->position.y + item->height > game.getWindowHeight()){
                item->direction.y *= -1;
                item->bounceCount--;
            }
        }


        // 如果超出屏幕范围，删除
        if(item->position.x + item->width < 0 || item->position.x > game.getWindowWidth() ||
           item->position.y + item->height < 0 || item->position.y > game.getWindowHeight()){
            delete item;
            it = items.erase(it);
        }else{
            // 没有超出屏幕范围，就进行碰撞检测
            SDL_Rect itemRect = {
                static_cast<int>(item->position.x), 
                static_cast<int>(item->position.y), 
                item->width, 
                item->height
            };
            SDL_Rect playerRect = {
                static_cast<int>(player.position.x),
                static_cast<int>(player.position.y),
                player.width,
                player.height
            };
            // 玩家活着才可以接受物品
            if(SDL_HasIntersection(&itemRect, &playerRect) && !isDead){
                // 碰撞检测成功，玩家获得物品
                PlayerGetItem(item);
                delete item;
                it = items.erase(it);
            }else{
                // 没有碰撞，就继续迭代
                it++;
            }
        }
    }
}

void SceneMain::PlayerGetItem(Item * item)
{
    // 拾取物品得分
    score += 5;

    // 播放音效
    Mix_PlayChannel(-1, sounds["item"], 0);
    if(item->type == ItemType::Life){
        // SDL_Log("玩家拾取道具，当前生命值：%d\n", player.currentHealth);
        if(player.currentHealth < player.maxHealth){
            // SDL_Log("玩家拾取道具，当前生命值：%d\n", player.currentHealth);
            // 拾取道具，玩家生命值加1
            player.currentHealth += 1;  // 回血
        }
    }else if(item->type == ItemType::Shield){
        // SDL_Log("玩家拾取道具，当前护盾值：%d\n", player.currentShield);
        // 拾取道具，玩家护盾值加1
        if(player.currentShield < player.maxShield){
            player.currentShield += 1;
            // SDL_Log("玩家拾取道具，当前护盾值：%d\n", player.currentShield);
        }
    }
}

void SceneMain::ChangeSceneDelay(float deltaTime, float delay)
{
    timerEnd += deltaTime;
    if(timerEnd >= delay){
        auto scene = new SceneEnd();
        game.changeScene(scene);    
    }
}

void SceneMain::renderItems()
{
    for(auto item : items){
        SDL_Rect dstRect = {
            static_cast<int>(item->position.x), 
            static_cast<int>(item->position.y), 
            item->width, 
            item->height
        };
        SDL_RenderCopy(game.getRenderer(), item->texture, NULL, &dstRect);
    }
}

void SceneMain::renderUI()
{
    // 渲染血条

    // 直接设定好大小去绘制，而不是根据原本的大小去绘制
    int x = 10, y = 10;
    int offset = 40;
    int size = 32;
    // 绘制血量上限
    SDL_SetTextureColorMod(uiHealth, 100, 100, 100);    //想让颜色变淡，就都对原来的数字乘以一个小于1的数
    for(int i = 0; i < player.maxHealth; i++){    
        SDL_Rect dstRect = {x + i * offset, y, size, size};
        SDL_RenderCopy(game.getRenderer(), uiHealth, NULL, &dstRect);
    }
    // 绘制当前血量，先把颜色还原
    SDL_SetTextureColorMod(uiHealth, 255, 255, 255);
    for(int i = 0; i < player.currentHealth; i++){
        SDL_Rect dstRect = {x + i * offset, y, size, size};
        SDL_RenderCopy(game.getRenderer(), uiHealth, NULL, &dstRect);
    }

    // 渲染护盾
    x = 10, y = 50;
    SDL_SetTextureColorMod(uiShield, 100, 100, 100);
    for(int i = 0; i < player.maxShield; i++){
        SDL_Rect dstRect = {x + i * offset, y, size, size};
        SDL_RenderCopy(game.getRenderer(), uiShield, NULL, &dstRect);
    }
    SDL_SetTextureColorMod(uiShield, 255, 255, 255);
    for(int i = 0; i < player.currentShield; i++){
        SDL_Rect dstRect = {x + i * offset, y, size, size};
        SDL_RenderCopy(game.getRenderer(), uiShield, NULL, &dstRect);
    }


    // 渲染得分

    auto text = "Score: " + std::to_string(score);
    SDL_Color color = {255, 255, 255, 255};   // 字体颜色为白色
    SDL_Surface* surface = TTF_RenderText_Solid(scorefont, text.c_str(), color);    // 渲染文本
    SDL_Texture* texture = SDL_CreateTextureFromSurface(game.getRenderer(), surface); // 创建纹理
    SDL_Rect dstRect = {game.getWindowWidth() - 10 - surface->w, 10, surface->w, surface->h}; // 设置目标矩形
    SDL_RenderCopy(game.getRenderer(), texture, NULL, &dstRect);


    // 直接绘制好后删除
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void SceneMain::shootEnemy(Enemy* enemy)
{
    // 和玩家发射子弹是一样的
    auto projectile = new projectileEnemy(enemyProjectileTemplate);
    projectile->position.x = enemy->position.x + enemy->width / 2 - projectile->width / 2;
    projectile->position.y = enemy->position.y + enemy->height;
    // 敌机瞄准玩家发射子弹
    projectile->direction = getDirection(enemy);
    projectilesEnemies.push_back(projectile);

    // 播放音效
    Mix_PlayChannel(-1, sounds["enemy_shoot"], 0);
}

void SceneMain::updateEnemyProjectiles(float deltaTime)
{
    // 设定边界
    auto margin = 32;
    // 和子弹的更新是一样的
    for(auto it = projectilesEnemies.begin(); it != projectilesEnemies.end();){
        auto projectile = *it;
        // 子弹移动
        projectile->position.y += deltaTime * projectile->speed * projectile->direction.y;
        projectile->position.x += deltaTime * projectile->speed * projectile->direction.x;
        // 边界检测
        if(projectile->position.y > game.getWindowHeight() + margin ||
           projectile->position.y < -margin ||
           projectile->position.x > game.getWindowWidth() + margin ||
           projectile->position.x < -margin){
            delete projectile;
            it = projectilesEnemies.erase(it);
        }else{
            // 检测是否击中玩家，与敌机检测不同，由于只有一个玩家，所以不需要遍历
            // 因为不需要遍历，所以逻辑更加简单，也不需要bool值控制
            SDL_Rect projectileRect = {
                static_cast<int>(projectile->position.x), 
                static_cast<int>(projectile->position.y), 
                projectile->width, 
                projectile->height
            };
            SDL_Rect playerRect = {
                static_cast<int>(player.position.x), 
                static_cast<int>(player.position.y), 
                player.width,
                player.height
            };
            // 只有玩家活着才进行碰撞检测
            if(SDL_HasIntersection(&projectileRect, &playerRect ) && !isDead){

                if(player.currentShield > 0){
                    player.currentShield -= projectile->damage;
                }else{
                    player.currentHealth -= projectile->damage;
                }
                // 播放音效
                Mix_PlayChannel(-1, sounds["hit"], 0);
                delete projectile;
                it = projectilesEnemies.erase(it);    // 击中玩家，就删除子弹
            }else{
                it++;   // 没有击中玩家，就继续迭代
            }
        }
    }
}

void SceneMain::renderEnemyProjectiles()
{
    for(auto projectile : projectilesEnemies){
        SDL_Rect projectileRect = {
            static_cast<int>(projectile->position.x), 
            static_cast<int>(projectile->position.y), 
            projectile->width, 
            projectile->height
        };
        // 让子弹朝着玩家方向
        float angle = atan2(projectile->direction.y, projectile->direction.x) * 180 / M_PI - 90; // 弧度转角度，以传统坐标的x轴逆时针为正，旋转角度为正
        SDL_RenderCopyEx(game.getRenderer(), projectile->texture, NULL, &projectileRect, angle, NULL, SDL_FLIP_NONE);   // 以顺时针为正
    }
}

SDL_FPoint SceneMain::getDirection(Enemy *enemy)
{
    auto x = player.position.x + player.width / 2 - enemy->position.x - enemy->width / 2;
    auto y = player.position.y + player.height / 2 - enemy->position.y - enemy->height / 2;
    // 归一化
    auto length = sqrt(x * x + y * y);
    SDL_FPoint direction = {x / length, y / length};
    return direction;
}
