#ifndef _MAPS_H
#define _MAPS_H

#include "Location.hpp"
#include <vector>

struct LocationInfo {
    Location* location;
    std::string locationName;
};

class Maps {
public:
    static std::vector<LocationInfo> locationArray;
    static Location* currentLocation;

    static Location* AddLocation(std::string locationName);
    static void CacheLocation(std::string locationName);
    static Location* AddCacheLocation(std::string locationName);

    static Location* SetCurrentLocation(std::string locationName);
    static Location* GetLocation(std::string locationName);

    static void DeleteLocations();

    static bool MapCollision(sf::FloatRect playerCollision);
};

#endif