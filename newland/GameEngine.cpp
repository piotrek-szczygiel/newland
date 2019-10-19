#include "GameEngine.hpp"
#include "Chronometer.hpp"
#include "Controls.hpp"
#include <Windows.h>
#include <string.h>

sf::RenderWindow* GameEngine::renderWindow;
sf::Event GameEngine::event;
sf::Text GameEngine::versionText;

sf::Vector2f GameEngine::screenDimensions;
bool GameEngine::fullscreen;
std::string GameEngine::loadMap;

bool GameEngine::mainMenu;

sf::Font GameEngine::font;

bool GameEngine::showFps;
bool GameEngine::showVersion;

sftools::Chronometer GameEngine::runtimeClock;

bool GameEngine::paused = false;

void GameEngine::PauseRuntime()
{
    paused = true;
    runtimeClock.pause();
    Game::PauseNpcs(true);
}

void GameEngine::ResumeRuntime()
{
    paused = false;
    runtimeClock.resume();
    Game::PauseNpcs(false);
}

GameEngine::~GameEngine()
{
    delete renderWindow;
}

// Inicjalizacja silnika gry
void GameEngine::Initialize(float x, float y, std::string title, bool _fullscreen, std::string arg)
{
    mainMenu = true;

    // Ustawienie rozdzielszosci okna
    screenDimensions.x = x;
    screenDimensions.y = y;

    // Wczytana mapa bedzie ta podana przez argument
    loadMap = arg;

    std::string name = "Gracz";
    // std::cout << "Wprowadz imie dla gracza: ";
    // std::cin >> name;

    // Utworzenie okna SFML na podstawie przekazanych parametrow
    renderWindow
        = new sf::RenderWindow(sf::VideoMode((unsigned int)screenDimensions.x, (unsigned int)screenDimensions.y), title,
            _fullscreen ? sf::Style::Fullscreen : sf::Style::Close);
    // renderWindow->setVerticalSyncEnabled(true);
    // Utworzenie obiektu walsciwej gry
    if (_fullscreen)
        renderWindow->setMouseCursorVisible(false);

    fullscreen = _fullscreen;

    Game::Initialize(renderWindow, name);

    // Wczytanie ikony okna
    sf::Image icon;
    if (icon.loadFromFile("resources/ui/icon.png")) {
        renderWindow->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    }

    versionText.setString(title);

    showFps = true;
    showVersion = false;
}

// Procedura uruchamiajaca wlasciwa gre
int GameEngine::Run()
{
    if (sf::Joystick::isConnected(0)) {
        std::cout << "Wykryto kontroler!\nSterowanie padem!" << std::endl;
        Controls::device = Controls::DeviceJoystick;
    } else {
        std::cout << "\nNie wykryto kontrolera!\nSterowanie klawiatura!" << std::endl;
        Controls::device = Controls::DeviceKeyboard;
    }

    font.loadFromFile("resources/tahoma.ttf");

    sf::Text fpsText;
    fpsText.setFont(font);
    fpsText.setCharacterSize(18);
    fpsText.setColor(sf::Color(0, 255, 0, 128));

    versionText.setFont(font);
    versionText.setCharacterSize(18);
    versionText.setColor(sf::Color(255, 255, 255, 128));

    // Losowanie seeda
    srand(time(NULL));

    int returnCode = 0;
    int frames = 0;

    // Glowne zegary
    // Calkowity czas dzialania gry
    sftools::Chronometer frametimeClock; // Czas renderowania pojedynczej klatki
    sf::Clock fpsClock; // Zegar umozliwiajacy obliczenie FPS-ow

    runtimeClock.reset();

    // Petla wykonujaca sie przez czas otwarcia okna gry
    while (renderWindow->isOpen() && Game::running) {
        if (mainMenu) {
            MainMenu();
            mainMenu = false;
            continue;
        }
        // Pobranie eventow okna
        while (renderWindow->pollEvent(event)) {
            // Jezeli zamknieto okno, wyjdz z gry
            if (event.type == sf::Event::Closed) {
                renderWindow->close();
                returnCode = 1;
                continue;
            }
        }

        if (Controls::IsPressed(Controls::Pause))
            PauseRuntime();
        else if (Controls::IsPressed(Controls::Unpause))
            ResumeRuntime();
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F12))
            renderWindow->close();

        // Czas pojedynczej klatki
        sf::Time frameTime;
        // Jezeli jestesmy w GUI gry (w trakcie grania)

        if (!paused) {
            // Wznawiamy zegary jezeli byly zatrzymane
            frametimeClock.resume();
            runtimeClock.resume();

            // Obliczenie czasu klatki i zresetowanie zegara
            frameTime = frametimeClock.reset(true);
            sf::Time tpf = sf::seconds(1.f / 10.f);

            // W przypadku dluzszego zaciecia gry, eliminuje przekazanie zbyt duzej wartosci czasu klatki
            // co moglo by spowodowac np. przejscie przez mur itp.
            if (frameTime > tpf)
                frameTime = tpf;

            Game::PauseNpcs(false);

            // Aktualizujemy stan gry
            Game::Update(frameTime);

            // Rysujemy klatke
            Game::Draw(*renderWindow, sf::RenderStates::Default);

            // Jezeli minela sekunda
            if (showFps && fpsClock.getElapsedTime().asSeconds() >= 1.f) {
                // To obliczamy ilosc klatek ktory wygenerowalismy oraz zerujemy zegar je zliczajacy
                float fpsTime = fpsClock.restart().asSeconds();
                float fps = (float)frames / fpsTime;
                fps = floor(fps + 0.5);

                std::string stringFps = "FPS: " + std::to_string((int)fps);
                fpsText.setString(stringFps);
                // Zerujemy zmienna zliczajaca klatki
                frames = 0;
            }

            fpsText.setPosition(Camera::cameraPosition.x + screenDimensions.x - fpsText.getGlobalBounds().width - 5,
                Camera::cameraPosition.y);
            versionText.setPosition(Camera::cameraPosition);

            if (showFps)
                renderWindow->draw(fpsText);
            if (showVersion)
                renderWindow->draw(versionText);

            // Wyswietlamy klatke z BackBuffera
            renderWindow->display();

            // Dodajemy zmienna zliczajaca klatki
            frames++;
        }
    }

    // Jezeli wyszlismy inaczej niz zamkajac okno, sprawdzamy czy jest dalej otwarte
    if (renderWindow->isOpen())
        // Jezeli jest, to zamykamy je
        renderWindow->close();

    Maps::DeleteLocations();
    // Zwracamy kod-zwrotny
    return returnCode;
}

void GameEngine::MainMenu()
{
    sf::Clock timer;

    int position = 0;
    const int max_position = 1;

    std::vector<sf::Texture*> menuTextures;
    std::vector<sf::Sprite> menuSprites;

    int actualFrame = 0;
    int maxFrames = 59;

    for (int i = 0; i < maxFrames; i++) {
        sf::Texture* frame = new sf::Texture;
        std::string file = "resources/menuanimation/";
        file += std::to_string(i + 1);
        file += ".png";
        frame->loadFromFile(file);

        sf::Sprite frameSprite(*frame);
        menuSprites.push_back(frameSprite);
        menuTextures.push_back(frame);
    }

    sf::Texture logoTexture;
    logoTexture.loadFromFile("resources/ui/logo.png");
    sf::Sprite logo(logoTexture);
    logo.setPosition(GameEngine::screenDimensions.x / 2 - (logo.getGlobalBounds().width / 2), 50);

    sf::Music muzyka;
    muzyka.openFromFile("resources/music/menu.ogg");
    muzyka.setLoop(true);
    muzyka.play();
    muzyka.setVolume(25);

    sf::Texture nowagraTexture, nowagraActiveTexture;
    nowagraTexture.loadFromFile("resources/ui/nowagra.png");
    nowagraActiveTexture.loadFromFile("resources/ui/nowagra_active.png");
    sf::Sprite nowagra(nowagraTexture);
    sf::Sprite nowagraActive(nowagraActiveTexture);

    sf::Texture wyjscieTexture, wyjscieActiveTexture;
    wyjscieTexture.loadFromFile("resources/ui/wyjscie.png");
    wyjscieActiveTexture.loadFromFile("resources/ui/wyjscie_active.png");
    sf::Sprite wyjscie(wyjscieTexture);
    sf::Sprite wyjscieActive(wyjscieActiveTexture);

    sf::Sprite* pozycje[max_position + 1] = { &nowagra, &wyjscie };
    sf::Sprite* pozycje_active[max_position + 1] = { &nowagraActive, &wyjscieActive };

    sf::Clock frameTimer;
    sf::Time lastFrame;
    sf::Time frameInverval = sf::seconds(0.05f);

    while (renderWindow->isOpen()) {
        while (renderWindow->pollEvent(event)) {
            // Jezeli zamknieto okno, wyjdz z gry
            if (event.type == sf::Event::Closed) {
                renderWindow->close();
                return;
            } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                renderWindow->close();
                return;
            }
        }

        if (Controls::IsPressed(Controls::Down)) {
            if (position + 1 <= max_position) {
                ++position;
            }
        } else if (Controls::IsPressed(Controls::Up)) {
            if (position - 1 >= 0) {
                --position;
            }
        }

        if (Controls::IsPressed(Controls::Accept)) {
            for (int i = 0; i < 59; i++) {
                delete menuTextures[i];
            }

            switch (position) {
            case 0:
                Game::LoadMap(loadMap);
                return;
                break;
            case 1:
                renderWindow->close();
                return;
                break;
            }
        }

        renderWindow->clear();

        renderWindow->draw(menuSprites[actualFrame]);

        renderWindow->draw(logo);

        if (lastFrame + frameInverval <= frameTimer.getElapsedTime())
            actualFrame++, lastFrame = frameTimer.getElapsedTime();

        if (actualFrame + 1 == maxFrames)
            actualFrame = 0;

        for (int i = 0; i <= max_position; i++) {
            if (i == position) {
                pozycje_active[i]->setPosition(
                    GameEngine::screenDimensions.x / 2 - (pozycje_active[i]->getGlobalBounds().width / 2),
                    240 + i * 60);
                renderWindow->draw(*pozycje_active[i]);
            } else {
                pozycje[i]->setPosition(
                    GameEngine::screenDimensions.x / 2 - (pozycje_active[i]->getGlobalBounds().width / 2),
                    240 + i * 60);
                renderWindow->draw(*pozycje[i]);
            }
        }

        renderWindow->display();
    }
}
