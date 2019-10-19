#ifndef _GAMEPLAYINFO_H
#define _GAMEPLAYINFO_H

#include <SFML/Graphics.hpp>
#include <vector>

struct TriggerInfo {
    std::string name;
    int count;
};

struct TriggersInfo {
    std::string objName;
    std::vector<TriggerInfo>* triggerInfo;
};

class GameplayInfo {
public:
    ~GameplayInfo();

    static std::vector<TriggersInfo> Triggers;

    static TriggerInfo GetInfo(std::string objName, std::string name);
    static void AddInfo(std::string objName, std::string name);
    static void SetInfo(std::string objName, std::string name, int count);
};

#endif