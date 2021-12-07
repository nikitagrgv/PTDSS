#include "Canvas.hpp"
#include "PTDSS.hpp"

Canvas::Canvas(sf::Vector2u size, PTDSS *ptdss) : ptdss(ptdss)
{
    createImage(size);
    restartViewport();
}

sf::Vector2i Canvas::toImageCoord(const sf::Vector2<double> real_coord) const
{
    sf::Vector2i image_coord;
    image_coord.x = round(image.getSize().x * (+real_coord.x - viewport.center.x + 0.5 * viewport.size.x) / viewport.size.x);
    image_coord.y = round(image.getSize().y * (-real_coord.y + viewport.center.y + 0.5 * viewport.size.y) / viewport.size.y);
    return image_coord;
}

sf::Vector2<double> Canvas::toRealCoord(const sf::Vector2i image_coord) const
{
    sf::Vector2<double> real_coord;
    real_coord.x = viewport.center.x + ((double)image_coord.x / image.getSize().x - 0.5) * viewport.size.x;
    real_coord.y = viewport.center.y - ((double)image_coord.y / image.getSize().y - 0.5) * viewport.size.y;
    return real_coord;
}

sf::Vector2i Canvas::getMousePosImage() const
{
    sf::Vector2i mouse_pos_window = ptdss->getMousePosWindow();
    sf::Vector2i mouse_pos_image;
    mouse_pos_image.x = mouse_pos_window.x * (int)getImageSize().x / (int)ptdss->getWindow().getSize().x;
    mouse_pos_image.y = mouse_pos_window.y * (int)getImageSize().y / (int)ptdss->getWindow().getSize().y;
    return mouse_pos_image;
}

sf::Vector2<double> Canvas::getMousePosReal() const
{
    return toRealCoord(getMousePosImage());
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

void Canvas::drawAxes()
{
    sf::Vector2i center_pos_image = toImageCoord({0, 0});

    sf::Color colorx = sf::Color::Red;
    sf::Color colory = sf::Color::Green;

    if (center_pos_image.y < 0)
    {
        center_pos_image.y = 6;
        colorx.r /= 3;
    }
    else if (center_pos_image.y >= (int)image.getSize().y)
    {
        center_pos_image.y = image.getSize().y - 7;
        colorx.r /= 3;
    }

    if (center_pos_image.x < 0)
    {
        center_pos_image.x = 6;
        colory.g /= 3;
    }
    else if (center_pos_image.x >= (int)image.getSize().x)
    {
        center_pos_image.x = image.getSize().x - 7;
        colory.g /= 3;
    }

    for (size_t i = 0; i < image.getSize().x; i++)
    {
        image.setPixel(i, center_pos_image.y, colorx);
    }

    for (size_t i = 0; i < image.getSize().y; i++)
    {
        image.setPixel(center_pos_image.x, i, colory);
    }
}

void Canvas::drawPlots()
{
    auto &plots = ptdss->getEntityStorage().plots;
    for (size_t px = 0; px < image.getSize().x; px++)
    {
        for (auto plot : plots)
        {
            double x = toRealCoord({(int)px, 0}).x;
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
            sf::Vector2<double> real_coords = toRealCoord({(int)px, (int)py});
            sf::Color color = f(real_coords);

            image.setPixel(px, py, color);
        }
    }
}

void Canvas::draw(sf::RenderTarget &window)
{
    fill();
    drawAxes();
    drawPlots();
    drawMousePos(); //DEBUG

    texture.loadFromImage(image);
    sprite.setTexture(texture);
    window.draw(sprite);
}


//DEBUG
void Canvas::drawMousePos()
{
    sf::Vector2i mouse_pos_image = getMousePosImage();

    size_t left_border = std::min<int>(std::max<int>(mouse_pos_image.x - 5, 0), image.getSize().x); 
    size_t right_border = std::min<int>(std::max<int>(mouse_pos_image.x + 5, 0), image.getSize().x); 

    size_t up_border = std::min<int>(std::max<int>(mouse_pos_image.y - 5, 0), image.getSize().y); 
    size_t down_border = std::min<int>(std::max<int>(mouse_pos_image.y + 5, 0), image.getSize().y); 

    for (size_t i = left_border; i < right_border; i++)
    {
        for (size_t j = up_border; j < down_border; j++)
        {
            image.setPixel(i, j, {100, 100, 100});
        }
        
    }
}