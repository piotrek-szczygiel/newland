#include "Pickup.hpp"
#include "Inventory.hpp"

Pickup::Pickup(std::string item, sf::Vector2f position)
{
    itemTexture.loadFromFile("resources/items/" + item + ".png");
    itemSprite.setTexture(itemTexture);

    itemSprite.setPosition(position);

    itemName = item;
}

bool Pickup::intersects(sf::FloatRect playerFoots)
{
    return (playerFoots.intersects(itemSprite.getGlobalBounds()));
}

void Pickup::PickupItem()
{
    Inventory::AddItem(itemName);
    delete this;
}

void Pickup::Draw(sf::RenderWindow& rt)
{
    rt.draw(itemSprite);
}
