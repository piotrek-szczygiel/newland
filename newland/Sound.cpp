#include "Sound.hpp"
#include <iostream>

sf::SoundBuffer Sound::playerHurt1Buffer;
sf::SoundBuffer Sound::playerHurt2Buffer;
sf::SoundBuffer Sound::skeletonWalkingBuffer;
sf::SoundBuffer Sound::skeletonDeadBuffer;
sf::SoundBuffer Sound::patykSlashBuffer;
sf::SoundBuffer Sound::leczenieBuffer;
sf::SoundBuffer Sound::pianinoBuffer;

sf::Sound Sound::playerHurt1;
sf::Sound Sound::playerHurt2;
sf::Sound Sound::skeletonWalking;
sf::Sound Sound::skeletonDead;
sf::Sound Sound::patykSlash;
sf::Sound Sound::leczenie;
sf::Sound Sound::pianino;

Sound::Sound()
{
    std::cout << "Loading sounds...\n";

    playerHurt1Buffer.loadFromFile("resources/sounds/playerHurt1.wav");
    playerHurt2Buffer.loadFromFile("resources/sounds/playerHurt2.wav");
    skeletonWalkingBuffer.loadFromFile("resources/sounds/skeletonWalking.wav");
    skeletonDeadBuffer.loadFromFile("resources/sounds/skeletonDead.wav");
    patykSlashBuffer.loadFromFile("resources/sounds/patykSlash.wav");
    leczenieBuffer.loadFromFile("resources/sounds/leczenie.ogg");
    pianinoBuffer.loadFromFile("resources/sounds/piano.ogg");

    playerHurt1.setBuffer(playerHurt1Buffer);
    playerHurt2.setBuffer(playerHurt2Buffer);
    skeletonWalking.setBuffer(skeletonWalkingBuffer);
    skeletonDead.setBuffer(skeletonDeadBuffer);
    patykSlash.setBuffer(patykSlashBuffer);
    leczenie.setBuffer(leczenieBuffer);
    pianino.setBuffer(pianinoBuffer);

    playerHurt1.setVolume(50);
    playerHurt2.setVolume(50);
    skeletonWalking.setVolume(50);
    skeletonDead.setVolume(50);
    patykSlash.setVolume(50);
    leczenie.setVolume(10);
    pianino.setVolume(50);

    playerHurt1.setRelativeToListener(true);
    playerHurt2.setRelativeToListener(true);
    patykSlash.setRelativeToListener(true);
    leczenie.setRelativeToListener(true);
    pianino.setRelativeToListener(true);

    skeletonWalking.setMinDistance(32.0f);
}
