#ifndef _NPC_H
#define _NPC_H

#include "Chronometer.hpp"
#include "Player.hpp"

class Location;

class Npc {
private:
    std::wstring name;
    sf::Sprite avatar;
    sf::Texture avatarTex;
    sf::Vector2f originalPosition;
    sftools::Chronometer timer;
    std::vector<sf::FloatRect> npcPaths;

    sf::Time changeTime;
    bool changeDirection;
    int direction;

    bool avFile;
    bool CheckPath(sf::FloatRect);

public:
    bool staticNpc;
    bool free;
    bool interaction, interaction_message;
    Player player;

    bool Load(std::string);
    void SetPaths(sf::FloatRect aabb);

    sf::FloatRect GetInteractionRect();
    std::wstring GetName();
    sf::Sprite GetAvatar();

    void AiUpdate(float frameTime, Location* location, std::vector<sf::FloatRect>* playerCollisions);
    void Pause(bool pause);
    void Draw(sf::RenderWindow& rt, sf::RenderStates states);
};

#endif