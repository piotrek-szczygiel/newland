#include "Inventory.hpp"
#include "GameplayInfo.hpp"
#include "UserInterface.hpp"

std::vector<InventoryInfo>* Inventory::inventory;
std::vector<Descriptions>* Inventory::descriptions;
int Inventory::money = 0;

Inventory::~Inventory()
{
    delete[] inventory;
    delete[] descriptions;
}

void Inventory::Initialize()
{
    inventory = new std::vector<InventoryInfo>;
    descriptions = new std::vector<Descriptions>;

    descriptions->push_back(
        Descriptions("stara_notatka", L"Notatka znaleziona w starej dziupli.\nNie można się z niej odczytać."));
    descriptions->push_back(
        Descriptions("przepustka", L"Przepustka zakupiona u handlarza.\nUpoważnia kupców do przejścia przez most."));
    descriptions->push_back(
        Descriptions("zegarek", L"Najnowszy wynalazek lokalnego starca.\nUmożliwia odczytanie dokładnej pory dnia."));
    descriptions->push_back(Descriptions("patyk",
        L"Patyk znaleziony na ziemi.\nPodstawowe narzędzie walki zadające 5 obrażeń.\n\nWalczysz za pomocą przycisku "
        L"[SPACJA]\nMożesz go ubrać naciskając [ENTER]"));
    descriptions->push_back(Descriptions("miecz",
        L"Stalowy miecz.\nZadaje 15 obrażeń.\n\nWalczysz za pomocą przycisku [SPACJA]\nMożesz go ubrać naciskając "
        L"[ENTER]"));
    descriptions->push_back(Descriptions("butelka", L"Butelka z listem w środku.\nZawiera starą notatkę."));
    descriptions->push_back(
        Descriptions("kosci", L"Kosci wydobyte z grobowca w lochach.\nZnachorka potrzebuje ich do zrobienia mikstur."));
    descriptions->push_back(Descriptions("butelka_pusta", L"Pusta manierka. Służy do przechowywania wody."));
    descriptions->push_back(Descriptions("butelka_pelna", L"Manierka z wodą"));
    descriptions->push_back(
        Descriptions("hp_pot", L"Mikstura odnawiająca 30 zdrowia.\nMożesz jej użyć naciskając [ENTER]"));
    descriptions->push_back(Descriptions("czaszka", L"Łup ze szkieleta."));
    descriptions->push_back(Descriptions("lekarstwa", L"Trudno dostępne lekarstwo."));
    descriptions->push_back(Descriptions("klucz_zabojca", L"Klucz do dobrze zabezpieczonej skrzyni."));
    descriptions->push_back(
        Descriptions("zaklecie", L"Stare zaklęcie. Prawdopodobnie otwiera\nswego rodzaju przejście."));
}

std::vector<InventoryInfo>* Inventory::GetInventory()
{
    return inventory;
}

std::wstring Inventory::GetDescription(std::string itemName)
{
    for (int i = 0; i < descriptions->size(); i++) {
        if (descriptions->at(i).itemName == itemName) {
            return descriptions->at(i).description;
        }
    }

    return L"";
}

InventoryInfo Inventory::GetItemInfo(std::string itemName)
{
    for (int i = 0; i < inventory->size(); i++) {
        if (inventory->at(i).itemName == itemName) {
            return inventory->at(i);
        }
    }

    // jezeli nie znaleziono przedmiotu zwracamy puste "info"

    InventoryInfo noInfo;
    noInfo.count = 0;
    noInfo.itemName = "";

    return noInfo;
}

void Inventory::AddItem(std::string itemName, int count)
{
    bool found = false;

    for (int i = 0; i < inventory->size(); i++) {
        if (inventory->at(i).itemName == itemName) {
            found = true;
            inventory->at(i).count += count;
        }
    }

    if (!found) // brak przedmiotu wiec trzeba go utworzyc
    {
        InventoryInfo newInfo;
        newInfo.count = count;
        newInfo.itemName = itemName;
        newInfo.equipped = false;

        bool equipable = false;
        bool useable = false;

        if (itemName == "patyk" || itemName == "miecz")
            equipable = true;
        else if (itemName == "hp_pot")
            useable = true;

        newInfo.equipable = equipable;
        newInfo.useable = useable;

        inventory->push_back(newInfo);

        if (itemName == "butelka") {
            if (GameplayInfo::GetInfo("cutscenka", "butelka").count == 0) {
                GameplayInfo::AddInfo("cutscenka", "butelka");
                // Game::bgMusic.stop();
                UserInterface::Cutscenka("butelka");
            }
        }
    }
}

void Inventory::DeleteItem(std::string itemName, int count)
{
    for (int i = 0; i < inventory->size(); i++) {
        if (inventory->at(i).itemName == itemName) {
            inventory->at(i).count -= count;

            if (inventory->at(i).count <= 0
                || count == 0) // jezeli usunieto tyle przedmiotow ze juz nie ma wcale itemu, usuwamy go z tablicy
            {
                inventory->erase(inventory->begin() + i);
            }
        }
    }
}