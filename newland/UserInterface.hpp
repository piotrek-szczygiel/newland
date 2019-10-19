#ifndef _USER_INTERFACE_H
#define _USER_INTERFACE_H

#include <SFML/Graphics.hpp>

class Npc;
class Game;
class Inventory;
class Player;

struct PopupInfo {
    sf::Text text;
    sf::RectangleShape bg;
    float hideTime;
    float difference;
};

struct PlayerPopupInfo {
    PopupInfo popupInfo;
    Player* player;
};

struct InteractionPopupInfo {
    sf::Text text;
    sf::RectangleShape bg;
    float hideTime;
    float difference;
    int button;
};

class UserInterface {
public:
    enum { NO_MESSAGE, NORMAL_MESSAGE, WAIT_MESSAGE };
    enum { Trojkat, Kwadrat };

    static Npc* npc;

    static sf::Texture trojkatTexture;
    static sf::Texture kwadratTexture;

    static sf::Texture eTexture;
    static sf::Texture zTexture;

    static std::wstring message;
    static std::wstring messageWait;

    static sf::Font normalFont;
    static sf::Font messageFont;

    static sf::Text messageText;
    static sf::Text timeText;

    static sf::Texture messageTexture;
    static sf::Sprite messageSprite;

    static float messageHideTime;
    static int messageType;

    static bool showInventory;

    static std::vector<PopupInfo> popups;
    static std::vector<PlayerPopupInfo> playerPopups;
    static std::vector<InteractionPopupInfo> interactionPopups;

    static sf::Texture clockTexture;
    static sf::Sprite clockSprite;

    static sf::Texture inventoryTexture;
    static sf::Sprite inventorySprite;
    static sf::Texture itemInfoTexture;
    static sf::Sprite itemInfoSprite;

    static sf::Texture moneyTexture;
    static sf::Sprite moneySprite;

    static sf::Texture selectedItemTexture;
    static sf::Sprite selectedItemSprite;

    static sf::Texture equippedTexture;

    static void Initialize();

    static void Cutscenka(std::string scena);

    static void ConfigureGui(sf::RenderWindow& rt);
    static void GuiEvent(sf::Event* event);

    static void SetInventory(Inventory* inventory);
    static void SetGame(Game* game);

    static void DrawMessage(sf::RenderWindow& rt);

    static void Draw(std::string sTime, sf::RenderWindow& rt);
    static void Clear();

    static void ShowInventory();

    static void Sterowanie();
    static void Dead();

    static void MessageClear();
    static void MessageUpdate();

    static void Message(std::wstring message);
    static void MessageWait(std::wstring message, Npc* npc, float timeout = 2.0f);

    static bool MessageShown();

    static void Popup(std::wstring wstr, sf::Vector2f centerPosition, float hideTime = 2.0f,
        sf::Color color = sf::Color::White, bool eraseLast = false);
    static void Popup(std::wstring wstr, Player* player, float hideTime = 2.0f, sf::Color color = sf::Color::White,
        bool eraseLast = false);
    static void InteractionPopup(std::wstring wstr, sf::Vector2f centerPosition, int button, float hideTime = 2.0f,
        sf::Color color = sf::Color::Magenta, bool eraseLast = false);
    static void DrawPopups(sf::RenderWindow& rt);
};

#endif