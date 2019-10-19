#include "Game.hpp"
#include "Controls.hpp"
#include "Inventory.hpp"
#include "Sound.hpp"
#include <iomanip>

Player Game::player;

sf::Time Game::frameTime;

float Game::dayTimer;
std::stringstream Game::ssTime;

sf::Music Game::bgMusic;

bool Game::music;
int Game::daynightType;
int Game::playerDirection;
bool Game::changeMap;
bool Game::running = true;

// Procedura wczytujaca mape
void Game::LoadMap(std::string map, std::string portal)
{
    changeMap = true;

    std::string oldMusic = "";
    if (Maps::currentLocation != NULL) {
        oldMusic = Maps::currentLocation->GetBgMusic();
        Maps::currentLocation->PauseNpcs(true);
    }
    // Ustawienie lokacji i sprawdzenie bledu
    if (Maps::SetCurrentLocation(map) == NULL) {
        // Jezeli nie ma takiej mapy do wczytania
        std::cout << "Unknow map!" << std::endl;
        Maps::AddCacheLocation(map);
        if (Maps::SetCurrentLocation(map) == NULL) {
            // Jezeli nie ma takiej mapy do wczytania
            std::cout << "Couldn't load map!" << std::endl;
            std::cin.get();

            // Wychodzimy z gry
            running = false;
            return;
        }
    }

    // Ustawienie pozycji gracza na podstawie ustawionego punktu odrodzenia
    if (Maps::currentLocation != NULL)
        player.SetPosition(Maps::currentLocation->GetSpawnPoint(portal));

    // Ustawienie odpowiedniej muzyki w tle
    if (oldMusic != Maps::currentLocation->GetBgMusic()) {
        bgMusic.stop();
        if (bgMusic.openFromFile(Maps::currentLocation->GetBgMusic())) {
            bgMusic.setLoop(true);
            bgMusic.setVolume(25);
            bgMusic.setRelativeToListener(true);
            bgMusic.play();
        }
    }

    Maps::currentLocation->PauseNpcs(false);
}

// Wlaczenie / Wylaczenie muzyki w tle
void Game::ToggleMusic()
{
    music = !music;

    if (music) {
        bgMusic.setVolume(25);
    } else {
        bgMusic.setVolume(0);
    }
}

// Konstruktor obiektu gry
void Game::Initialize(sf::RenderWindow* win, std::string name)
{
    Sound::Sound();
    Gameplay::Gameplay();
    changeMap = false;
    music = true;

    Maps::AddCacheLocation("plaza");

    Maps::AddCacheLocation("pustynia");
    Maps::AddCacheLocation("lochy");

    Maps::AddCacheLocation("galicja");
    Maps::AddCacheLocation("karczma");
    Maps::AddCacheLocation("domek1");
    Maps::AddCacheLocation("domek1_pietro");
    Maps::AddCacheLocation("domek1_piwnica");
    Maps::AddCacheLocation("domek2");
    Maps::AddCacheLocation("domek2_pietro");

    Maps::AddCacheLocation("farma");

    // Wczytanie tekstury gracza
    player.LoadTexture("resources/sprites/player.png");
    player.SetName(pugi::as_wide(name), sf::Color::Cyan);
    player.mainPlayer = true;

    // Zerowanie czasu dnia
    dayTimer = 0.f;

    running = true;

    daynightType = 0;

    Inventory::Initialize();
    UserInterface::Initialize();
}

// Aktualizowanie stanu gry
void Game::Update(sf::Time _frameTime)
{
    if (player.dead) {
        bgMusic.stop();
        UserInterface::Dead();
        GameEngine::renderWindow->close();
        return;
    }
    frameTime = _frameTime;

    // Zmienna do ktorej zapisujemy ruch gracza w aktualnej klatce
    sf::Vector2f movement(0.f, 0.f);

    // Zmienna okreslajaca czy naciskamy ktorys z klawiszow poruszania
    bool noKeyWasPressed = true;

    // Obliczenie danej godziny na podstawie czasu dzialania gry
    dayTimer = fmod(GameEngine::runtimeClock.getElapsedTime().asSeconds(), 360.f) / 6.f;

    if (dayTimer < 26.f)
        daynightType = 0;
    else if (dayTimer < 27.f)
        daynightType = 1;
    else if (dayTimer < 28.f)
        daynightType = 2;
    else if (dayTimer < 29.f)
        daynightType = 3;
    else if (dayTimer < 30.f)
        daynightType = 4;
    else if (dayTimer < 56.f)
        daynightType = 5;
    else if (dayTimer < 57.f)
        daynightType = 4;
    else if (dayTimer < 58.f)
        daynightType = 3;
    else if (dayTimer < 59.f)
        daynightType = 2;
    else if (dayTimer < 60.f)
        daynightType = 1;

    // ssTime przechowuje godzine - np. "Godzina: 16:54"
    ssTime.str(std::string());
    ssTime.clear();

    ssTime << "Godzina: " << std::setfill('0') << std::setw(2)
           << floorf(((fmod(dayTimer + 17.5, 60.f) / 2.5f) * 3600.f) / 3600.f) << ":" << std::setfill('0')
           << std::setw(2)
           << floorf(fmod(((fmod(dayTimer + 17.5, 60.f) / 2.5f) * 3600.f) / 60.f,
                  60.f)); // obliczanie minut na podstawie timera

    // Obsluga poruszania sie postacia - ustawienie odpowiedniej animacji oraz ruchu gracza

    float Ymov, Xmov;

    if ((Ymov = Controls::Moving(Controls::DirectionY)) < -10.f) {
        player.SetCurrentAnimation(player.GetAnimation(Player::Up));
        movement.y += player.GetSpeed() * (Ymov / 100.f);
        noKeyWasPressed = false;
        playerDirection = Player::Up;
    } else if ((Ymov = Controls::Moving(Controls::DirectionY)) > 10.f) {
        player.SetCurrentAnimation(player.GetAnimation(Player::Down));
        movement.y -= player.GetSpeed() * (Ymov / -100.f);
        noKeyWasPressed = false;
        playerDirection = Player::Down;
    }
    if ((Xmov = Controls::Moving(Controls::DirectionX)) > 10.0f) {
        player.SetCurrentAnimation(player.GetAnimation(Player::Right));
        movement.x += player.GetSpeed() * (Xmov / 100.f);
        noKeyWasPressed = false;
        playerDirection = Player::Right;
    } else if ((Xmov = Controls::Moving(Controls::DirectionX)) < -10.0f) {
        player.SetCurrentAnimation(player.GetAnimation(Player::Left));
        movement.x -= player.GetSpeed() * (Xmov / -100.f);
        noKeyWasPressed = false;
        playerDirection = Player::Left;
    }

    // Obsluga sprintu
    if (Controls::IsPressed(Controls::Sprint, false))
        movement.x *= 1.5, movement.y *= 1.5;

    // Poruszenie graczem o odpowiednia ilosc pikseli
    player.Move(movement * frameTime.asSeconds());

    // Test kolizji
    if ((Maps::MapCollision(player.GetCollisionPoints())
            || Maps::currentLocation->Collision(player.GetCollisionPoints())
            || Maps::currentLocation->NpcCollision(player.GetCollisionPoints()))) {
        // Jezeli wystepuja kolizja to cofamy ruch gracza - w wyniku gracz w ogole sie nie poruszy
        player.Move(-movement * frameTime.asSeconds());

        if (movement.x != 0.f && movement.y != 0.f) {
            player.Move(sf::Vector2f(movement.x, 0.f) * frameTime.asSeconds());
            if (Maps::MapCollision(player.GetCollisionPoints())
                || Maps::currentLocation->Collision(player.GetCollisionPoints())
                || Maps::currentLocation->NpcCollision(player.GetCollisionPoints())) {
                noKeyWasPressed = true;
                player.Move(sf::Vector2f(-movement.x, 0.f) * frameTime.asSeconds());
                player.Move(sf::Vector2f(0.f, movement.y) * frameTime.asSeconds());
                if (Maps::MapCollision(player.GetCollisionPoints())
                    || Maps::currentLocation->Collision(player.GetCollisionPoints())
                    || Maps::currentLocation->NpcCollision(player.GetCollisionPoints())) {
                    player.Move(sf::Vector2f(0.f, -movement.y) * frameTime.asSeconds());
                    noKeyWasPressed = true;
                } else
                    noKeyWasPressed = false;
            }
        } else
            noKeyWasPressed = true;
    }
    // Poruszenie kamery
    Camera::Move(player.GetPosition());
    sf::Listener::setPosition(player.GetPosition().x, 0, player.GetPosition().y);

    if (Controls::IsPressed(Controls::Attack, false)) {
        player.SwingSword();
    }

    // Jezeli nie naciskamy klawiszy ruchu - zatrzymujemy animacje postaci
    if (noKeyWasPressed) {
        player.StopAnimation();
    }
    noKeyWasPressed = true;

    // Aktualizacja animacji ruchu postaci
    player.Update(frameTime);

    // Aktualizacja shadera wody

    // Procedury obslugujaca interakcje uzytkownika ze swiatem (np. npc, itemy)
    Gameplay::Interaction();
}

// Procedura rysujaca klatke
void Game::Draw(sf::RenderWindow& rt, sf::RenderStates states)
{
    if (changeMap) {
        changeMap = false;

        // Ustawienie wymiarow kamery
        Camera::SetDimensions(GameEngine::screenDimensions, Maps::currentLocation->GetSize());
        Camera::Move(player.GetPosition());
    }

    // Czyszczenie okna
    rt.clear();
    // Ustawienie widoku
    rt.setView(Camera::view);

    // Rysowanie mapy, gracza i npc
    Maps::currentLocation->Draw(rt, states, &player, GameEngine::runtimeClock.getElapsedTime().asSeconds(),
        frameTime.asSeconds(), daynightType);

    // Rysowanie interfejsu gracza (rozmowy z NPC, okienka popup, GUI gracza)
    UserInterface::Draw(ssTime.str(), rt);
}

void Game::PauseNpcs(bool pause)
{
    Maps::currentLocation->PauseNpcs(pause);
}

// Procedura wychodzenia z gry
void Game::Close()
{
    running = false;
}
