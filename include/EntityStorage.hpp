#pragma once
#include <functional>

#include "SFML/Graphics.hpp"

struct EntityStorage
{
    struct Plot
    {
        std::function<double(double)> f;
        sf::Color color;
    };
    std::vector<Plot> plots;
};