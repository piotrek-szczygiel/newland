#include "Npc.hpp"
#include "Location.hpp"
#include <cmath>
#include <iostream>
#include <pugixml\pugixml.hpp>
#include <sstream>

std::string as_utf8(const char* str)
{
    return str;
}
std::wstring as_wide(const wchar_t* str)
{
    return str;
}
const double M_PI = 3.14159265358979323846;
// Wczytanie npc z pliku
bool Npc::Load(std::string file)
{
    staticNpc = true;
    free = false;
    timer.reset();
    direction = 0;
    changeTime = sf::seconds(0);
    changeDirection = true;
    interaction = false;
    interaction_message = false;

    std::string npcFile = "resources/" + file;
    pugi::xml_document doc;

    player.SetSpeed(30.f);

    avFile = false;

    // Zaladowanie pliku XML
    if (doc.load_file(npcFile.c_str())) {
        pugi::xml_node panels = doc.child("npc");
        for (pugi::xml_node panel = panels.first_child(); panel; panel = panel.next_sibling()) {
            std::string name = panel.name();
            std::string value = panel.child_value();

            // Plik textury npc
            if (name == "sprite") {
                if (!player.LoadTexture("resources/sprites/" + value))
                    return false;
            }

            else if (name == "speed") {
                player.SetSpeed(std::stoi(value));
            }

            else if (name == "talkable") {
                if (value == "yes") {
                    player.talkable = true;
                } else {
                    player.talkable = false;
                }
            }

            else if (name == "free") {
                if (value == "yes") {
                    free = true;
                    staticNpc = false;
                } else {
                    free = false;
                }
            }

            // Nazwa npc
            else if (name == "name") {
                this->name = pugi::as_wide(value);
                player.SetName(this->name, sf::Color::White);
            }

            else if (name == "avatar") {
                avatarTex.loadFromFile("resources/sprites/" + value);
                avatar.setTexture(avatarTex);
                avFile = true;
            }

            // Kierunek w ktorym npc sie bedzie patrzyl
            else if (name == "direction") {
                player.SetCurrentAnimation(player.GetAnimation(std::stoi(value)));
            }
        }

        if (!avFile) {
            // Ustawienie tekstury awataru (pokazywanego podczas rozmowy z npc)
            avatar.setTexture(*player.GetTexture());
            avatar.setTextureRect(sf::IntRect(0, 0, 32, 23));
        }

        avatar.setScale(86.f / avatar.getGlobalBounds().width, 86.f / avatar.getGlobalBounds().height);
    } else
        return false;

    return true;
}

// Ustawia pole po ktorym npc sie bedzie poruszal
void Npc::SetPaths(sf::FloatRect aabb)
{
    npcPaths.push_back(aabb);
}

// Pobranie obszaru w ktorym bedziemy wywolywac trigger npc
sf::FloatRect Npc::GetInteractionRect()
{
    sf::FloatRect interactionRect;
    interactionRect.top = player.GetCollisionPoints().top - 40;
    interactionRect.left = player.GetCollisionPoints().left - 40;
    interactionRect.height = player.GetCollisionPoints().height + 80;
    interactionRect.width = player.GetCollisionPoints().width + 80;

    return interactionRect;
}

std::wstring Npc::GetName()
{
    return name;
}

sf::Sprite Npc::GetAvatar()
{
    return avatar;
}

// Rysowanie npc
void Npc::Draw(sf::RenderWindow& rt, sf::RenderStates states)
{
    // Rysowanie sprite npc
    player.DrawPlayer(rt, states);
}

// Calkowite spauzowanie
void Npc::Pause(bool pause)
{
    if (pause)
        timer.pause();
    else
        timer.resume();
}

bool Npc::CheckPath(sf::FloatRect collision)
{
    for (int i = 0; i < npcPaths.size(); i++) {
        if (collision.intersects(npcPaths[i]))
            return true;
    }
    return false;
}

void Npc::AiUpdate(float frameTime, Location* location, std::vector<sf::FloatRect>* playerCollisions)
{
    if (!staticNpc) {
        if (timer.isRunning()) {
            if (!interaction) {
                sf::Vector2f movement(0.f, 0.f);

                if (changeTime.asSeconds() <= timer.getElapsedTime().asSeconds()) {
                    changeDirection = true;
                }
                if (changeDirection) {
                    changeDirection = false;
                    int longer = floorf((1.f / (player.GetSpeed() / 70.f)) + 0.5f);
                    if (!free)
                        changeTime = sf::seconds((rand() % (3 * longer)) + 0.25) + timer.getElapsedTime();
                    else
                        changeTime = sf::seconds((rand() % (5 * longer)) + 0.25) + timer.getElapsedTime();
                    // direction = (direction + (1+(rand()%3)))%4;
                    direction = rand() % 4;
                }

                player.SetCurrentAnimation(player.GetAnimation(direction));
                if (direction == 0) {
                    movement.y -= player.GetSpeed();
                } else if (direction == 1) {
                    movement.y += player.GetSpeed();
                } else if (direction == 2) {
                    movement.x -= player.GetSpeed();
                } else if (direction == 3) {
                    movement.x += player.GetSpeed();
                }

                player.Move(movement * frameTime);

                sf::FloatRect npcCollision
                    = sf::FloatRect(player.GetPosition().x + 8, player.GetPosition().y + 32, 16, 16);

                bool collision = false;

                if ((npcCollision.left < 0) || (npcCollision.top < 0)
                    || (npcCollision.left + npcCollision.width > location->GetSize().x)
                    || (npcCollision.top + npcCollision.height > location->GetSize().y)) {
                    collision = true;
                } else if (name != L"#" && location->Collision(npcCollision)) {
                    collision = true;
                } else {
                    if (name != L"#") {
                        for (auto& playerObj = playerCollisions->begin(); playerObj != playerCollisions->end();
                             ++playerObj) {
                            if (playerObj->intersects(npcCollision)) {
                                collision = true;
                                break;
                            }
                        }
                    }
                }

                if (!free) {
                    if (!CheckPath(player.GetPathCollisionPoints())) {
                        collision = true;
                    }
                }

                if (collision) {
                    changeDirection = true;
                    player.Move(-movement * frameTime);
                }
            } else {
                sf::FloatRect npcCollision = player.GetCollisionPoints();
                sf::FloatRect playerCollision = playerCollisions->at(playerCollisions->size() - 1);

                sf::Vector2f playerPos, npcPos;
                playerPos.x = playerCollision.left + (playerCollision.width / 2);
                playerPos.y = playerCollision.top + (playerCollision.height / 2);

                npcPos.x = npcCollision.left + (npcCollision.width / 2);
                npcPos.y = npcCollision.top + (npcCollision.height / 2);

                float dx = npcPos.x - playerPos.x;
                float dy = npcPos.y - playerPos.y;

                float rotation = ((atan2(dy, dx)) * 180 / M_PI) + 180.f;

                if (rotation > 315.f || rotation <= 45.f) {
                    player.SetCurrentAnimation(player.GetAnimation(Player::Right));
                } else if (rotation > 45.f && rotation <= 135.f) {
                    player.SetCurrentAnimation(player.GetAnimation(Player::Down));
                } else if (rotation > 135.f && rotation <= 225.f) {
                    player.SetCurrentAnimation(player.GetAnimation(Player::Left));
                } else if (rotation > 225.f && rotation <= 315.f) {
                    player.SetCurrentAnimation(player.GetAnimation(Player::Up));
                }
                player.Move(sf::Vector2f(0.f, 0.f));
                player.StopAnimation();
            }
        }
    } else {
        if (interaction) {
            sf::FloatRect npcCollision = player.GetCollisionPoints();
            sf::FloatRect playerCollision = playerCollisions->at(playerCollisions->size() - 1);

            sf::Vector2f playerPos, npcPos;
            playerPos.x = playerCollision.left + (playerCollision.width / 2);
            playerPos.y = playerCollision.top + (playerCollision.height / 2);

            npcPos.x = npcCollision.left + (npcCollision.width / 2);
            npcPos.y = npcCollision.top + (npcCollision.height / 2);

            float dx = npcPos.x - playerPos.x;
            float dy = npcPos.y - playerPos.y;

            float rotation = ((atan2(dy, dx)) * 180 / M_PI) + 180.f;

            if (rotation > 315.f || rotation <= 45.f) {
                player.SetCurrentAnimation(player.GetAnimation(Player::Right));
            } else if (rotation > 45.f && rotation <= 135.f) {
                player.SetCurrentAnimation(player.GetAnimation(Player::Down));
            } else if (rotation > 135.f && rotation <= 225.f) {
                player.SetCurrentAnimation(player.GetAnimation(Player::Left));
            } else if (rotation > 225.f && rotation <= 315.f) {
                player.SetCurrentAnimation(player.GetAnimation(Player::Up));
            }
            player.Update(sf::seconds(frameTime));
        }
    }

    player.Update(sf::seconds(frameTime));
}
