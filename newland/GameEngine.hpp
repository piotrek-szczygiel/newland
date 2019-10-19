#ifndef _GAME_ENGINE_H
#define _GAME_ENGINE_H

#include "Chronometer.hpp"
#include "Game.hpp"

class GameEngine {
public:
    static sf::RenderWindow* renderWindow;
    static sf::Event event;
    static sf::Text versionText;

    static sf::Vector2f screenDimensions;
    static bool fullscreen;
    static std::string loadMap;

    static bool mainMenu;

    static sf::Font font;

    static bool showFps;
    static bool showVersion;

    static sftools::Chronometer runtimeClock;

    static bool paused;

    ~GameEngine();

    static void Initialize(float x, float y, std::string title, bool _fullscreen, std::string arg);
    static int Run();

    static void PauseRuntime();
    static void ResumeRuntime();

    static void MainMenu();
};

#endif
