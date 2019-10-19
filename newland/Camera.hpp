#ifndef _CAMERA_H
#define _CAMERA_H

#include <SFML/Graphics.hpp>

class Camera {
public:
    static sf::View view;

    static sf::Vector2f cameraPosition;

    static sf::Vector2f screenDimensions;
    static sf::Vector2u mapDimensions;

    static void SetDimensions(sf::Vector2f screenDimensions, sf::Vector2u mapDimensions);
    static void Move(sf::Vector2f playerPosition);
};

#endif