#ifndef _SOUND_H
#define _SOUND_H

#include <SFML/Audio.hpp>

class Sound {
public:
    static sf::SoundBuffer playerHurt1Buffer;
    static sf::SoundBuffer playerHurt2Buffer;
    static sf::SoundBuffer skeletonWalkingBuffer;
    static sf::SoundBuffer skeletonDeadBuffer;
    static sf::SoundBuffer patykSlashBuffer;
    static sf::SoundBuffer leczenieBuffer;
    static sf::SoundBuffer pianinoBuffer;

    static sf::Sound playerHurt1;
    static sf::Sound playerHurt2;
    static sf::Sound skeletonWalking;
    static sf::Sound skeletonDead;
    static sf::Sound patykSlash;
    static sf::Sound leczenie;
    static sf::Sound pianino;

    Sound();
};

#endif