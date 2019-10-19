#include "Camera.hpp"

sf::View Camera::view;

sf::Vector2f Camera::cameraPosition;

sf::Vector2f Camera::screenDimensions;
sf::Vector2u Camera::mapDimensions;

// Ustawienie rozmiarow kamery
void Camera::SetDimensions(sf::Vector2f _screenDimensions, sf::Vector2u _mapDimensions)
{
    view.reset(sf::FloatRect(0, 0, screenDimensions.x, screenDimensions.y));
    view.setViewport(sf::FloatRect(0, 0, 1.0f, 1.0f));

    screenDimensions = _screenDimensions;
    mapDimensions = _mapDimensions;
}

// Proszuszanie kamera
void Camera::Move(sf::Vector2f playerPosition)
{
    cameraPosition = sf::Vector2f(
        playerPosition.x + 16 - (screenDimensions.x / 2), playerPosition.y + 16 - (screenDimensions.y / 2));

    if (mapDimensions.x > screenDimensions.x) {
        if (cameraPosition.x < 0) // JESLI KAMERA MIALA BY POKAZAC POZA GRANICA MAPY TO USTAWIAMY KAMERE TAK ZEBY GRACZ
                                  // NIE BYL W SRODKU - KAMERA BEDZIE POKAZYWAC OKRESLONY ROG MAPY
            cameraPosition.x = 0;
        else if (cameraPosition.x > mapDimensions.x - screenDimensions.x)
            cameraPosition.x = mapDimensions.x - screenDimensions.x;
    } else {
        cameraPosition.x = -(screenDimensions.x - mapDimensions.x) / 2;
    }
    if (mapDimensions.y > screenDimensions.y) {
        if (cameraPosition.y < 0)
            cameraPosition.y = 0;
        else if (cameraPosition.y > mapDimensions.y - screenDimensions.y)
            cameraPosition.y = mapDimensions.y - screenDimensions.y;
    } else {
        cameraPosition.y = -(screenDimensions.y - mapDimensions.y) / 2;
    }

    // Map tearing fix
    cameraPosition.x = floorf(cameraPosition.x), cameraPosition.y = floorf(cameraPosition.y);

    view.reset(sf::FloatRect(cameraPosition, screenDimensions));
}