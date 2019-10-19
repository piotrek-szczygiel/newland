#ifndef _ENEMY_H
#define _ENEMY_H

#include "AnimatedSprite.hpp"
#include <SFML/Graphics.hpp>

class Location;

class Enemy {
private:
    sf::Texture enemyTexture;

    Animation walkingAnimationDown;
    Animation walkingAnimationUp;
    Animation walkingAnimationLeft;
    Animation walkingAnimationRight;

    Animation* currentWalkingAnimation;

    AnimatedSprite* animatedSprite;

    float enemySpeed;

    int maxHp;
    int hp;
    int damage;

    sf::Font enemyFont;
    sf::Text enemyName;
    sf::Text enemyHp;
    sf::RectangleShape enemyNameBg;
    sf::RectangleShape enemyHpBg;

    sf::Texture patykTexture;
    sf::Sprite patyk;

    sf::Time changeTime;
    sf::Time lastAttackRange;
    bool changeDirection;
    int direction;

    sf::Time startSwordSwing;

    bool swordSwing;
    bool drawWeaponFront;
    sf::Sprite weapon;
    sf::FloatRect weaponHitbox;

    std::vector<sf::FloatRect> paths;

    int enemyState;
    enum States { Walking, ChasingPlayer };

public:
    bool dead;
    sf::Time lastSwordSwing;
    sf::Time deadTime;
    sf::Clock timer;

    enum Animations { Up, Down, Left, Right };
    enum Weapons { None, Patyk, Miecz };

    int weaponType;

    Enemy();
    ~Enemy();

    bool Load(std::string file);
    bool LoadTexture(std::string enemyTextureFile);

    float GetSpeed();
    void SetSpeed(float speed);

    void SetCurrentAnimation(Animation* animation);
    void StopAnimation();
    Animation* GetAnimation(int animation);

    void Move(sf::Vector2f movement);
    void SetPosition(sf::Vector2f position);
    void SetName(std::wstring name);

    void SetPaths(sf::FloatRect aabb);
    bool CheckPath(sf::FloatRect collision);

    void Damage(int takenDamage);

    sf::Vector2f GetPosition();
    sf::FloatRect GetCollisionPoints();
    sf::FloatRect GetPathCollisionPoints();
    sf::FloatRect GetAttackRange();
    sf::FloatRect GetHitRange();
    sf::Texture* GetTexture();

    void SwingSword();

    void AiUpdate(
        float frameTime, Location* location, std::vector<sf::FloatRect>* playerCollisions, sf::Vector2f playerPosition);
    void Update(sf::Time time);
    void DrawEnemy(sf::RenderWindow& rt, sf::RenderStates states);
    void DrawNick(sf::RenderWindow& rt, sf::RenderStates states);

    void SetFrameTime(sf::Time time);
};

#endif