#pragma once

#include "SFML/Graphics.hpp"
#include "SFML/System/Clock.hpp"

#include "Canvas.hpp"
#include "EntityStorage.hpp"

class PTDSS
{
private:
    sf::RenderWindow window;
    Canvas canvas;
    EntityStorage entity_storage;
    sf::Clock clock;
    double dt;

public:
    PTDSS(sf::Vector2u window_size);

    sf::RenderWindow &getWindow() { return window; }
    Canvas &getCanvas() { return canvas; }
    EntityStorage &getEntityStorage() { return entity_storage; }

    double getTime() const { return clock.getElapsedTime().asMicroseconds() / 1e6; };
    double getDt() const { return dt; }

    sf::Vector2i getMousePosWindow() { return sf::Mouse::getPosition(window); }

    void processInput();
    void draw();
    void run();
};