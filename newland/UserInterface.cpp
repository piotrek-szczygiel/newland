#include "UserInterface.hpp"
#include "Controls.hpp"
#include "Game.hpp"
#include "GameEngine.hpp"
#include "Inventory.hpp"
#include "Npc.hpp"

Npc* UserInterface::npc;

std::wstring UserInterface::message;
std::wstring UserInterface::messageWait;

sf::Font UserInterface::normalFont;
sf::Font UserInterface::messageFont;

sf::Text UserInterface::messageText;
sf::Text UserInterface::timeText;

sf::Texture UserInterface::messageTexture;
sf::Sprite UserInterface::messageSprite;

float UserInterface::messageHideTime;
int UserInterface::messageType;

bool UserInterface::showInventory;

std::vector<PopupInfo> UserInterface::popups;
std::vector<PlayerPopupInfo> UserInterface::playerPopups;
std::vector<InteractionPopupInfo> UserInterface::interactionPopups;

sf::Texture UserInterface::clockTexture;
sf::Sprite UserInterface::clockSprite;

sf::Texture UserInterface::kwadratTexture;
sf::Texture UserInterface::trojkatTexture;

sf::Texture UserInterface::eTexture;
sf::Texture UserInterface::zTexture;

sf::Texture UserInterface::inventoryTexture;
sf::Sprite UserInterface::inventorySprite;

sf::Texture UserInterface::itemInfoTexture;
sf::Sprite UserInterface::itemInfoSprite;

sf::Texture UserInterface::selectedItemTexture;
sf::Sprite UserInterface::selectedItemSprite;

sf::Texture UserInterface::moneyTexture;
sf::Sprite UserInterface::moneySprite;

sf::Texture UserInterface::equippedTexture;

// Konstruktor obiektu interfejsu uzytkownika
void UserInterface::Initialize()
{
    normalFont.loadFromFile("resources/tahoma.ttf");
    messageFont.loadFromFile("resources/tahoma.ttf");

    messageText.setFont(messageFont);
    messageText.setColor(sf::Color(255, 255, 255, 255));
    messageText.setCharacterSize(13);
    messageText.setStyle(sf::Text::Bold);

    messageType = NO_MESSAGE;

    timeText.setFont(normalFont);
    timeText.setCharacterSize(18);
    timeText.setColor(sf::Color(255, 128, 0, 230));

    messageTexture.loadFromFile("resources/ui/message.png");
    messageSprite.setTexture(messageTexture);
    messageSprite.setScale(2.0f, 2.0f);

    clockTexture.loadFromFile("resources/items/zegarek.png");
    clockSprite.setTexture(clockTexture);

    inventoryTexture.loadFromFile("resources/ui/eq.png");
    inventorySprite.setTexture(inventoryTexture);

    itemInfoTexture.loadFromFile("resources/ui/item.png");
    itemInfoSprite.setTexture(itemInfoTexture);

    moneyTexture.loadFromFile("resources/ui/panel.png");
    moneySprite.setTexture(moneyTexture);

    selectedItemTexture.loadFromFile("resources/ui/selected.png");
    selectedItemSprite.setTexture(selectedItemTexture);

    equippedTexture.loadFromFile("resources/ui/equipped.png");

    trojkatTexture.loadFromFile("resources/ui/trojkat.png");
    kwadratTexture.loadFromFile("resources/ui/kwadrat.png");

    eTexture.loadFromFile("resources/ui/e.png");
    zTexture.loadFromFile("resources/ui/z.png");

    showInventory = false;
}

void UserInterface::Clear()
{
    MessageUpdate();
}

void UserInterface::Dead()
{
    sf::Music smiercMuzyka;
    smiercMuzyka.openFromFile("resources/sounds/smierc.wav");
    smiercMuzyka.setLoop(true);
    smiercMuzyka.setVolume(50);
    smiercMuzyka.play();
    sf::Texture smiercTexture;
    smiercTexture.loadFromFile("resources/ui/smierc.png");
    sf::Sprite smierc(smiercTexture);
    smierc.setPosition(Camera::cameraPosition);
    smierc.setScale(640.f / smierc.getGlobalBounds().width, 480.f / smierc.getGlobalBounds().height);

    sf::RenderWindow* rt = GameEngine::renderWindow;

    while (!Controls::IsPressed(Controls::Accept)) {
        rt->clear();
        rt->draw(smierc);
        rt->display();
    }
}

// Rysowanie interfejsu uzytkownika
void UserInterface::Draw(std::string sTime, sf::RenderWindow& rt)
{

    if (showInventory) {
        sf::Texture screenshot;
        screenshot.create(GameEngine::screenDimensions.x, GameEngine::screenDimensions.y);
        screenshot.update(rt, 0, 0);
        sf::Sprite screenshotSprite(screenshot);
        screenshotSprite.setPosition(Camera::cameraPosition);

        sf::Text moneyText;

        std::stringstream money;
        money << "$" << Inventory::money;

        moneyText.setFont(normalFont);
        moneyText.setString(money.str());
        moneyText.setCharacterSize(20);
        moneyText.setColor(sf::Color::Yellow);
        moneyText.setStyle(sf::Text::Bold);

        sf::Text description;
        description.setFont(messageFont);
        description.setCharacterSize(16);
        description.setColor(sf::Color::White);

        int nItems = Inventory::GetInventory()->size();

        sf::Texture* itemsTextures = new sf::Texture[nItems];
        sf::Sprite* itemsSprites = new sf::Sprite[nItems];

        std::vector<sf::Sprite> equippedSprites;

        for (int i = 0; i < nItems; i++) {
            if (itemsTextures[i].loadFromFile(
                    "resources/items/" + Inventory::GetInventory()->at(i).itemName + ".png")) {
                itemsSprites[i].setTexture(itemsTextures[i]);
                sf::Sprite equippedSprite_(equippedTexture);
                equippedSprites.push_back(equippedSprite_);
            }
        }

        GameEngine::PauseRuntime();
        rt.clear();

        int selectPosition = 0;

        while (1) {
            bool BREAK = false;
            while (GameEngine::renderWindow->pollEvent(GameEngine::event)) {
                // Jezeli zamknieto okno, wyjdz z gry
                if (GameEngine::event.type == sf::Event::Closed) {
                    GameEngine::renderWindow->close();
                    BREAK = true;
                    break;
                }
            }

            if (BREAK)
                break;

            if (Controls::IsPressed(Controls::Back))
                break;

            if (Controls::IsPressed(Controls::Right)) {
                if ((selectPosition + 1) < nItems)
                    selectPosition++;
            } else if (Controls::IsPressed(Controls::Left)) {
                if ((selectPosition - 1) >= 0)
                    selectPosition--;
            }

            if (Controls::IsPressed(Controls::Accept)) {
                if (Inventory::GetInventory()->at(selectPosition).equipable) {
                    if (!Inventory::GetInventory()->at(selectPosition).equipped) {
                        for (int i = 0; i < Inventory::GetInventory()->size(); i++) {
                            if (i != selectPosition) {
                                Inventory::GetInventory()->at(i).equipped = false;
                            }
                        }
                        Inventory::GetInventory()->at(selectPosition).equipped = true;
                    } else {
                        Inventory::GetInventory()->at(selectPosition).equipped = false;
                    }
                } else if (Inventory::GetInventory()->at(selectPosition).useable) {
                    if (Inventory::GetInventory()->at(selectPosition).itemName == "hp_pot") {
                        if (Game::player.hp >= Game::player.maxHp)
                            break;

                        Game::player.hp += 30;
                        if (Game::player.hp > Game::player.maxHp)
                            Game::player.hp = Game::player.maxHp;

                        Inventory::DeleteItem("hp_pot");
                        break;
                    }
                }
            }

            sf::Vector2f inventoryPosition(
                Camera::cameraPosition.x + ((Camera::screenDimensions.x - inventorySprite.getGlobalBounds().width) / 2),
                Camera::cameraPosition.y + 100
                    + ((Camera::screenDimensions.y - inventorySprite.getGlobalBounds().height) / 2));

            inventorySprite.setPosition(inventoryPosition);
            itemInfoSprite.setPosition(
                Camera::cameraPosition.x + ((Camera::screenDimensions.x - itemInfoSprite.getGlobalBounds().width) / 2),
                inventoryPosition.y - 200);
            description.setPosition(itemInfoSprite.getPosition().x + 10.f, itemInfoSprite.getPosition().y + 40.f);

            moneySprite.setPosition(
                Camera::cameraPosition.x + Camera::screenDimensions.x - moneySprite.getGlobalBounds().width,
                Camera::cameraPosition.y + Camera::screenDimensions.y - moneySprite.getGlobalBounds().height - 50.f);
            moneyText.setPosition(moneySprite.getPosition().x + 10.f, moneySprite.getPosition().y + 10.f);

            rt.draw(screenshotSprite);
            rt.draw(inventorySprite);
            rt.draw(itemInfoSprite);
            rt.draw(moneySprite);
            rt.draw(moneyText);

            for (int i = 0; i < nItems; i++) {
                if (i > 11) {
                    itemsSprites[i].setPosition(
                        inventoryPosition.x + ((i - 12) * 54) + 20, 166 + (inventoryPosition.y));
                } else if (i > 5) {
                    itemsSprites[i].setPosition(inventoryPosition.x + ((i - 6) * 54) + 20, 113 + (inventoryPosition.y));
                } else {
                    itemsSprites[i].setPosition(inventoryPosition.x + (i * 54) + 20, 60 + (inventoryPosition.y));
                }
                rt.draw(itemsSprites[i]);

                if (Inventory::GetInventory()->at(i).count > 1) {
                    sf::Text count;
                    count.setFont(normalFont);
                    count.setCharacterSize(10);
                    count.setString(std::to_string(Inventory::GetInventory()->at(i).count));
                    count.setPosition(itemsSprites[i].getPosition().x + 27, itemsSprites[i].getPosition().y + 22);
                    rt.draw(count);
                }

                if (Inventory::GetInventory()->at(i).equipped) {
                    equippedSprites[i].setPosition(
                        itemsSprites[i].getPosition().x - 6, itemsSprites[i].getPosition().y - 6);
                    rt.draw(equippedSprites[i]);
                }
            }

            selectedItemSprite.setPosition(
                itemsSprites[selectPosition].getPosition().x - 8, itemsSprites[selectPosition].getPosition().y - 8);

            if (nItems != 0)
                rt.draw(selectedItemSprite);

            if (nItems != 0)
                description.setString(
                    Inventory::GetDescription(Inventory::GetInventory()->at(selectPosition).itemName));

            rt.draw(description);
            rt.display();
        }

        delete[] itemsTextures, itemsSprites;
        showInventory = false;
        GameEngine::ResumeRuntime();
    }

    if (Inventory::GetItemInfo("zegarek").count > 0) {
        timeText.setString(sTime);
        timeText.setPosition(Camera::cameraPosition.x + GameEngine::screenDimensions.x - 150,
            Camera::cameraPosition.y + GameEngine::screenDimensions.y - 20 - timeText.getGlobalBounds().height);
        clockSprite.setPosition(timeText.getPosition().x - 35, timeText.getPosition().y - 5);
        rt.draw(clockSprite);
        rt.draw(timeText);
    }
    DrawMessage(rt);
    DrawPopups(rt);
}

void UserInterface::Cutscenka(std::string scena)
{
    sf::RenderWindow* rt = GameEngine::renderWindow;
    int volume = Game::bgMusic.getVolume();
    Game::bgMusic.setVolume(5);

    if (scena == "plaza") {
        sf::Music lektor;
        lektor.openFromFile("resources/cutscenes/plaza.wav");
        lektor.setVolume(100);
        lektor.setRelativeToListener(true);
        sf::Texture obraz;
        obraz.loadFromFile("resources/cutscenes/plaza.png");
        sf::Sprite obrazSprite(obraz);
        obrazSprite.setScale(640.f / obrazSprite.getGlobalBounds().width, 480.f / obrazSprite.getGlobalBounds().height);
        obrazSprite.setPosition(Camera::cameraPosition);
        rt->clear();
        rt->draw(obrazSprite);
        rt->display();
        lektor.play();
        sf::Clock timer;
        while (timer.getElapsedTime().asSeconds() < 18.5f) {
            if (Controls::IsPressed(Controls::Back))
                break;
        }
        lektor.stop();
    } else if (scena == "pustynia") {
        sf::Music lektor;
        lektor.openFromFile("resources/cutscenes/pustynia.wav");
        lektor.setVolume(100);
        lektor.setRelativeToListener(true);
        sf::Texture obraz;
        obraz.loadFromFile("resources/cutscenes/pustynia.png");
        sf::Sprite obrazSprite(obraz);
        obrazSprite.setScale(640.f / obrazSprite.getGlobalBounds().width, 480.f / obrazSprite.getGlobalBounds().height);
        obrazSprite.setPosition(Camera::cameraPosition);
        rt->clear();
        rt->draw(obrazSprite);
        rt->display();
        lektor.play();
        sf::Clock timer;
        while (timer.getElapsedTime().asSeconds() < 24.5f) {
            if (Controls::IsPressed(Controls::Back))
                break;
        }
        lektor.stop();
    } else if (scena == "karczma") {
        sf::Music lektor;
        lektor.openFromFile("resources/cutscenes/karczma.wav");
        lektor.setVolume(100);
        lektor.setRelativeToListener(true);
        sf::Texture obraz;
        obraz.loadFromFile("resources/cutscenes/karczma.png");
        sf::Sprite obrazSprite(obraz);
        obrazSprite.setScale(640.f / obrazSprite.getGlobalBounds().width, 480.f / obrazSprite.getGlobalBounds().height);
        obrazSprite.setPosition(Camera::cameraPosition);
        rt->clear();
        rt->draw(obrazSprite);
        rt->display();
        lektor.play();
        sf::Clock timer;
        while (timer.getElapsedTime().asSeconds() < 25.5f) // < czas trwania
        {
            if (Controls::IsPressed(Controls::Back))
                break;
        }
        lektor.stop();
    } else if (scena == "lochy") {
        Game::bgMusic.setVolume(0);
        sf::Music lektor;
        lektor.openFromFile("resources/cutscenes/lochy.wav");
        lektor.setVolume(100);
        lektor.setRelativeToListener(true);
        sf::Texture obraz;
        obraz.loadFromFile("resources/cutscenes/lochy.png");
        sf::Sprite obrazSprite(obraz);
        obrazSprite.setScale(640.f / obrazSprite.getGlobalBounds().width, 480.f / obrazSprite.getGlobalBounds().height);
        obrazSprite.setPosition(Camera::cameraPosition);
        rt->clear();
        rt->draw(obrazSprite);
        rt->display();
        lektor.play();
        sf::Clock timer;
        while (timer.getElapsedTime().asSeconds() < 21.5f) // < czas trwania
        {
            if (Controls::IsPressed(Controls::Back))
                break;
        }
        lektor.stop();
    } else if (scena == "butelka") {
        sf::RenderStates blendStates;
        blendStates.blendMode = sf::BlendMultiply;
        sf::RenderTexture darken;
        darken.create(640, 480);
        darken.clear(sf::Color(64, 64, 64, 128));
        darken.display();
        sf::Sprite darkenSprite(darken.getTexture());
        darkenSprite.setPosition(Camera::cameraPosition);

        sf::Music lektor;
        lektor.openFromFile("resources/cutscenes/butelka.wav");
        lektor.setVolume(100);
        lektor.setRelativeToListener(true);
        sf::Texture obraz;
        obraz.loadFromFile("resources/cutscenes/butelka.png");
        sf::Sprite obrazSprite(obraz);
        obrazSprite.setScale(640.f / obrazSprite.getGlobalBounds().width, 480.f / obrazSprite.getGlobalBounds().height);
        obrazSprite.setPosition(Camera::cameraPosition);

        rt->draw(darkenSprite, blendStates);
        rt->draw(obrazSprite);
        rt->display();

        lektor.play();

        sf::Clock timer;
        while (timer.getElapsedTime().asSeconds() < 39.5f) {
            if (Controls::IsPressed(Controls::Back))
                break;
        }
        lektor.stop();
    }

    Game::bgMusic.setVolume(volume);
}

void UserInterface::ShowInventory()
{
    showInventory = true;
}

void UserInterface::Sterowanie()
{
    sf::Texture sterowanieTexture;
    if (Controls::device == Controls::DeviceJoystick)
        sterowanieTexture.loadFromFile("resources/ui/padologia.png");
    else
        sterowanieTexture.loadFromFile("resources/ui/klawiszologia.png");

    sf::Sprite sterowanie(sterowanieTexture);
    sterowanie.setPosition(Camera::cameraPosition);

    sf::RenderWindow* rt = GameEngine::renderWindow;

    while (true) {
        if (Controls::IsPressed(Controls::Back) || Controls::IsPressed(Controls::Accept))
            break;
        rt->clear();
        rt->draw(sterowanie);
        rt->display();
    }
}

void UserInterface::MessageUpdate()
{
    if (messageType != WAIT_MESSAGE) {
        message = L"";
        messageType = NO_MESSAGE;
    }
}

void UserInterface::MessageClear()
{
    message = L"";
    messageType = NO_MESSAGE;
    npc = NULL;
}

// Wiadmosc dla uzytkownika
void UserInterface::Message(std::wstring _message)
{
    message = _message;

    messageType = NORMAL_MESSAGE;
}

// Wiadomosc dla uzytkownika z czasem wygasania
void UserInterface::MessageWait(std::wstring _message, Npc* _npc, float timeout)
{
    message = _message;
    messageType = WAIT_MESSAGE;

    messageHideTime = GameEngine::runtimeClock.getElapsedTime().asSeconds() + timeout;

    npc = _npc;
}

// Rysowanie wiadomosci
void UserInterface::DrawMessage(sf::RenderWindow& rt)
{
    if (messageType == NORMAL_MESSAGE) {
        sf::Vector2f position = Camera::cameraPosition;
        messageSprite.setPosition(
            ((GameEngine::screenDimensions.x - messageSprite.getGlobalBounds().width) / 2) + position.x,
            GameEngine::screenDimensions.y - messageSprite.getGlobalBounds().height + position.y - 3);
        rt.draw(messageSprite);

        messageText.setString(message);

        position.y = position.y + GameEngine::screenDimensions.y - (GameEngine::screenDimensions.y / 3);

        messageText.setPosition(position.x + 30, position.y + 30);
        rt.draw(messageText);
    } else if (messageType == WAIT_MESSAGE) {
        sf::Vector2f position = Camera::cameraPosition;
        messageSprite.setPosition(
            ((GameEngine::screenDimensions.x - messageSprite.getGlobalBounds().width) / 2) + position.x,
            GameEngine::screenDimensions.y - messageSprite.getGlobalBounds().height + position.y - 3);
        rt.draw(messageSprite);

        messageText.setString(message);
        position.y = position.y + GameEngine::screenDimensions.y - (GameEngine::screenDimensions.y / 3);

        messageText.setPosition(position.x + 120, position.y + 15);
        rt.draw(messageText);

        if (npc != NULL) {
            sf::Sprite avatar;
            avatar = npc->GetAvatar();
            avatar.setPosition(position.x + 20.0f, position.y + 35.0f);
            rt.draw(avatar);
        }

        if (messageHideTime <= GameEngine::runtimeClock.getElapsedTime().asSeconds())
            MessageClear();
    }
}

bool UserInterface::MessageShown()
{
    if (messageType == NORMAL_MESSAGE || messageType == WAIT_MESSAGE)
        return true;
    else
        return false;
}

// Okienka popup dla uzytkownika z czasem wygasania i pozycja pokazania wiadomosci
void UserInterface::Popup(
    std::wstring wstr, sf::Vector2f centerPosition, float hideTime, sf::Color color, bool eraseLast)
{
    sf::Text text(wstr, normalFont, (unsigned int)GameEngine::screenDimensions.y / 40);
    text.setColor(color);
    text.setCharacterSize(15);
    text.setPosition(
        centerPosition.x - (text.getGlobalBounds().width / 2), centerPosition.y - (32 + text.getGlobalBounds().height));

    sf::RectangleShape bg;
    bg.setFillColor(sf::Color(64, 64, 64, 128));
    bg.setOutlineThickness(2.0f);
    bg.setOutlineColor(sf::Color::Black);

    if (text.getPosition().x < Camera::cameraPosition.x)
        text.setPosition(Camera::cameraPosition.x, text.getPosition().y);
    else if ((text.getPosition().x + text.getGlobalBounds().width)
        > (Camera::cameraPosition.x + GameEngine::screenDimensions.x))
        text.setPosition(Camera::cameraPosition.x + GameEngine::screenDimensions.x - text.getGlobalBounds().width,
            text.getPosition().y);

    PopupInfo popupInfo;
    popupInfo.text = text;
    popupInfo.bg = bg;
    popupInfo.hideTime = GameEngine::runtimeClock.getElapsedTime().asSeconds() + hideTime;
    popupInfo.difference = popupInfo.hideTime - GameEngine::runtimeClock.getElapsedTime().asSeconds();

    if (eraseLast && popups.size() > 0)
        popups.pop_back();

    popups.push_back(popupInfo);
}

// j.w tylko zamiast pozycji przekazujemy obiekt gracza/npc nad ktorym bedzie widniala wiadomosc
void UserInterface::Popup(std::wstring wstr, Player* player, float hideTime, sf::Color color, bool eraseLast)
{
    sf::Text text(wstr, normalFont);
    text.setColor(color);
    text.setCharacterSize(15);

    sf::RectangleShape bg;
    bg.setFillColor(sf::Color(64, 64, 64, 128));
    bg.setOutlineThickness(2.0f);
    bg.setOutlineColor(sf::Color::Black);

    PopupInfo popupInfo;
    popupInfo.text = text;
    popupInfo.bg = bg;
    popupInfo.hideTime = GameEngine::runtimeClock.getElapsedTime().asSeconds() + hideTime;
    popupInfo.difference = hideTime;

    PlayerPopupInfo playerPopupInfo;
    playerPopupInfo.player = player;
    playerPopupInfo.popupInfo = popupInfo;

    if (eraseLast && playerPopups.size() > 0)
        playerPopups.pop_back();

    playerPopups.push_back(playerPopupInfo);
}

void UserInterface::InteractionPopup(
    std::wstring wstr, sf::Vector2f centerPosition, int button, float hideTime, sf::Color color, bool eraseLast)
{
    sf::Text text(wstr, normalFont, (unsigned int)GameEngine::screenDimensions.y / 40);
    text.setColor(color);
    text.setCharacterSize(15);
    text.setPosition(
        centerPosition.x - (text.getGlobalBounds().width / 2), centerPosition.y - (32 + text.getGlobalBounds().height));

    sf::RectangleShape bg;
    bg.setFillColor(sf::Color(64, 64, 64, 128));
    bg.setOutlineThickness(2.0f);
    bg.setOutlineColor(sf::Color::Black);

    if (text.getPosition().x < Camera::cameraPosition.x)
        text.setPosition(Camera::cameraPosition.x + 48, text.getPosition().y);
    else if ((text.getPosition().x + text.getGlobalBounds().width)
        > (Camera::cameraPosition.x + GameEngine::screenDimensions.x))
        text.setPosition(Camera::cameraPosition.x + GameEngine::screenDimensions.x - text.getGlobalBounds().width,
            text.getPosition().y);

    InteractionPopupInfo popupInfo;
    popupInfo.text = text;
    popupInfo.bg = bg;
    popupInfo.hideTime = GameEngine::runtimeClock.getElapsedTime().asSeconds() + hideTime;
    popupInfo.difference = popupInfo.hideTime - GameEngine::runtimeClock.getElapsedTime().asSeconds();
    popupInfo.button = button;

    if (eraseLast && interactionPopups.size() > 0)
        interactionPopups.pop_back();

    interactionPopups.push_back(popupInfo);
}

// Rysowanie okienek popup
void UserInterface::DrawPopups(sf::RenderWindow& rt)
{
    float runTime = GameEngine::runtimeClock.getElapsedTime().asSeconds();
    for (int i = 0; i < popups.size(); i++) {
        if (popups[i].hideTime <= runTime) {
            if (popups[i].difference == 0)
                rt.draw(popups[i].text);
            popups.erase(popups.begin() + i);
            continue;
        }

        if (popups[i].difference != 0.0f) {
            sf::Color color = popups[i].text.getColor();
            color.a = (sf::Uint8)(((popups[i].hideTime - runTime) / popups[i].difference) * 255.0f);
            popups[i].text.setColor(color);

            color = popups[i].bg.getFillColor();
            color.a = (sf::Uint8)(((popups[i].hideTime - runTime) / popups[i].difference) * 255.0f);
            popups[i].bg.setFillColor(color);

            color = popups[i].bg.getOutlineColor();
            color.a = (sf::Uint8)(((popups[i].hideTime - runTime) / popups[i].difference) * 255.0f);
            popups[i].bg.setOutlineColor(color);
        }

        rt.draw(popups[i].text);
    }
    for (int i = 0; i < interactionPopups.size(); i++) {
        sf::Sprite button;
        if (Controls::device == Controls::DeviceJoystick)
            button.setTexture(interactionPopups[i].button == Kwadrat ? kwadratTexture : trojkatTexture);
        else
            button.setTexture(interactionPopups[i].button == Kwadrat ? zTexture : eTexture);

        button.setPosition(
            interactionPopups[i].text.getPosition().x - 35, interactionPopups[i].text.getPosition().y - 7);

        if (interactionPopups[i].hideTime <= runTime) {
            if (interactionPopups[i].difference == 0) {
                rt.draw(button);
                rt.draw(interactionPopups[i].text);
            }
            interactionPopups.erase(interactionPopups.begin() + i);
            continue;
        }

        if (interactionPopups[i].difference != 0.0f) {
            sf::Color color = interactionPopups[i].text.getColor();
            color.a
                = (sf::Uint8)(((interactionPopups[i].hideTime - runTime) / interactionPopups[i].difference) * 255.0f);
            interactionPopups[i].text.setColor(color);

            color = interactionPopups[i].bg.getFillColor();
            color.a
                = (sf::Uint8)(((interactionPopups[i].hideTime - runTime) / interactionPopups[i].difference) * 255.0f);
            interactionPopups[i].bg.setFillColor(color);

            color = interactionPopups[i].bg.getOutlineColor();
            color.a
                = (sf::Uint8)(((interactionPopups[i].hideTime - runTime) / interactionPopups[i].difference) * 255.0f);
            interactionPopups[i].bg.setOutlineColor(color);

            color = button.getColor();
            color.a
                = (sf::Uint8)(((interactionPopups[i].hideTime - runTime) / interactionPopups[i].difference) * 255.0f);
            button.setColor(color);
        }

        rt.draw(button);
        rt.draw(interactionPopups[i].text);
    }
    for (int i = 0; i < playerPopups.size(); i++) {
        if (playerPopups[i].popupInfo.hideTime <= runTime) {
            if (playerPopups[i].popupInfo.difference == 0) {
                sf::Vector2f centerPosition(
                    playerPopups[i].player->GetPosition().x + 16, playerPopups[i].player->GetPosition().y - 32);
                playerPopups[i].popupInfo.text.setPosition(
                    centerPosition.x - (playerPopups[i].popupInfo.text.getGlobalBounds().width / 2),
                    centerPosition.y - playerPopups[i].popupInfo.text.getGlobalBounds().height);

                if (playerPopups[i].popupInfo.text.getPosition().x < Camera::cameraPosition.x)
                    playerPopups[i].popupInfo.text.setPosition(
                        Camera::cameraPosition.x + 4, playerPopups[i].popupInfo.text.getPosition().y);
                else if ((playerPopups[i].popupInfo.text.getPosition().x
                             + playerPopups[i].popupInfo.text.getGlobalBounds().width)
                    > (Camera::cameraPosition.x + GameEngine::screenDimensions.x))
                    playerPopups[i].popupInfo.text.setPosition(Camera::cameraPosition.x + GameEngine::screenDimensions.x
                            - playerPopups[i].popupInfo.text.getGlobalBounds().width - 4,
                        playerPopups[i].popupInfo.text.getPosition().y);

                playerPopups[i].popupInfo.bg.setSize(
                    sf::Vector2f(playerPopups[i].popupInfo.text.getGlobalBounds().width + 6,
                        playerPopups[i].popupInfo.text.getGlobalBounds().height + 6));
                playerPopups[i].popupInfo.bg.setPosition(playerPopups[i].popupInfo.text.getGlobalBounds().left - 3,
                    playerPopups[i].popupInfo.text.getGlobalBounds().top - 3);

                rt.draw(playerPopups[i].popupInfo.bg);
                rt.draw(playerPopups[i].popupInfo.text);
            }
            playerPopups.erase(playerPopups.begin() + i);
            continue;
        }

        if (playerPopups[i].popupInfo.difference != 0.0f) {
            sf::Color color = playerPopups[i].popupInfo.text.getColor();
            color.a = (sf::Uint8)(
                ((playerPopups[i].popupInfo.hideTime - runTime) / playerPopups[i].popupInfo.difference) * 255.0f);
            playerPopups[i].popupInfo.text.setColor(color);

            color = playerPopups[i].popupInfo.bg.getFillColor();
            color.a = (sf::Uint8)(
                ((playerPopups[i].popupInfo.hideTime - runTime) / playerPopups[i].popupInfo.difference) * 255.0f);
            playerPopups[i].popupInfo.bg.setFillColor(color);

            color = playerPopups[i].popupInfo.bg.getOutlineColor();
            color.a = (sf::Uint8)(
                ((playerPopups[i].popupInfo.hideTime - runTime) / playerPopups[i].popupInfo.difference) * 255.0f);
            playerPopups[i].popupInfo.bg.setOutlineColor(color);
        }

        sf::Vector2f centerPosition(
            playerPopups[i].player->GetPosition().x + 16, playerPopups[i].player->GetPosition().y - 32);
        playerPopups[i].popupInfo.text.setPosition(
            centerPosition.x - (playerPopups[i].popupInfo.text.getGlobalBounds().width / 2),
            centerPosition.y - playerPopups[i].popupInfo.text.getGlobalBounds().height);

        if (playerPopups[i].popupInfo.text.getPosition().x < Camera::cameraPosition.x)
            playerPopups[i].popupInfo.text.setPosition(
                Camera::cameraPosition.x + 4, playerPopups[i].popupInfo.text.getPosition().y);
        else if ((playerPopups[i].popupInfo.text.getPosition().x
                     + playerPopups[i].popupInfo.text.getGlobalBounds().width)
            > (Camera::cameraPosition.x + GameEngine::screenDimensions.x))
            playerPopups[i].popupInfo.text.setPosition(Camera::cameraPosition.x + GameEngine::screenDimensions.x
                    - playerPopups[i].popupInfo.text.getGlobalBounds().width - 4,
                playerPopups[i].popupInfo.text.getPosition().y);

        playerPopups[i].popupInfo.bg.setSize(sf::Vector2f(playerPopups[i].popupInfo.text.getGlobalBounds().width + 6,
            playerPopups[i].popupInfo.text.getGlobalBounds().height + 6));
        playerPopups[i].popupInfo.bg.setPosition(playerPopups[i].popupInfo.text.getGlobalBounds().left - 3,
            playerPopups[i].popupInfo.text.getGlobalBounds().top - 3);

        rt.draw(playerPopups[i].popupInfo.bg);
        rt.draw(playerPopups[i].popupInfo.text);
    }
}
