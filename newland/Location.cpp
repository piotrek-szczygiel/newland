#include "Location.hpp"
#include "Game.hpp"
#include "Player.hpp"
const double M_PI = 3.14159265358979323846;
// Konstruktor obiektu lokacji
Location::Location()
{
    npcs = new std::vector<Npc*>();
    enemies = new std::vector<Enemy*>();

    blendStates.blendMode = sf::BlendMultiply;
    blendStates2.blendMode = sf::BlendAdd;

    noDaylightColor = sf::Color::Color(128, 128, 128);

    daynightColors.push_back(DayNightColorsInfo(sf::Color::Color(230, 230, 215), false));
    daynightColors.push_back(DayNightColorsInfo(sf::Color::Color(196, 196, 192), false));
    daynightColors.push_back(DayNightColorsInfo(sf::Color::Color(162, 162, 169), false));
    daynightColors.push_back(DayNightColorsInfo(sf::Color::Color(128, 128, 146), false));
    daynightColors.push_back(DayNightColorsInfo(sf::Color::Color(94, 94, 123), true));
    daynightColors.push_back(DayNightColorsInfo(sf::Color::Color(60, 60, 100), true));
}

Location::~Location()
{
    for (int i = 0; i < npcs->size(); i++) {
        delete npcs->at(i);
    }

    for (int i = 0; i < enemies->size(); i++) {
        delete enemies->at(i);
    }

    for (int i = 0; i < pickups.size(); i++) {
        delete pickups[i];
    }

    for (int i = 0; i < textures.size(); i++) {
        delete textures[i].texture;
    }

    for (int i = 0; i < animations.size(); i++) {
        delete animations[i].animatedSprite;
        delete animations[i].animation;
    }

    delete npcs, enemies;
}

std::vector<tmx::MapObject>* Location::GetTriggers()
{
    for (int i = 0; i < objectLayers.size(); i++) {
        if (objectLayers[i].layerName == "Trigger") {
            return objectLayers[i].objects;
        }
    }

    return NULL;
}

std::vector<tmx::MapObject>* Location::GetObjects()
{
    for (int i = 0; i < objectLayers.size(); i++) {
        if (objectLayers[i].layerName == "Object") {
            return objectLayers[i].objects;
        }
    }

    return NULL;
}

std::vector<Npc*>* Location::GetNpcs()
{
    return npcs;
}

Npc* Location::GetNpc(std::wstring name)
{
    for (int i = 0; i < npcs->size(); i++) {
        if (npcs->at(i)->GetName() == name)
            return npcs->at(i);
    }

    return NULL;
}

std::string Location::GetBgMusic()
{
    return bgMusic;
}

void Location::SetLayerVisible(std::string layerName, bool visible)
{
    for (auto layer = mapLoader->getLayers().begin(); layer != mapLoader->getLayers().end(); ++layer) {
        if (layer->name == layerName)
            layer->visible = visible;
    }
}

// Procedura wczytujaca mape
void Location::Load(std::string fileName)
{
    // Wczytanie mapy z danego pliku
    mapLoader = new tmx::MapLoader("resources/maps/");
    mapLoader->addSearchPath("resources/tilesets/");
    mapLoader->load(fileName + ".tmx");

    lightTex.loadFromFile("resources/sprites/light.png");
    lightTex.setSmooth(true);

    lightBase.create(GetSize().x, GetSize().y);
    lightBase.clear();

    for (auto layer = mapLoader->getLayers().begin(); layer != mapLoader->getLayers().end(); ++layer) {
        // Dodanie obiektow mapy do tablicy objektow
        if (layer->type == tmx::MapLayerType::ObjectGroup) {
            ObjectLayerInfo layerInfo;
            layerInfo.layerName = layer->name;
            layerInfo.objects = new std::vector<tmx::MapObject>;
            for (auto& obj : layer->objects)
                layerInfo.objects->push_back(obj);

            objectLayers.push_back(layerInfo);
        }
    }

    // Wczytanie nazwy mapy
    locationName = mapLoader->getPropertyString("level") == "" ? fileName : mapLoader->getPropertyString("level");
    parentLocation = mapLoader->getPropertyString("parent") == "" ? fileName : mapLoader->getPropertyString("parent");
    daylight = mapLoader->getPropertyString("daylight") == "true" ? true : false;

    // Wczytanie nazwy pliku myzki w tle
    bgMusic = "resources/music/" + mapLoader->getPropertyString("music");

    for (auto layer = objectLayers.begin(); layer != objectLayers.end(); ++layer) {
        if (layer->layerName == "Npc") {
            // Dodawanie npc
            for (auto obj = layer->objects->begin(); obj != layer->objects->end(); ++obj) {
                Npc* npc;
                npc = new Npc();
                if (!npc->Load(obj->getPropertyString("file"))) {
                    delete npc;
                    continue;
                }
                npc->player.SetPosition(obj->getPosition());
                npc->player.Move(sf::Vector2f(0, 0));

                if (!npc->free) {
                    bool found = false;

                    for (int i = 0; i < objectLayers.size(); i++) {
                        if (objectLayers[i].layerName == "NpcPaths") {
                            for (int j = 0; j < objectLayers[i].objects->size(); j++) {
                                if (obj->getName() == objectLayers[i].objects->at(j).getName()) {
                                    npc->SetPaths(objectLayers[i].objects->at(j).getAABB());
                                    found = true;
                                }
                            }
                            break;
                        }
                    }

                    if (!found)
                        npc->staticNpc = true;
                    else
                        npc->staticNpc = false;
                }

                npcs->push_back(npc);
            }
        } else if (layer->layerName == "Enemies") {
            // Dodawanie npc
            for (auto obj = layer->objects->begin(); obj != layer->objects->end(); ++obj) {
                for (int i = 0; i < 4; i++) {
                    Enemy* enemy;
                    enemy = new Enemy();
                    if (!enemy->Load(obj->getPropertyString("file"))) {
                        delete enemy;
                        continue;
                    }
                    switch (i) {
                    case 0:
                        enemy->SetPosition(sf::Vector2f(obj->getAABB().left + 40, obj->getAABB().top + 40));
                        break;
                    case 1:
                        enemy->SetPosition(
                            sf::Vector2f(obj->getAABB().left + 40, obj->getAABB().top + (obj->getAABB().height) - 40));
                        break;
                    case 2:
                        enemy->SetPosition(
                            sf::Vector2f(obj->getAABB().left + (obj->getAABB().width) - 40, obj->getAABB().top + 40));
                        break;
                    case 3:
                        enemy->SetPosition(sf::Vector2f(obj->getAABB().left + (obj->getAABB().width) - 40,
                            obj->getAABB().top + (obj->getAABB().height) - 40));
                        break;
                    case 4:
                        enemy->SetPosition(sf::Vector2f(obj->getAABB().left + (obj->getAABB().width / 2),
                            obj->getAABB().top + (obj->getAABB().height / 2)));
                        break;
                    default:
                        break;
                    }
                    enemy->Move(sf::Vector2f(0, 0));
                    enemy->SetPaths(obj->getAABB());
                    enemies->push_back(enemy);
                }
            }

        } else if (layer->layerName == "Light") {
            for (auto obj = layer->objects->begin(); obj != layer->objects->end(); ++obj) {
                sf::Sprite light(lightTex);
                float scalex, scaley;
                scalex = scaley
                    = obj->getPropertyString("scale") == "" ? 1.f : std::atof(obj->getPropertyString("scale").c_str());
                light.setScale(scalex, scaley);
                light.setPosition(obj->getCentre().x - (light.getGlobalBounds().width / 2),
                    obj->getCentre().y - (light.getGlobalBounds().height / 2));

                std::string lightColor
                    = obj->getPropertyString("color") == "" ? "white" : obj->getPropertyString("color");

                if (lightColor == "yellow")
                    light.setColor(sf::Color(255, 255, 128));
                else if (lightColor == "orange")
                    light.setColor(sf::Color(255, 128, 64));

                lights.push_back(light);
            }
        } else if (layer->layerName == "Pickup") {
            for (auto obj = layer->objects->begin(); obj != layer->objects->end(); ++obj) {
                Pickup* pickup = new Pickup(obj->getName(), obj->getPosition());
                pickups.push_back(pickup);
            }
        } else if (layer->layerName == "Animation") {
            // Dodawanie npc
            for (auto obj = layer->objects->begin(); obj != layer->objects->end(); ++obj) {
                std::string file = "resources/" + obj->getPropertyString("file");
                std::string spriteFile;
                pugi::xml_document doc;
                int frames = 0;
                float delay = 0.2f;
                int level = 1;

                if (doc.load_file(file.c_str())) {
                    sf::Texture* spriteSheet = new sf::Texture;
                    pugi::xml_node panels = doc.child("animation");
                    for (pugi::xml_node panel = panels.first_child(); panel; panel = panel.next_sibling()) {
                        std::string name = panel.name();
                        std::string value = panel.child_value();

                        // Plik textury npc
                        if (name == "sprite") {
                            std::string filepath = "resources/sprites/" + value;
                            bool found = false;

                            for (int i = 0; i < textures.size(); i++) {
                                if (textures[i].filepath == filepath) {
                                    spriteSheet = textures[i].texture;
                                    found = true;
                                }
                            }

                            if (!found) {
                                spriteSheet->loadFromFile(filepath);

                                TextureInfo texInf;
                                texInf.filepath = filepath;
                                texInf.texture = spriteSheet;
                                textures.push_back(texInf);
                            }
                        }

                        // Nazwa npc
                        else if (name == "frames") {
                            frames = std::atoi(value.c_str());
                        }

                        // Kierunek w ktorym npc sie bedzie patrzyl
                        else if (name == "delay") {
                            delay = (float)std::atoi(value.c_str()) / 1000.f;
                        }

                        else if (name == "level") {
                            level = std::atoi(value.c_str());
                        }
                    }

                    AnimationInfo animationInfo;
                    Animation* animation = new Animation;
                    AnimatedSprite* animatedSprite = new AnimatedSprite(sf::seconds(delay));

                    animation->setSpriteSheet(*spriteSheet);
                    for (int i = 0; i < frames; i++)
                        animation->addFrame(sf::IntRect(i * 32, 0, 32, 32));

                    animatedSprite->setPosition(obj->getPosition());
                    animatedSprite->play(*animation);

                    animationInfo.animatedSprite = animatedSprite;
                    animationInfo.animation = animation;

                    animationInfo.level = level;

                    animations.push_back(animationInfo);
                }
            }
        }
    }
}

sf::Vector2f Location::GetSpawnPoint(std::string portal)
{
    for (int i = 0; i < objectLayers.size(); i++) {
        if (objectLayers[i].layerName == "Portal") {
            for (int j = 0; j < objectLayers[i].objects->size(); j++) {
                if (objectLayers[i].objects->at(j).getName() == portal) {
                    return objectLayers[i].objects->at(j).getPosition();
                }
            }
        }
    }

    return sf::Vector2f(0.f, 0.f);
}

void Location::PauseNpcs(bool pause)
{
    for (int i = 0; i < npcs->size(); i++) {
        npcs->at(i)->Pause(pause);
    }
}

bool compareNpcs(Npc* npc1, Npc* npc2)
{
    return (npc1->player.GetPosition().y < npc2->player.GetPosition().y);
}

// Rysowanie mapy
void Location::Draw(
    sf::RenderWindow& rt, sf::RenderStates states, Player* player, float runTime, float frameTime, int daynightType)
{
    std::sort(npcs->begin(), npcs->end(), compareNpcs);

    for (int i = 0; i < npcs->size(); i++) {
        std::vector<sf::FloatRect> playerCollisions;

        for (int j = 0; j < npcs->size(); j++)
            if (j != i)
                playerCollisions.push_back(npcs->at(j)->player.GetCollisionPoints());

        playerCollisions.push_back(player->GetCollisionPoints());

        npcs->at(i)->AiUpdate(frameTime, this, &playerCollisions);
    }

    for (int i = 0; i < enemies->size(); i++) {
        std::vector<sf::FloatRect> playerCollisions;

        if (!enemies->at(i)->dead) {
            for (int j = 0; j < enemies->size(); j++)
                if (j != i && !enemies->at(j)->dead)
                    playerCollisions.push_back(enemies->at(j)->GetCollisionPoints());

            playerCollisions.push_back(player->GetCollisionPoints());
        }

        enemies->at(i)->AiUpdate(frameTime, this, &playerCollisions, player->GetPosition());
    }

    // Rysowanie tylnich warst mapy (tych po ktorych gracz sie porusza)
    int backLayers = 0;

    for (int i = 0; i < animations.size(); i++) {
        if (animations[i].level == 0) {
            animations[i].animatedSprite->update(sf::seconds(frameTime));
            rt.draw(*animations[i].animatedSprite, states);
        }
    }

    for (auto layer = mapLoader->getLayers().begin(); layer != mapLoader->getLayers().end(); ++layer) {
        if (layer->name == "Cover")
            break;

        rt.draw(*layer, states);
        backLayers++;
    }

    for (int i = 0; i < animations.size(); i++) {
        if (animations[i].level == 1) {
            animations[i].animatedSprite->update(sf::seconds(frameTime));
            rt.draw(*animations[i].animatedSprite, states);
        }
    }

    for (int i = 0; i < pickups.size(); i++) {
        pickups[i]->Draw(rt);
    }

    // Rysowanie npc-ow ktorzy sa za graczem
    for (int i = 0; i < npcs->size(); i++)
        if (npcs->at(i)->player.GetCollisionPoints().top < player->GetCollisionPoints().top)
            npcs->at(i)->Draw(rt, states);

    for (int i = 0; i < enemies->size(); i++)
        if (!enemies->at(i)->dead && enemies->at(i)->GetCollisionPoints().top < player->GetCollisionPoints().top)
            enemies->at(i)->DrawEnemy(rt, states);

    // Rysowanie gracza
    player->DrawPlayer(rt, states);

    // Rysowanie npc-ow ktorzy sa przed graczem
    for (int i = 0; i < npcs->size(); i++)
        if (npcs->at(i)->player.GetCollisionPoints().top >= player->GetCollisionPoints().top)
            npcs->at(i)->Draw(rt, states);

    for (int i = 0; i < enemies->size(); i++)
        if (!enemies->at(i)->dead && enemies->at(i)->GetCollisionPoints().top >= player->GetCollisionPoints().top)
            enemies->at(i)->DrawEnemy(rt, states);

    // Rysowanie przednich warstw mapy (tych za ktorymi gracz sie chowa)
    for (auto layer = mapLoader->getLayers().begin() + backLayers; layer != mapLoader->getLayers().end(); ++layer)
        rt.draw(*layer, states);

    for (int i = 0; i < animations.size(); i++) {
        if (animations[i].level == 2) {
            animations[i].animatedSprite->update(sf::seconds(frameTime));
            rt.draw(*animations[i].animatedSprite, states);
        }
    }

    // Rysowanie nietoperzy
    for (int i = 0; i < npcs->size(); i++)
        if (npcs->at(i)->GetName() == L"#")
            npcs->at(i)->Draw(rt, states);

    for (int i = 0; i < npcs->size(); i++)
        npcs->at(i)->player.DrawNick(rt, states, npcs->at(i)->interaction_message);

    for (int i = 0; i < enemies->size(); i++)
        enemies->at(i)->DrawNick(rt, states);

    player->DrawNick(rt, states);

    if (this->daynightType != daynightType) {
        this->daynightType = daynightType;
        if (daylight) {
            lightBase.clear(daynightColors[daynightType].color);

            if (daynightColors[daynightType].lights)
                for (int i = 0; i < lights.size(); i++)
                    lightBase.draw(lights[i], blendStates2);
        } else {
            lightBase.clear(noDaylightColor);
            for (int i = 0; i < lights.size(); i++)
                lightBase.draw(lights[i], blendStates2);
        }

        lightBase.display();
        lightBaseSpr.setTexture(lightBase.getTexture());
    }

    rt.draw(lightBaseSpr, blendStates);
}

void Location::AttackEnemy(sf::FloatRect playerWeaponHitbox)
{
    if ((lastEnemyHit + sf::seconds(0.35f)) < timer.getElapsedTime()) {
        for (int i = 0; i < enemies->size(); i++) {
            if (!enemies->at(i)->dead && enemies->at(i)->GetHitRange().intersects(playerWeaponHitbox)) {
                std::cout << "Enemy hit!\n";
                lastEnemyHit = timer.getElapsedTime();
                enemies->at(i)->Damage(Game::player.damage);

                if (enemies->at(i)->dead) {
                    if ((rand() % 2) == 1) {
                        Pickup* pickup = new Pickup("czaszka", enemies->at(i)->GetPosition());
                        pickups.push_back(pickup);
                    }
                }
            }
        }
    }
}

sf::Vector2u Location::GetSize()
{
    return mapLoader->getMapSize();
}

bool Location::NpcCollision(sf::FloatRect playerCollisionRect)
{
    for (int i = 0; i < npcs->size(); i++) {
        if (npcs->at(i)->player.GetCollisionPoints().intersects(playerCollisionRect))
            return true;
    }

    for (int i = 0; i < enemies->size(); i++) {
        if (!enemies->at(i)->dead && enemies->at(i)->GetCollisionPoints().intersects(playerCollisionRect))
            return true;
    }

    return false;
}

// Sprawdzenie kolizji dla podanego obszaru
bool Location::Collision(sf::FloatRect playerCollisionRect)
{
    bool collision = false;
    for (int i = 0; i < objectLayers.size(); i++) {
        if (objectLayers[i].layerName == "Collision" || objectLayers[i].layerName == "Object") {
            for (int j = 0; j < objectLayers[i].objects->size(); j++) {
                if (objectLayers[i].objects->at(j).getAABB().intersects(playerCollisionRect))
                    return true;
            }
        }
    }

    /*// Sprawdzenie kolizji z danym npc
    for(int i = 0; i <npcs->size(); i++)
    {
            if(npcs->at(i)->player.GetCollisionPoints().intersects(playerCollisionRect))
                    return true;
    }*/

    return false;
}

std::string Location::GetName()
{
    return locationName;
}

std::string Location::GetParent()
{
    return parentLocation;
}
