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

    sf::Color fill_color = sf::Color::Black;

    struct Plot
    {
        std::function<double(double)> f;
        sf::Color color;
    };
    std::vector<Plot> plots;

public:
    Canvas() = delete;
    Canvas(sf::Vector2u size);

    sf::Vector2u getImageSize() const { return image.getSize(); }

    sf::Vector2i toImageCoord(const sf::Vector2<double> real_coord) const;
    sf::Vector2<double> toRealCoord(const sf::Vector2i image_coord) const;

    void createImage(sf::Vector2u size);
    void restartViewport();

    void fill();
    void drawAxes();
    void drawPlots();
    void drawByPixels(const std::function<sf::Color(sf::Vector2<double>)> f);

    void draw(sf::RenderTarget &window);
};