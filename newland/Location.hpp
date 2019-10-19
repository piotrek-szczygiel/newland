#ifndef _LOCATION_H
#define _LOCATION_H

#include <AnimatedSprite.hpp>
#include <SFML/Graphics.hpp>
#include <tmx/MapLoader.hpp>
//#include "Shader.hpp"
#include "Enemy.hpp"
#include "Npc.hpp"
#include "Pickup.hpp"

struct ObjectLayerInfo {
    std::string layerName;
    std::vector<tmx::MapObject>* objects;
};

struct AnimationInfo {
    Animation* animation;
    AnimatedSprite* animatedSprite;
    int level;
};

struct TextureInfo {
    sf::Texture* texture;
    std::string filepath;
};

struct DayNightColorsInfo {
    DayNightColorsInfo(sf::Color color, bool lights)
        : color(color)
        , lights(lights)
    {
    }
    sf::Color color;
    bool lights;
};

class Location {
private:
    sf::Clock timer;
    sf::Time lastEnemyHit;
    std::vector<DayNightColorsInfo> daynightColors;
    sf::Color noDaylightColor;

    int daynightType;

    sf::RenderStates blendStates, blendStates2;

    tmx::MapLoader* mapLoader;

    std::vector<ObjectLayerInfo> objectLayers;
    std::vector<Npc*>* npcs;
    std::vector<Enemy*>* enemies;
    std::vector<AnimationInfo> animations;
    std::vector<TextureInfo> textures;

    std::string locationName;
    std::string parentLocation;
    std::string bgMusic;

    std::vector<sf::Sprite> lights;

    sf::RenderTexture lightBase;
    sf::Sprite lightBaseSpr;

    sf::Texture lightTex;

    bool daylight;

public:
    Location();
    ~Location();

    std::vector<Pickup*> pickups;

    void Load(std::string fileName);
    void ConfigureLights(float runTime);
    bool Collision(sf::FloatRect playerCollisionRect);
    bool NpcCollision(sf::FloatRect playerCollisionRect);
    void SetLayerVisible(std::string layerName, bool visible);

    sf::Vector2u GetSize();
    sf::Vector2f GetSpawnPoint(std::string portal);
    std::string GetBgMusic();

    std::vector<tmx::MapObject>* GetTriggers();
    std::vector<Npc*>* GetNpcs();
    std::vector<tmx::MapObject>* GetObjects();

    void PauseNpcs(bool pause);

    void AttackEnemy(sf::FloatRect playerWeaponHitbox);

    std::string GetName();
    std::string GetParent();
    Npc* GetNpc(std::wstring name);

    void Draw(sf::RenderWindow& rt, sf::RenderStates states, Player* player, float runTime, float frameTime,
        int daynightType);
};

#endif
