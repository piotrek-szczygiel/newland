#include "Enemy.hpp"
#include "Game.hpp"
#include "Inventory.hpp"
#include "Location.hpp"
#include "Sound.hpp"
#include <cmath>
#include <iostream>
#include <pugixml\pugixml.hpp>
#include <sstream>

const double M_PI = 3.14159265358979323846;
// Konstruktor obiektu gracza
Enemy::Enemy()
{
    patykTexture.loadFromFile("resources/eq/kosc.png");
    patyk.setTexture(patykTexture);
    patyk.setOrigin(patyk.getGlobalBounds().width / 2, patyk.getGlobalBounds().height);

    direction = 0;
    changeTime = sf::seconds(0);
    changeDirection = true;

    currentWalkingAnimation = &walkingAnimationDown;
    animatedSprite = new AnimatedSprite(sf::seconds(0.15f), true, false);
    enemySpeed = 70.f;

    enemyFont.loadFromFile("resources/tahoma.ttf");

    enemyName.setFont(enemyFont);
    enemyName.setCharacterSize(16);
    enemyName.setColor(sf::Color::Yellow);

    enemyHp.setFont(enemyFont);
    enemyHp.setCharacterSize(14);
    enemyHp.setColor(sf::Color::Red);

    enemyNameBg.setFillColor(sf::Color(64, 64, 64, 192));
    enemyHpBg.setFillColor(sf::Color(64, 64, 64, 192));

    enemyName.setString(L"Przeciwnik");

    startSwordSwing = sf::seconds(0.f);
    swordSwing = false;

    weaponType = None;
    maxHp = 60;
    hp = 60;
    damage = 5;
    dead = false;
}

Enemy::~Enemy()
{
    delete animatedSprite;
}

bool Enemy::Load(std::string file)
{
    std::string enemyFile = "resources/" + file;
    pugi::xml_document doc;

    SetSpeed(50.f);

    // Zaladowanie pliku XML
    if (doc.load_file(enemyFile.c_str())) {
        pugi::xml_node panels = doc.child("enemy");
        for (pugi::xml_node panel = panels.first_child(); panel; panel = panel.next_sibling()) {
            std::string name = panel.name();
            std::string value = panel.child_value();

            // Plik textury enemy
            if (name == "sprite") {
                if (!LoadTexture("resources/sprites/" + value))
                    return false;
            }

            else if (name == "speed") {
                SetSpeed(std::stoi(value));
                // int longer = round(30.f / player.GetSpeed());
                // player.SetFrameTime(sf::seconds(0.15f * longer));
            }

            // Nazwa enemy
            else if (name == "name") {
                SetName(pugi::as_wide(value));
            }
        }
    }

    return true;
}

void Enemy::SetFrameTime(sf::Time time)
{
    animatedSprite->setFrameTime(time);
}

// Wczytanie tekstur
bool Enemy::LoadTexture(std::string enemyTextureFile)
{
    if (!enemyTexture.loadFromFile(enemyTextureFile))
        return false;

    // Dodawania pojedynczych klatek ruchu gracza na podstawie wzoru tekstury glownej
    walkingAnimationDown.setSpriteSheet(enemyTexture);

    walkingAnimationDown.addFrame(sf::IntRect(0, 0, 32, 48));
    walkingAnimationDown.addFrame(sf::IntRect(32, 0, 32, 48));
    walkingAnimationDown.addFrame(sf::IntRect(64, 0, 32, 48));
    walkingAnimationDown.addFrame(sf::IntRect(96, 0, 32, 48));

    walkingAnimationLeft.setSpriteSheet(enemyTexture);
    walkingAnimationLeft.addFrame(sf::IntRect(0, 48, 32, 48));
    walkingAnimationLeft.addFrame(sf::IntRect(32, 48, 32, 48));
    walkingAnimationLeft.addFrame(sf::IntRect(64, 48, 32, 48));
    walkingAnimationLeft.addFrame(sf::IntRect(96, 48, 32, 48));

    walkingAnimationRight.setSpriteSheet(enemyTexture);
    walkingAnimationRight.addFrame(sf::IntRect(0, 96, 32, 48));
    walkingAnimationRight.addFrame(sf::IntRect(32, 96, 32, 48));
    walkingAnimationRight.addFrame(sf::IntRect(64, 96, 32, 48));
    walkingAnimationRight.addFrame(sf::IntRect(96, 96, 32, 48));

    walkingAnimationUp.setSpriteSheet(enemyTexture);
    walkingAnimationUp.addFrame(sf::IntRect(0, 144, 32, 48));
    walkingAnimationUp.addFrame(sf::IntRect(32, 144, 32, 48));
    walkingAnimationUp.addFrame(sf::IntRect(64, 144, 32, 48));
    walkingAnimationUp.addFrame(sf::IntRect(96, 144, 32, 48));

    return true;
}

// Ustawienie animacji gracza (np animacja chodzenia w dol, gory, prawo, lewo)
void Enemy::SetCurrentAnimation(Animation* animation)
{
    if (!swordSwing)
        currentWalkingAnimation = animation;
}

// Ustawia pole po ktorym npc sie bedzie poruszal
void Enemy::SetPaths(sf::FloatRect aabb)
{
    paths.push_back(aabb);
}

bool Enemy::CheckPath(sf::FloatRect collision)
{
    for (int i = 0; i < paths.size(); i++) {
        if (collision.intersects(paths[i]))
            return true;
    }

    return false;
}

void Enemy::Damage(int takenDamage)
{
    hp -= takenDamage;
    if (hp <= 0) {
        dead = true;
        Sound::skeletonDead.play();
        Sound::skeletonDead.setPlayingOffset(sf::seconds(0.8f));
        deadTime = timer.getElapsedTime();
    }
}

void Enemy::SetName(std::wstring name)
{
    enemyName.setString(name);
    enemyName.setColor(sf::Color::White);
    enemyNameBg.setSize(sf::Vector2f(enemyName.getGlobalBounds().width + 4, enemyName.getGlobalBounds().height + 4));
}

Animation* Enemy::GetAnimation(int animation)
{
    switch (animation) {
    case Up:
        return &walkingAnimationUp;
        break;
    case Down:
        return &walkingAnimationDown;
        break;
    case Left:
        return &walkingAnimationLeft;
        break;
    case Right:
        return &walkingAnimationRight;
        break;
    default:
        return &walkingAnimationDown;
        break;
    }
}

void Enemy::StopAnimation()
{
    animatedSprite->stop();
}

void Enemy::Move(sf::Vector2f movement)
{
    animatedSprite->play(*currentWalkingAnimation);
    animatedSprite->move(movement);
}

void Enemy::SetPosition(sf::Vector2f position)
{
    animatedSprite->setPosition(sf::Vector2f(position.x, position.y - 44));
}

sf::Vector2f Enemy::GetPosition()
{
    return animatedSprite->getPosition();
}

sf::FloatRect Enemy::GetCollisionPoints()
{
    return sf::FloatRect(GetPosition().x + 8, GetPosition().y + 32, 16, 16);
}

sf::FloatRect Enemy::GetPathCollisionPoints()
{
    return sf::FloatRect(GetPosition().x + 14, GetPosition().y + 32, 4, 4);
}

float Enemy::GetSpeed()
{
    return enemySpeed;
}

void Enemy::SetSpeed(float speed)
{
    enemySpeed = speed;
}

void Enemy::Update(sf::Time time)
{

    drawWeaponFront = false;
    weapon = patyk;

    if (swordSwing) {
        if ((startSwordSwing + sf::seconds(0.15f)) < timer.getElapsedTime())
            swordSwing = false;
        else {
            float swordLvl = timer.getElapsedTime().asSeconds() - startSwordSwing.asSeconds();

            float rotationLvl = swordLvl / 0.15f;

            if (currentWalkingAnimation == &walkingAnimationLeft) {
                weapon.setPosition(GetPosition().x + 14, GetPosition().y + 32);
                weapon.setRotation(rotationLvl * -90.f);
            } else if (currentWalkingAnimation == &walkingAnimationRight) {
                weapon.setPosition(GetPosition().x + 14, GetPosition().y + 32);
                weapon.setRotation(rotationLvl * 90.f), drawWeaponFront = true;
            } else if (currentWalkingAnimation == &walkingAnimationDown) {
                weapon.setPosition(GetPosition().x + 6, GetPosition().y + 32);
                weapon.setRotation(90 + (rotationLvl * 90.f)), drawWeaponFront = true;
            } else if (currentWalkingAnimation == &walkingAnimationUp) {
                weapon.setPosition(GetPosition().x + 26, GetPosition().y + 24);
                weapon.setRotation(-90.f + (rotationLvl * 90.f));
            }
            weaponHitbox = weapon.getGlobalBounds();
        }
    } else {
        weaponHitbox = sf::FloatRect(0, 0, 0, 0);
    }

    animatedSprite->update(time);
    Game::player.EnemyAttack(weaponHitbox, damage);
}

sf::FloatRect Enemy::GetAttackRange()
{
    return sf::FloatRect(GetPosition().x + 4, GetPosition().y + 12, 24, 24);
}

sf::FloatRect Enemy::GetHitRange()
{
    return animatedSprite->getGlobalBounds();
}

void Enemy::AiUpdate(
    float frameTime, Location* location, std::vector<sf::FloatRect>* playerCollisions, sf::Vector2f playerPosition)
{
    if (dead) {
        if ((deadTime + sf::seconds(30.f)) < timer.getElapsedTime()) {
            dead = false;
            hp = maxHp;
        }
        return;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))
        SwingSword();

    sf::Vector2f playerFoots(playerPosition.x + 16, playerPosition.y + 40);
    sf::Vector2f enemyFoots(GetPosition().x + 16, GetPosition().y + 40);
    sf::FloatRect enemyAggroRange(enemyFoots.x - 150, enemyFoots.y - 150, 300, 300);
    sf::FloatRect playerAttackRange(playerPosition.x - 4, playerPosition.y + 4, 40, 40);

    if (enemyAggroRange.contains(playerFoots)
        && CheckPath(sf::FloatRect(playerPosition.x + 14, playerPosition.y + 32, 4, 4))) {
        enemyState = ChasingPlayer;
    } else
        enemyState = Walking;

    if (enemyState == Walking) {
        sf::Vector2f movement(0.f, 0.f);

        if (changeTime.asSeconds() <= timer.getElapsedTime().asSeconds()) {
            changeDirection = true;
        }
        if (changeDirection) {
            Sound::skeletonWalking.setPosition(GetPosition().x, 0, GetPosition().y);
            Sound::skeletonWalking.play();
            changeDirection = false;
            int longer = floorf((50.f / GetSpeed()) + 0.5f);
            changeTime = sf::seconds((rand() % (3 * longer)) + 0.25) + timer.getElapsedTime();
            // direction = (direction + (1+(rand()%3)))%4
            direction = rand() % 4;
        }

        SetCurrentAnimation(GetAnimation(direction));
        if (direction == 0) {
            movement.y -= GetSpeed();
        } else if (direction == 1) {
            movement.y += GetSpeed();
        } else if (direction == 2) {
            movement.x -= GetSpeed();
        } else if (direction == 3) {
            movement.x += GetSpeed();
        }

        Move(movement * frameTime);

        sf::FloatRect npcCollision = GetCollisionPoints();

        bool collision = false;

        if ((npcCollision.left < 0) || (npcCollision.top < 0)
            || (npcCollision.left + npcCollision.width > location->GetSize().x)
            || (npcCollision.top + npcCollision.height > location->GetSize().y)) {
            collision = true;
        } else if (!CheckPath(GetPathCollisionPoints()) || location->Collision(npcCollision)) {
            collision = true;
        } else {
            for (auto& playerObj = playerCollisions->begin(); playerObj != playerCollisions->end(); ++playerObj) {
                if (playerObj->intersects(npcCollision)) {
                    collision = true;
                    break;
                }
            }
        }

        if (collision) {
            changeDirection = true;
            Move(-movement * frameTime);
        }
    } else if (enemyState == ChasingPlayer) {
        sf::Vector2f movement(0.f, 0.f);

        float dx = enemyFoots.x - playerFoots.x;
        float dy = enemyFoots.y - playerFoots.y;

        float rotation = ((atan2(dy, dx)) * 180 / M_PI) + 180.f;

        if (rotation > 315.f || rotation <= 45.f) {
            SetCurrentAnimation(GetAnimation(Player::Right));
        } else if (rotation > 45.f && rotation <= 135.f) {
            SetCurrentAnimation(GetAnimation(Player::Down));
        } else if (rotation > 135.f && rotation <= 225.f) {
            SetCurrentAnimation(GetAnimation(Player::Left));
        } else if (rotation > 225.f && rotation <= 315.f) {
            SetCurrentAnimation(GetAnimation(Player::Up));
        }

        if (playerFoots.y < enemyFoots.y) {
            // SetCurrentAnimation(GetAnimation(0));
            movement.y -= GetSpeed();
        } else if (playerFoots.y > enemyFoots.y) {
            // SetCurrentAnimation(GetAnimation(1));
            movement.y += GetSpeed();
        }
        if (playerFoots.x < enemyFoots.x) {
            // SetCurrentAnimation(GetAnimation(2));
            movement.x -= GetSpeed();
        } else if (playerFoots.x > enemyFoots.x) {
            // SetCurrentAnimation(GetAnimation(3));
            movement.x += GetSpeed();
        }

        Move(movement * frameTime);

        sf::FloatRect npcCollision = GetCollisionPoints();

        bool collision = false;

        if ((npcCollision.left < 0) || (npcCollision.top < 0)
            || (npcCollision.left + npcCollision.width > location->GetSize().x)
            || (npcCollision.top + npcCollision.height > location->GetSize().y)) {
            collision = true;
        } else if (!CheckPath(GetPathCollisionPoints()) || location->Collision(npcCollision)) {
            collision = true;
        } else {
            for (auto& playerObj = playerCollisions->begin(); playerObj != playerCollisions->end(); ++playerObj) {
                if (playerObj->intersects(npcCollision)) {
                    collision = true;
                    break;
                }
            }
        }

        if (collision) {
            changeDirection = true;
            Move(-movement * frameTime);
            StopAnimation();
        }

        if (GetAttackRange().intersects(playerAttackRange)) {
            SwingSword();
        }
    }

    Update(sf::seconds(frameTime));
}

void Enemy::DrawEnemy(sf::RenderWindow& rt, sf::RenderStates states)
{
    if (drawWeaponFront) {
        rt.draw(*animatedSprite, states);
        if (swordSwing)
            rt.draw(weapon);
    } else {
        if (swordSwing)
            rt.draw(weapon);
        rt.draw(*animatedSprite, states);
    }
}

void Enemy::SwingSword()
{
    if ((startSwordSwing + sf::seconds(0.2f)) < timer.getElapsedTime()
        && (lastSwordSwing + sf::seconds(1.5f)) < timer.getElapsedTime()) {
        startSwordSwing = timer.getElapsedTime();
        lastSwordSwing = startSwordSwing;
        swordSwing = true;
    }
}

void Enemy::DrawNick(sf::RenderWindow& rt, sf::RenderStates states)
{
    if (dead)
        return;

    enemyHp.setString(std::to_string(hp) + " / " + std::to_string(maxHp));
    enemyHpBg.setSize(sf::Vector2f(enemyHp.getGlobalBounds().width + 4, enemyHp.getGlobalBounds().height + 4));

    sf::Vector2f centerPosition(animatedSprite->getPosition().x + 16, animatedSprite->getPosition().y);
    enemyName.setPosition(centerPosition.x - (enemyName.getGlobalBounds().width / 2),
        centerPosition.y - (enemyName.getGlobalBounds().height + 7));
    enemyNameBg.setPosition(centerPosition.x - (enemyNameBg.getGlobalBounds().width / 2) + 1,
        centerPosition.y - (enemyNameBg.getGlobalBounds().height + 1));

    enemyHp.setPosition(centerPosition.x - (enemyHp.getGlobalBounds().width / 2),
        centerPosition.y - (enemyHp.getGlobalBounds().height + 23));
    enemyHpBg.setPosition(centerPosition.x - (enemyHpBg.getGlobalBounds().width / 2) + 1,
        centerPosition.y - (enemyHpBg.getGlobalBounds().height + 17));

    rt.draw(enemyNameBg, states);
    rt.draw(enemyName, states);
    rt.draw(enemyHpBg, states);
    rt.draw(enemyHp, states);
}

sf::Texture* Enemy::GetTexture()
{
    return &enemyTexture;
}
