#ifndef SCENEMAIN_H
#define SCENEMAIN_H

#include "Scene.h"
#include "Object.h"
#include <list>
#include <random>
#include <map>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

// 不要引用Game类，否则会循环引用
// 在cpp文件中引用Game类
class Game;

class SceneMain : public Scene
{
public:
    // SceneMain(); // 因为不需要
    ~SceneMain();

    // 公有函数
    void init() override;
    void update(float deltaTime) override;
    void render() override;
    void clean() override;
    void handleEvent(SDL_Event* event) override;



private:

    // 私有函数

    // 渲染
    void renderPlayerProjectiles(); // 渲染玩家子弹
    void renderEnemyProjectiles(); // 渲染敌人子弹
    void renderEnemies(); // 渲染敌人
    void renderExplosions(); // 渲染爆炸效果
    void renderItems(); // 渲染物品
    void renderUI(); // 渲染UI

    // 更新
    void updatePlayerProjectiles(float deltaTime); // 更新玩家子弹
    void updatePlayer(float deltaTime); // 更新玩家状态
    void updateEnemise(float deltaTime); // 更新敌人
    void updateEnemyProjectiles(float deltaTime); // 更新敌人子弹
    void updateItems(float deltaTime); // 更新物品
    void updateExplosions(float deltaTime); // 更新爆炸效果

    // 其他
    void keyboardControl(float deltaTime);    // 玩家方向控制
    void shootPlayer();    // 子弹发射
    void spawnEnemy(); // 生成敌人
    void shootEnemy(Enemy* enemy); // 敌人发射子弹，需要知道是谁发射的
    SDL_FPoint getDirection(Enemy* enemy); // 获取敌人子弹朝向
    void enemyExplode(Enemy* enemy); // 敌人爆炸（死亡）
    void dropItem(Enemy* enemy); // 敌人掉落物品
    void PlayerGetItem(Item* item); // 玩家获取物品
    void ChangeSceneDelay(float deltaTime, float delay); // 延时切换场景



    //已经在Scene.cpp中初始化列表了，因为每一个场景都需要用到game对象
    // 只是声明了有game成员变量，没有定义，所以cpp的构造函数中要初始化它
    // 初始化列表的方式
    // Game &game;  // 不需要再初始化了
    Player player; // 玩家对象
    bool isDead = false; // 玩家是否死亡
    Mix_Music* bgm = NULL; // 背景音乐
    SDL_Texture* uiHealth = NULL; // UI血条
    SDL_Texture* uiShield = NULL; // UI护盾
    TTF_Font* scorefont = NULL; // 分数字体

    int score = 0; // 分数
    float timerEnd = 0; // 用来延时切换结束场景的计时器
    

    //  创建每个物体的模板
    projectilePlayer projectilePlayerTemplate; // 子弹模板
    Enemy enemyTemplate; // 敌人模板
    projectileEnemy enemyProjectileTemplate; // 敌人子弹模板
    Explosion explosionTemplate; // 爆炸模板
    Item itemLifeTemplate; // 生命道具道具模板
    Item itemShieldTemplate; // 护盾道具模板

    // 创建每个物体的容器
    std::list<projectilePlayer*> projectilesPlayer ; // 子弹列表，使用指针是因为子弹可能很多，避免占用太多内存
    std::list<Enemy*> enemies; // 敌人列表
    std::list<projectileEnemy*> projectilesEnemies; // 敌人子弹列表
    std::list<Explosion*> explosions; // 爆炸列表
    std::list<Item*> items; // 道具列表
    std::map<std::string, Mix_Chunk*> sounds; // 声音（音效）列表

    // 随机数生成器
    std::mt19937 gen;   // 引擎
    std::uniform_real_distribution<float> dist; // 分布
};



#endif // SCENEMAIN_H