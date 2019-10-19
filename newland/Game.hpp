#ifndef _GAME_H
#define _GAME_H

#include "Camera.hpp"
#include "GameEngine.hpp"
#include "Gameplay.hpp"
#include "Maps.hpp"
#include "Player.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <sstream>
#include <tmx/MapLoader.hpp>

class Game {
public:
    static int playerDirection;
    static Player player;

    static sf::Time frameTime;

    static float dayTimer;
    static std::stringstream ssTime;

    static sf::Music bgMusic;

    static bool music;
    static int daynightType;
    static bool changeMap;
    static bool running;

    static void Initialize(sf::RenderWindow* win, std::string name);

    static void Update(sf::Time frameTime);
    static void Draw(sf::RenderWindow& rt, sf::RenderStates state);
    static void Close();
    static void LoadMap(std::string map, std::string portal = "main");
    static void PauseNpcs(bool pause);

    static void ToggleMusic();
};

#endif
