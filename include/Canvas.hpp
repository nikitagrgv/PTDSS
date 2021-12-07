#pragma once
#include <cmath>
#include <functional>

#include "SFML/Graphics.hpp"

class PTDSS;

class Canvas
{
private:
    PTDSS *ptdss;
    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;

    sf::Color fill_color = sf::Color::Black;

    struct Viewport
    {
        sf::Vector2<double> center;
        sf::Vector2<double> size;
    } viewport;

public:
    Canvas() = delete;
    Canvas(sf::Vector2u size, PTDSS *ptdss);

    sf::Vector2u getImageSize() const { return image.getSize(); }

    Viewport &getViewport() { return viewport; }

    void setFillColor(sf::Color new_color) { fill_color = new_color; }

    sf::Vector2i toImageCoord(const sf::Vector2<double> real_coord) const;
    sf::Vector2<double> toRealCoord(const sf::Vector2i image_coord) const;

    sf::Vector2i getMousePosImage() const;
    sf::Vector2<double> getMousePosReal() const;

    void createImage(sf::Vector2u size);
    void restartViewport();

    void fill();
    void drawAxes();
    void drawPlots();
    void drawByPixels(const std::function<sf::Color(sf::Vector2<double>)> f);

    void draw(sf::RenderTarget &window);




    //DEBUG
    void drawMousePos();
};