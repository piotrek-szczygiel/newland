#include "Maps.hpp"

std::vector<LocationInfo> Maps::locationArray;
Location* Maps::currentLocation = NULL;

// Dodanie lokacji do kontenera map
Location* Maps::AddLocation(std::string locationName)
{
    Location* location = new Location();

    LocationInfo LocationInfo;
    LocationInfo.location = location;
    LocationInfo.locationName = locationName;
    locationArray.push_back(LocationInfo);

    return location;
}

// Wczytanie lokacji do pamieci (zaladowanie wszystkich potrzebnych plikow)
void Maps::CacheLocation(std::string locationName)
{
    for (int i = 0; i < locationArray.size(); i++) {
        if (locationArray[i].locationName == locationName) {
            locationArray[i].location->Load(locationArray[i].locationName);
            break;
        }
    }
}

// Dodanie i zaladowanie lokacji w jednej funkcji
Location* Maps::AddCacheLocation(std::string locationName)
{
    Location* location = AddLocation(locationName);
    CacheLocation(locationName);

    return location;
}

// Ustawienie podanej lokacji
Location* Maps::SetCurrentLocation(std::string locationName)
{
    for (int i = 0; i < locationArray.size(); i++) {
        if (locationArray[i].locationName == locationName) {
            currentLocation = locationArray[i].location;
            return currentLocation;
        }
    }

    return NULL;
}

Location* Maps::GetLocation(std::string locationName)
{
    for (int i = 0; i < locationArray.size(); i++) {
        if (locationArray[i].locationName == locationName) {
            return locationArray[i].location;
        }
    }

    return NULL;
}

void Maps::DeleteLocations()
{
    for (int i = 0; i < locationArray.size(); i++) {
        delete locationArray[i].location;
    }
}

// Sprawdzenie kolizji z krawedziami mapy
bool Maps::MapCollision(sf::FloatRect playerCollision)
{
    if ((playerCollision.left < 0) || (playerCollision.top < 0)
        || (playerCollision.left + playerCollision.width > currentLocation->GetSize().x)
        || (playerCollision.top + playerCollision.height > currentLocation->GetSize().y))
        return true;
    return false;
}
