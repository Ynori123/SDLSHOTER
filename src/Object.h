#ifndef OBJECT_H
#define OBJECT_H

#include <SDL.h>

// 利用枚举区别种类
enum class ItemType {
    Life,
    Shield,
    Time
};

struct Player
{
    SDL_Texture* texture = nullptr; // 玩家纹理
    SDL_Texture* shieldTexture = nullptr; // 玩家护盾纹理
    int shieldOffsetY = -20; // 玩家护盾位置偏移量

    // 注意这个是SDL_FPoint类型，position的两个成员叫做x和y
    // 后面设计子弹飞行的时候需要进行坐标计算，用浮点型更加精确
    SDL_FPoint position = {0.0f, 0.0f}; // 玩家位置
    int width = 50; // 玩家宽度
    int height = 50; // 玩家高度
    int speed = 300; // 玩家速度
    int currentHealth = 5; // 玩家当前生命值
    int maxHealth = 5; // 玩家最大生命值
    int currentShield = 1; // 玩家当前护盾值
    int maxShield = 3; // 玩家最大护盾值

    // 射击子弹CD
    Uint32 coolDown = 200; // 毫秒
    Uint32 lastShootTime = 0; // 上次射击时间戳，单位为毫秒

};

// projectile 发射体
struct projectilePlayer
{
    SDL_Texture* texture = nullptr; // 子弹纹理
    SDL_FPoint position = {0.0f, 0.0f}; // 子弹位置
    int width = 10; // 子弹宽度
    int height = 10; // 子弹高度
    int speed = 600; // 子弹速度
    int damage = 1; // 子弹伤害

};

// 敌机
struct Enemy
{
    SDL_Texture* texture = nullptr; // 敌机纹理
    SDL_FPoint position = {0.0f, 0.0f}; // 敌机位置
    int width = 50; // 敌机宽度
    int height = 50; // 敌机高度
    int speed = 100; // 敌机速度
    int currentHealth = 2; // 敌机当前生命值
    // 射击子弹CD
    Uint32 coolDown = 3000; // 1.5秒
    Uint32 lastShootTime = 0; // 上次射击时间戳，单位为毫秒
};

// 敌机子弹结构体
struct projectileEnemy{
    SDL_Texture* texture = nullptr; // 子弹纹理
    SDL_FPoint position = {0.0f, 0.0f}; // 子弹位置
    SDL_FPoint direction = {0.0f, 0.0f}; // 子弹方向
    int width = 10; // 子弹宽度
    int height = 10; // 子弹高度
    int speed = 280; // 子弹速度
    int damage = 1; // 子弹伤害
};

// 爆炸效果
struct Explosion{
    SDL_Texture* texture = nullptr; // 爆炸纹理
    SDL_FPoint position = {0.0f, 0.0f}; // 爆炸位置
    int width; // 爆炸宽度
    int height; // 爆炸高度
    int currentFrame = 0; // 当前爆炸帧
    int totalFrames; // 总爆炸帧数
    Uint32 startTime = 0; // 爆炸开始时间戳，单位为毫秒
    Uint32 FPS = 10; // 帧率，单位为毫秒
};

// 物品
struct Item{
    SDL_Texture* texture = nullptr; // 物品纹理
    SDL_FPoint position = {0.0f, 0.0f}; // 物品位置
    SDL_FPoint direction = {0.0f, 0.0f}; // 物品方向
    int speed = 200; // 物品速度
    int width = 50; // 物品宽度
    int height = 50; // 物品高度
    int bounceCount = 3; // 物品反弹次数
    ItemType type = ItemType::Life; // 物品类型，默认为生命值;
};

// 背景
struct Background
{
    SDL_Texture* texture = nullptr; // 背景纹理
    SDL_FPoint position = {0.0f, 0.0f}; // 背景位置
    int width = 800; // 背景宽度
    int height = 600; // 背景高度
    int speed = 10; // 背景速度
    float offset = 0.0f; // 背景偏移量（y轴）
};
#endif // OBJECT_H