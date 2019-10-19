#ifndef _PICKUP_H
#define _PICKUP_H

#include "SFML/Graphics.hpp"

class Pickup {
private:
    sf::Texture itemTexture;
    std::string itemName;

public:
    sf::Sprite itemSprite;

    Pickup(std::string item, sf::Vector2f position);
    bool intersects(sf::FloatRect playerFoots);
    void PickupItem();
    void Draw(sf::RenderWindow& rt);
};

#endif