#pragma once
#include <functional>
#include <cmath>

#include "SFML/Graphics.hpp"
#include "SFML/Graphics/Image.hpp"

class Canvas
{
private:
    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;

public:
    class Viewport
    {
    public:
        sf::Vector2<double> center;
        sf::Vector2<double> size;
    } viewport;

public:
    Canvas() = delete;
    Canvas(sf::Vector2<size_t> size);

    sf::Vector2<size_t> toImageCoord(const sf::Vector2<double> real_coord) const;
    sf::Vector2<double> toRealCoord(const sf::Vector2<size_t> image_coord) const;

    void createImage(sf::Vector2<size_t> &size);
    void restartViewport();

    void fill(sf::Color color = sf::Color::Black);
    void drawAxis();
    void drawPlot(const std::function<double(double)> f, sf::Color color);
    void drawByPixels(const std::function<sf::Color(sf::Vector2<double>)> f);

    void draw(sf::RenderTarget &window);
};