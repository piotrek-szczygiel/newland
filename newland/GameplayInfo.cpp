#include "GameplayInfo.hpp"

std::vector<TriggersInfo> GameplayInfo::Triggers;

// Pobranie informacji o danym triggerze
TriggerInfo GameplayInfo::GetInfo(std::string objName, std::string name)
{
    for (int i = 0; i < Triggers.size(); i++) {
        if (Triggers[i].objName == objName) {
            for (int j = 0; j < Triggers[i].triggerInfo->size(); j++) {
                if (Triggers[i].triggerInfo->at(j).name == name)
                    return Triggers[i].triggerInfo->at(j);
            }
        }
    }

    TriggerInfo noInfo;
    noInfo.count = 0;
    noInfo.name = "";

    return noInfo;
}

void GameplayInfo::SetInfo(std::string objName, std::string name, int count)
{
    bool found = false;
    bool found2 = false;

    for (int i = 0; i < Triggers.size(); i++) {
        if (Triggers[i].objName == objName) {
            found = true;
            for (int j = 0; j < Triggers[i].triggerInfo->size(); j++) {
                if (Triggers[i].triggerInfo->at(j).name == name) {
                    found2 = true;
                    Triggers[i].triggerInfo->at(j).count = count;
                    break;
                }
            }

            if (!found2) {
                TriggerInfo triggerInfo;
                triggerInfo.count = count;
                triggerInfo.name = name;
                Triggers[i].triggerInfo->push_back(triggerInfo);
            }
            break;
        }
    }

    if (!found) {
        TriggersInfo triggersInfo;
        triggersInfo.objName = objName;
        triggersInfo.triggerInfo = new std::vector<TriggerInfo>;
        Triggers.push_back(triggersInfo);

        if (!found2) {
            for (int i = 0; i < Triggers.size(); i++) {
                if (Triggers[i].objName == objName) {
                    found = true;
                    for (int j = 0; j < Triggers[i].triggerInfo->size(); j++) {
                        if (Triggers[i].triggerInfo->at(j).name == name) {
                            found2 = true;
                            Triggers[i].triggerInfo->at(j).count = count;
                            break;
                        }
                    }

                    if (!found2) {
                        TriggerInfo triggerInfo;
                        triggerInfo.count = count;
                        triggerInfo.name = name;
                        Triggers[i].triggerInfo->push_back(triggerInfo);
                    }
                    break;
                }
            }
        }
    }
}

GameplayInfo::~GameplayInfo()
{
    for (int i = 0; i < Triggers.size(); i++) {
        delete Triggers[i].triggerInfo;
    }
}

// Dodanie informacji o danym triggerze
void GameplayInfo::AddInfo(std::string objName, std::string name)
{
    bool found = false;
    bool found2 = false;

    for (int i = 0; i < Triggers.size(); i++) {
        if (Triggers[i].objName == objName) {
            found = true;
            for (int j = 0; j < Triggers[i].triggerInfo->size(); j++) {
                if (Triggers[i].triggerInfo->at(j).name == name) {
                    found2 = true;
                    Triggers[i].triggerInfo->at(j).count++;
                    break;
                }
            }

            if (!found2) {
                TriggerInfo triggerInfo;
                triggerInfo.count = 1;
                triggerInfo.name = name;
                Triggers[i].triggerInfo->push_back(triggerInfo);
            }
            break;
        }
    }

    if (!found) {
        TriggersInfo triggersInfo;
        triggersInfo.objName = objName;
        triggersInfo.triggerInfo = new std::vector<TriggerInfo>;
        Triggers.push_back(triggersInfo);

        if (!found2) {
            for (int i = 0; i < Triggers.size(); i++) {
                if (Triggers[i].objName == objName) {
                    found = true;
                    for (int j = 0; j < Triggers[i].triggerInfo->size(); j++) {
                        if (Triggers[i].triggerInfo->at(j).name == name) {
                            found2 = true;
                            Triggers[i].triggerInfo->at(j).count++;
                            break;
                        }
                    }

                    if (!found2) {
                        TriggerInfo triggerInfo;
                        triggerInfo.count = 1;
                        triggerInfo.name = name;
                        Triggers[i].triggerInfo->push_back(triggerInfo);
                    }
                    break;
                }
            }
        }
    }
}