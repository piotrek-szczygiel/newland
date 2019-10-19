#ifndef _PLAYER_H
#define _PLAYER_H

#include "AnimatedSprite.hpp"
#include <SFML/Graphics.hpp>

class Player {
private:
    sf::Texture playerTexture;
    sf::Texture interactionIcon;
    sf::Sprite interactionIconSprite;

    Animation walkingAnimationDown;
    Animation walkingAnimationUp;
    Animation walkingAnimationLeft;
    Animation walkingAnimationRight;

    Animation* currentWalkingAnimation;

    AnimatedSprite* animatedSprite;

    float playerSpeed;

    sf::Font playerFont;
    sf::Text playerName;
    sf::Text playerHp;
    sf::RectangleShape playerNameBg;
    sf::RectangleShape playerHpBg;

    sf::Texture patykTexture;
    sf::Sprite patyk;

    sf::Texture mieczTexture;
    sf::Sprite miecz;

    sf::Time startSwordSwing;

    bool swordSwing;
    sf::FloatRect weaponHitbox;
    bool drawWeaponFront;
    sf::Sprite weapon;

    std::wstring _playerName;

public:
    bool dead;
    bool talkable;
    bool mainPlayer;
    sf::Time lastSwordSwing;
    sf::Time lastGotHit;
    sf::Clock timer;
    sf::Color nickColor;

    int maxHp;
    int hp;

    int damage;

    enum Animations { Up, Down, Left, Right };
    enum Weapons { None, Patyk, Miecz };

    int weaponType;

    Player();
    ~Player();

    bool LoadTexture(std::string playerTextureFile);

    float GetSpeed();
    void SetSpeed(float speed);

    void SetCurrentAnimation(Animation* animation);
    void StopAnimation();
    Animation* GetAnimation(int animation);

    void Move(sf::Vector2f movement);
    void SetPosition(sf::Vector2f position);
    void SetName(std::wstring name, sf::Color color);

    void EnemyAttack(sf::FloatRect weaponHitbox, int takenDamage);

    sf::Vector2f GetPosition();
    sf::FloatRect GetCollisionPoints();
    sf::FloatRect GetPathCollisionPoints();
    sf::FloatRect GetAttackRange();
    sf::FloatRect GetWeaponHitbox();
    sf::Texture* GetTexture();

    void SwingSword();

    void Update(sf::Time time);
    void DrawPlayer(sf::RenderWindow& rt, sf::RenderStates states);
    void DrawNick(sf::RenderWindow& rt, sf::RenderStates states, bool interaction = false);

    void SetFrameTime(sf::Time time);
};

#endif
