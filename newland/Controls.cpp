#include "Controls.hpp"

int Controls::device;
sf::Clock Controls::timer;
sf::Time Controls::lastClick;
const sf::Time Controls::interval = sf::seconds(0.3f);

bool Controls::IsPressed(int button, bool lockkeys)
{
    bool ret = false;

    if (!lockkeys || ((lastClick + interval) <= timer.getElapsedTime())) {
        if (device == DeviceJoystick) {
            sf::Joystick::update();

            switch (button) {
            case Up:
                ret = (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y) < -80.0f);
                break;
            case Down:
                ret = (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y) > 80.0f);
                break;
            case Left:
                ret = (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X) < -80.0f);
                break;
            case Right:
                ret = (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X) > 80.0f);
                break;
            case Sprint:
                ret = (sf::Joystick::isButtonPressed(0, 5));
                break;
            case Attack:
                ret = (sf::Joystick::isButtonPressed(0, 7));
                break;
            case Interaction:
                ret = (sf::Joystick::isButtonPressed(0, 0));
                break;
            case Inventory:
                ret = (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovY) == -100);
                break;
            case Pickup:
                ret = (sf::Joystick::isButtonPressed(0, 3));
                break;
            case Accept:
                ret = (sf::Joystick::isButtonPressed(0, 2));
                break;
            case Back:
                ret = (sf::Joystick::isButtonPressed(0, 1));
                break;
            case Choose1:
                ret = (sf::Joystick::isButtonPressed(0, 6));
                break;
            case Choose2:
                ret = (sf::Joystick::isButtonPressed(0, 4));
                break;
            case Choose3:
                ret = (sf::Joystick::isButtonPressed(0, 7));
                break;
            case Choose4:
                ret = (sf::Joystick::isButtonPressed(0, 5));
                break;
            case Pause:
                ret = (sf::Joystick::isButtonPressed(0, 8));
                break;
            case Unpause:
                ret = (sf::Joystick::isButtonPressed(0, 9));
                break;
            default:
                ret = false;
                break;
            }
        } else if (device == DeviceKeyboard) {
            switch (button) {
            case Up:
                ret = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up));
                break;
            case Down:
                ret = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down));
                break;
            case Left:
                ret = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left));
                break;
            case Right:
                ret = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right));
                break;
            case Sprint:
                ret = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift));
                break;
            case Attack:
                ret = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space));
                break;
            case Interaction:
                ret = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E));
                break;
            case Inventory:
                ret = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::I));
                break;
            case Pickup:
                ret = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z));
                break;
            case Accept:
                ret = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Return));
                break;
            case Back:
                ret = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape));
                break;
            case Choose1:
                ret = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1));
                break;
            case Choose2:
                ret = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2));
                break;
            case Choose3:
                ret = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3));
                break;
            case Choose4:
                ret = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num4));
                break;
            case Pause:
                ret = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Pause));
                break;
            case Unpause:
                ret = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::End));
                break;
            default:
                ret = false;
            }
        }
    }

    if (ret && lockkeys) {
        lastClick = timer.getElapsedTime();
    }
    return ret;
}

float Controls::Moving(int direction)
{
    if (device == DeviceJoystick) {
        sf::Joystick::update();
        switch (direction) {
        case DirectionY:
            return (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y));
            break;
        case DirectionX:
            return (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X));
            break;
        }
    } else if (device == DeviceKeyboard) {
        switch (direction) {
        case DirectionY:
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
                return -100;
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
                return 100;
            break;
        case DirectionX:
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
                return 100;
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
                return -100;
            break;
        }
    }

    return 0.0f;
}