#include "Canvas.hpp"

Canvas::Canvas(sf::Vector2u size)
{
    createImage(size);
    restartViewport();
}

sf::Vector2u Canvas::toImageCoord(const sf::Vector2<double> real_coord) const
{
    sf::Vector2u image_coord;
    image_coord.x = round(image.getSize().x * (+real_coord.x - viewport.center.x + 0.5 * viewport.size.x) / viewport.size.x);
    image_coord.y = round(image.getSize().y * (-real_coord.y + viewport.center.y + 0.5 * viewport.size.y) / viewport.size.y);
    return image_coord;
}

sf::Vector2<double> Canvas::toRealCoord(const sf::Vector2u image_coord) const
{
    sf::Vector2<double> real_coord;
    real_coord.x = viewport.center.x + ((double)image_coord.x / image.getSize().x - 0.5) * viewport.size.x;
    real_coord.y = viewport.center.y - ((double)image_coord.y / image.getSize().y - 0.5) * viewport.size.y;
    return real_coord;
}

void Canvas::createImage(sf::Vector2u size)
{
    image.create(size.x, size.y);
}

void Canvas::fill()
{
    for (size_t py = 0; py < image.getSize().y; py++)
    {
        for (size_t px = 0; px < image.getSize().x; px++)
        {
            image.setPixel(px, py, fill_color);
        }
    }
}

void Canvas::restartViewport()
{
    viewport.center = {0, 0};
    viewport.size = {10., 10. * image.getSize().y / image.getSize().x};
}

void Canvas::drawAxis()
{
    ssize_t px = toImageCoord({0, 0}).x;
    ssize_t py = toImageCoord({0, 0}).y;

    sf::Color colorx = sf::Color::Red;
    sf::Color colory = sf::Color::Green;

    if (py < 0)
    {
        py = 6;
        colorx.r /= 3;
    }
    else if (py >= image.getSize().y)
    {
        py = image.getSize().y - 7;
        colorx.r /= 3;
    }

    if (px < 0)
    {
        px = 6;
        colory.g /= 3;
    }
    else if (px >= image.getSize().x)
    {
        px = image.getSize().x - 7;
        colory.g /= 3;
    }

    for (size_t i = 0; i < image.getSize().x; i++)
    {
        image.setPixel(i, py, colorx);
    }

    for (size_t i = 0; i < image.getSize().y; i++)
    {
        image.setPixel(px, i, colory);
    }
}

void Canvas::drawPlots()
{
    for (size_t px = 0; px < image.getSize().x; px++)
    {
        for (auto plot : plots)
        {
            double x = toRealCoord({px, 0}).x;
            double y = plot.f(x);

            size_t py = toImageCoord({x, y}).y;
            if (py >= 0 && py < image.getSize().y)
            {
                image.setPixel(px, py, plot.color);
            }
        }
    }
}

void Canvas::drawByPixels(const std::function<sf::Color(sf::Vector2<double>)> f)
{
    for (size_t py = 0; py < image.getSize().y; py++)
    {
        for (size_t px = 0; px < image.getSize().x; px++)
        {
            sf::Vector2<double> real_coords = toRealCoord({px, py});
            sf::Color color = f(real_coords);

            image.setPixel(px, py, color);
        }
    }
}

void Canvas::draw(sf::RenderTarget &window)
{
    fill();
    drawAxis();
    drawPlots();

    texture.loadFromImage(image);
    sprite.setTexture(texture);
    window.draw(sprite);
}