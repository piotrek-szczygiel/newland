#ifndef _INVENTORY_H
#define _INVENTORY_H

#include <string>
#include <vector>

struct InventoryInfo {
    std::string itemName;
    int count;
    bool equipped;
    bool equipable;
    bool useable;
};

struct Descriptions {
    Descriptions(std::string item, std::wstring info)
        : itemName(item)
        , description(info)
    {
    }
    std::string itemName;
    std::wstring description;
};

class Inventory {
public:
    static std::vector<InventoryInfo>* inventory;
    static std::vector<Descriptions>* descriptions;

    static void Initialize();

    static InventoryInfo GetItemInfo(std::string itemName);
    static std::vector<InventoryInfo>* GetInventory();

    static void AddItem(std::string itemName, int count = 1);
    static void DeleteItem(std::string itemName, int count = 1);

    static std::wstring GetDescription(std::string itemName);

    static int money;

    ~Inventory();
};

#endif