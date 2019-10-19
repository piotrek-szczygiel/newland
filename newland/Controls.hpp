#ifndef _CONTROLS_H
#define _CONTROLS_H

#include <SFML/Graphics.hpp>

class Controls {
public:
    static const int Up = 0x101;
    static const int Down = 0x102;
    static const int Left = 0x103;
    static const int Right = 0x104;

    static const int Sprint = 0x105;

    static const int Interaction = 0x106;
    static const int Inventory = 0x107;
    static const int Accept = 0x108;
    static const int Back = 0x109;

    static const int Attack = 0x111;

    static const int Pickup = 0x112;

    static const int Choose1 = 0x201;
    static const int Choose2 = 0x202;
    static const int Choose3 = 0x203;
    static const int Choose4 = 0x204;

    static const int Pause = 0xF00;
    static const int Unpause = 0xF01;
    static const int Quit = 0xF03;

    static const int DeviceJoystick = 0xAAA;
    static const int DeviceKeyboard = 0xBBB;

    static const int DirectionY = 0x000;
    static const int DirectionX = 0x001;

    static int device;

    static sf::Clock timer;
    static sf::Time lastClick;
    static const sf::Time interval;

    static bool IsPressed(int button, bool lockkeys = true);
    static float Moving(int direction);
};

#endif