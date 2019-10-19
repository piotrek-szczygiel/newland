#include "Player.hpp"
#include "Camera.hpp"
#include "Controls.hpp"
#include "Inventory.hpp"
#include "Maps.hpp"
#include "Sound.hpp"

// Konstruktor obiektu gracza
Player::Player()
{
    currentWalkingAnimation = &walkingAnimationDown;
    animatedSprite = new AnimatedSprite(sf::seconds(0.15f), true, false);
    playerSpeed = 70.f;

    playerFont.loadFromFile("resources/tahoma.ttf");

    playerName.setFont(playerFont);
    playerName.setCharacterSize(16);
    playerName.setColor(sf::Color::Yellow);

    playerNameBg.setFillColor(sf::Color(64, 64, 64, 192));

    playerHp.setFont(playerFont);
    playerHp.setCharacterSize(14);
    playerHp.setColor(sf::Color::Magenta);

    playerHpBg.setFillColor(sf::Color(64, 64, 64, 128));

    if (Controls::device == Controls::DeviceJoystick)
        interactionIcon.loadFromFile("resources/ui/trojkat.png");
    else
        interactionIcon.loadFromFile("resources/ui/e.png");

    interactionIconSprite.setTexture(interactionIcon);
    interactionIconSprite.setScale(0.5, 0.5);

    patykTexture.loadFromFile("resources/eq/patyk.png");
    patyk.setTexture(patykTexture);
    patyk.setOrigin(patyk.getGlobalBounds().width / 2, patyk.getGlobalBounds().height);

    mieczTexture.loadFromFile("resources/eq/miecz.png");
    miecz.setTexture(mieczTexture);
    miecz.setOrigin(miecz.getGlobalBounds().width / 2, miecz.getGlobalBounds().height);

    startSwordSwing = sf::seconds(0.f);
    swordSwing = false;

    weaponType = None;

    maxHp = 200;
    hp = 200;
    dead = false;
    damage = 1;
    mainPlayer = false;
}

Player::~Player()
{
    delete animatedSprite;
}

void Player::SetFrameTime(sf::Time time)
{
    animatedSprite->setFrameTime(time);
}

// Wczytanie tekstur
bool Player::LoadTexture(std::string playerTextureFile)
{
    if (!playerTexture.loadFromFile(playerTextureFile))
        return false;

    // Dodawania pojedynczych klatek ruchu gracza na podstawie wzoru tekstury glownej
    walkingAnimationDown.setSpriteSheet(playerTexture);

    walkingAnimationDown.addFrame(sf::IntRect(0, 0, 32, 48));
    walkingAnimationDown.addFrame(sf::IntRect(32, 0, 32, 48));
    walkingAnimationDown.addFrame(sf::IntRect(64, 0, 32, 48));
    walkingAnimationDown.addFrame(sf::IntRect(96, 0, 32, 48));

    walkingAnimationLeft.setSpriteSheet(playerTexture);
    walkingAnimationLeft.addFrame(sf::IntRect(0, 48, 32, 48));
    walkingAnimationLeft.addFrame(sf::IntRect(32, 48, 32, 48));
    walkingAnimationLeft.addFrame(sf::IntRect(64, 48, 32, 48));
    walkingAnimationLeft.addFrame(sf::IntRect(96, 48, 32, 48));

    walkingAnimationRight.setSpriteSheet(playerTexture);
    walkingAnimationRight.addFrame(sf::IntRect(0, 96, 32, 48));
    walkingAnimationRight.addFrame(sf::IntRect(32, 96, 32, 48));
    walkingAnimationRight.addFrame(sf::IntRect(64, 96, 32, 48));
    walkingAnimationRight.addFrame(sf::IntRect(96, 96, 32, 48));

    walkingAnimationUp.setSpriteSheet(playerTexture);
    walkingAnimationUp.addFrame(sf::IntRect(0, 144, 32, 48));
    walkingAnimationUp.addFrame(sf::IntRect(32, 144, 32, 48));
    walkingAnimationUp.addFrame(sf::IntRect(64, 144, 32, 48));
    walkingAnimationUp.addFrame(sf::IntRect(96, 144, 32, 48));

    return true;
}

// Ustawienie animacji gracza (np animacja chodzenia w dol, gory, prawo, lewo)
void Player::SetCurrentAnimation(Animation* animation)
{
    if (!swordSwing)
        currentWalkingAnimation = animation;
}

void Player::SetName(std::wstring name, sf::Color color)
{
    playerName.setString(name);
    playerName.setColor(color);
    nickColor = color;
    _playerName = name;

    playerNameBg.setSize(sf::Vector2f(playerName.getGlobalBounds().width + 4, playerName.getGlobalBounds().height + 4));
}

Animation* Player::GetAnimation(int animation)
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

void Player::StopAnimation()
{
    animatedSprite->stop();
}

void Player::Move(sf::Vector2f movement)
{
    animatedSprite->play(*currentWalkingAnimation);
    animatedSprite->move(movement);
}

void Player::SetPosition(sf::Vector2f position)
{
    animatedSprite->setPosition(sf::Vector2f(position.x, position.y - 44));
}

sf::Vector2f Player::GetPosition()
{
    return animatedSprite->getPosition();
}

sf::FloatRect Player::GetAttackRange()
{
    return sf::FloatRect(GetPosition().x - 8, GetPosition().y, 48, 48);
}

sf::FloatRect Player::GetCollisionPoints()
{
    if (playerName.getString() != "#")
        return sf::FloatRect(GetPosition().x + 8, GetPosition().y + 32, 16, 16);
    else
        return sf::FloatRect(0, 0, 0, 0);
}

sf::FloatRect Player::GetPathCollisionPoints()
{
    return sf::FloatRect(GetPosition().x + 14, GetPosition().y + 32, 4, 4);
}

float Player::GetSpeed()
{
    return playerSpeed;
}

void Player::SetSpeed(float speed)
{
    playerSpeed = speed;
}

void Player::Update(sf::Time time)
{
    drawWeaponFront = false;

    switch (weaponType) {
    case Patyk:
        weapon = patyk;
        damage = 5;
        break;
    case Miecz:
        weapon = miecz;
        damage = 15;
        break;
    }
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
        }
        weaponHitbox = weapon.getGlobalBounds();
    } else {
        weaponHitbox = sf::FloatRect(0, 0, 0, 0);
    }
    animatedSprite->update(time);
    Maps::currentLocation->AttackEnemy(GetWeaponHitbox());
}

void Player::EnemyAttack(sf::FloatRect weaponHitbox, int takenDamage)
{
    if ((lastGotHit + sf::seconds(0.35f)) < timer.getElapsedTime())
        if (animatedSprite->getGlobalBounds().intersects(weaponHitbox)) {
            std::cout << "Player got hit!\n";
            lastGotHit = timer.getElapsedTime();
            if (rand() % 2)
                Sound::playerHurt1.play();
            else
                Sound::playerHurt2.play();

            hp -= takenDamage;
            if (hp <= 0) {
                dead = true;
            }
        }
}

sf::FloatRect Player::GetWeaponHitbox()
{
    return weaponHitbox;
}

void Player::DrawPlayer(sf::RenderWindow& rt, sf::RenderStates states)
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

void Player::SwingSword()
{
    if ((startSwordSwing + sf::seconds(0.2f)) < timer.getElapsedTime()
        && (lastSwordSwing + sf::seconds(0.35f)) < timer.getElapsedTime()) {
        bool weapon_eq = false;
        weaponType = None;
        for (auto item = Inventory::GetInventory()->begin(); item != Inventory::GetInventory()->end(); ++item) {
            if (item->equipped == true) {
                if (item->itemName == "patyk")
                    weaponType = Patyk, weapon_eq = true;
                else if (item->itemName == "miecz")
                    weaponType = Miecz, weapon_eq = true;
            }
        }

        if (!weapon_eq)
            return;

        startSwordSwing = timer.getElapsedTime();
        lastSwordSwing = startSwordSwing;
        swordSwing = true;

        if (weaponType == Patyk || weaponType == Miecz)
            Sound::patykSlash.play();
    }
}

void Player::DrawNick(sf::RenderWindow& rt, sf::RenderStates states, bool interaction)
{
    if (!mainPlayer) {
        if (playerName.getString() == L"#")
            return;

        if (interaction) {
            playerName.setColor(sf::Color::Green), playerName.setStyle(sf::Text::Underlined | sf::Text::Italic);
        } else {
            if (talkable)
                playerName.setColor(sf::Color::Yellow), playerName.setStyle(sf::Text::Italic);
            else
                playerName.setColor(nickColor), playerName.setStyle(sf::Text::Regular);
        }

        sf::Vector2f centerPosition(animatedSprite->getPosition().x + 16, animatedSprite->getPosition().y);
        playerName.setPosition(centerPosition.x - (playerName.getGlobalBounds().width / 2),
            centerPosition.y - (playerName.getGlobalBounds().height + 7));
        playerNameBg.setPosition(centerPosition.x - (playerNameBg.getGlobalBounds().width / 2) + 1,
            centerPosition.y - (playerNameBg.getGlobalBounds().height + 1));
        interactionIconSprite.setPosition(playerNameBg.getPosition().x - 20, playerNameBg.getPosition().y);

        if (interaction)
            rt.draw(interactionIconSprite);
        rt.draw(playerNameBg, states);
        rt.draw(playerName, states);
    } else {
        playerHp.setString("HP: " + std::to_string(hp) + "/" + std::to_string(maxHp));
        playerHpBg.setSize(sf::Vector2f(playerHp.getGlobalBounds().width + 7, playerHp.getGlobalBounds().height + 4));

        // sf::Vector2f centerPosition(animatedSprite -> getPosition().x+16,  animatedSprite -> getPosition().y);
        // playerName.setString(_playerName + L"  " + std::to_wstring(hp) + L" / " + std::to_wstring(maxHp));
        // playerNameBg.setSize(sf::Vector2f(playerName.getGlobalBounds().width+4,
        // playerName.getGlobalBounds().height+4)); playerName.setPosition(centerPosition.x -
        // ((playerName.getGlobalBounds().width/2)+(playerHp.getGlobalBounds().width/2)),
        // centerPosition.y-(playerName.getGlobalBounds().height+7)); playerNameBg.setPosition(centerPosition.x -
        // ((playerNameBg.getGlobalBounds().width/2)+(playerHpBg.getGlobalBounds().width/2))+1,
        // centerPosition.y-(playerNameBg.getGlobalBounds().height+1));

        playerHp.setPosition(Camera::cameraPosition.x + 1, Camera::cameraPosition.y + 1);
        playerHpBg.setPosition(Camera::cameraPosition);

        rt.draw(playerHpBg, states);
        rt.draw(playerHp, states);
        // rt.draw(playerNameBg, states);
        // rt.draw(playerName, states);
    }
}

sf::Texture* Player::GetTexture()
{
    return &playerTexture;
}
