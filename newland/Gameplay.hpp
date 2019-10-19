#ifndef _GAMEPLAY_H
#define _GAMEPLAY_H

#include "GameplayInfo.hpp"
#include "Inventory.hpp"
#include "UserInterface.hpp"
#include <SFML/Graphics.hpp>
#include <tmx/MapLoader.hpp>

class Gameplay {
public:
    static std::wstring straznikTekst[6];
    static std::wstring lastObject;

    static void Interaction();

    Gameplay();

    // jezeli true to znaczy ze np jest wiadomosc czy chcemy sie przeniesc do domu i musimy kliknac tak albo nie zamiast
    // poruszyc postacia
    static void Trigger(tmx::MapObject* object);
    static void Trigger(std::wstring objName);

    // Triggers

    static void Galicja(std::wstring objName, std::string location, sf::Vector2f objCenterPos);
    static void Farma(std::wstring objName, std::string location, sf::Vector2f objCenterPos);
    static void Pustynia(std::wstring objName, std::string location, sf::Vector2f objCenterPos);
    static void Plaza(std::wstring objName, std::string location, sf::Vector2f objCenterPos);
};

#endif
