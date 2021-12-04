#include <iostream>
#include <cstdint>
#include <string>
#include <complex>
#include <functional>

#include "SFML/Graphics.hpp"
#include "SFML/Graphics/Image.hpp"
#include "SFML/System/Clock.hpp"

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

    Canvas(sf::Vector2<size_t> size)
    {
        createImage(size);
        restartViewport();
    }

    sf::Vector2<size_t> toImageCoord(const sf::Vector2<double> &real_coord) const
    {
        sf::Vector2<size_t> image_coord;
        image_coord.x = round(image.getSize().x * (+real_coord.x - viewport.center.x + 0.5 * viewport.size.x) / viewport.size.x);
        image_coord.y = round(image.getSize().y * (-real_coord.y + viewport.center.y + 0.5 * viewport.size.y) / viewport.size.y);
        return image_coord;
    }

    sf::Vector2<double> toRealCoord(const sf::Vector2<size_t> &image_coord) const
    {
        sf::Vector2<double> real_coord;
        real_coord.x = viewport.center.x + ((double)image_coord.x / image.getSize().x - 0.5) * viewport.size.x;
        real_coord.y = viewport.center.y - ((double)image_coord.y / image.getSize().y - 0.5) * viewport.size.y;
        return real_coord;
    }

    void draw(sf::RenderTarget &window)
    {
        texture.loadFromImage(image);
        // ????????????????????
        sprite.setTexture(texture);
        window.draw(sprite);
    }

    void createImage(sf::Vector2<size_t> &size)
    {
        image.create(size.x, size.y);
    }

    void fill(sf::Color color = sf::Color::Black)
    {
        for (size_t py = 0; py < image.getSize().y; py++)
        {
            for (size_t px = 0; px < image.getSize().x; px++)
            {
                image.setPixel(px, py, color);
            }
            
        }
        
    }

    void restartViewport()
    {
        viewport.center = {0, 0};
        viewport.size = {10., 10. * image.getSize().y / image.getSize().x};
    }

    void drawAxis()
    {
        size_t px = toImageCoord({0, 0}).x;
        size_t py = toImageCoord({0, 0}).y;

        // ox
        if (py >= 0 && py < image.getSize().y)
        {
            for (size_t i = 0; i < image.getSize().x; i++)
            {
                image.setPixel(i, py, sf::Color::Red);
            }
        }

        // oy
        if (px >= 0 && px < image.getSize().x)
        {
            for (size_t i = 0; i < image.getSize().y; i++)
            {
                image.setPixel(px, i, sf::Color::Green);
            }
        }
    }

    void drawPlot(const std::function<double(double)> f, sf::Color color)
    {
        for (size_t px = 0; px < image.getSize().x; px++)
        {
            double x = toRealCoord({px, 0}).x;
            double y = f(x);

            size_t py = toImageCoord({x, y}).y;
            if (py >= 0 && py < image.getSize().y)
            {
                image.setPixel(px, py, color);
            }
        }
    }

    void drawByPixels(const std::function<sf::Color(sf::Vector2<double>)> f)
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
};

int main()
{
    sf::Vector2<size_t> window_size(1600, 900);

    sf::RenderWindow window(sf::VideoMode(window_size.x, window_size.y), "Zachem ya eto sdelal", sf::Style::Fullscreen);
    window.setVerticalSyncEnabled(true);

    Canvas canvas(window.getSize());

    sf::Font font;
    font.loadFromFile("lucon.ttf");
    sf::Text text_render;
    text_render.setFont(font);
    text_render.setCharacterSize(15);
    text_render.setFillColor(sf::Color(255, 255, 255, 200));

    sf::RectangleShape text_fone(sf::Vector2f(120., 24. * 5));
    text_fone.setFillColor(sf::Color(0, 0, 0, 200));

    sf::Clock time;
    sf::Clock clock;
    double t = time.getElapsedTime().asMilliseconds() / 1000.;
    while (window.isOpen())
    {
        double dt = clock.restart().asMilliseconds() / 1000.;

        sf::Vector2<size_t> mouse_pos_image;
        sf::Vector2<double> mouse_pos_real;
        mouse_pos_image.x = sf::Mouse::getPosition(window).x * window_size.x / window.getSize().x;
        mouse_pos_image.y = sf::Mouse::getPosition(window).y * window_size.y / window.getSize().y;

        mouse_pos_real.x = canvas.toRealCoord(mouse_pos_image).x;
        mouse_pos_real.y = canvas.toRealCoord(mouse_pos_image).y;

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                {
                    window.close();
                    continue;
                }

                if (event.key.code == sf::Keyboard::F5)
                {
                    time.restart();
                }
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    canvas.viewport.center.x = mouse_pos_real.x;
                    canvas.viewport.center.y = mouse_pos_real.y;
                }
            }
        }

        double camera_speed = 0.5;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            canvas.viewport.center.x -= camera_speed * dt * canvas.viewport.size.x;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            canvas.viewport.center.x += camera_speed * dt * canvas.viewport.size.x;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            canvas.viewport.center.y -= camera_speed * dt * canvas.viewport.size.x;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            canvas.viewport.center.y += camera_speed * dt * canvas.viewport.size.x;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
        {
            canvas.viewport.size.x *= 1 + dt * 1.4;
            canvas.viewport.size.y *= 1 + dt * 1.4;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            canvas.viewport.size.x /= 1 + dt * 1.4;
            canvas.viewport.size.y /= 1 + dt * 1.4;
        }

        canvas.fill(sf::Color(18, 25, 32));
        canvas.drawAxis();

        auto fx1 = [](double x) -> double
        {
            return sin(x);
        };
        canvas.drawPlot(fx1, sf::Color::Magenta);

        std::string info;
        info = info + "x: " + std::to_string(canvas.viewport.center.x) + "\n";
        info = info + "y: " + std::to_string(canvas.viewport.center.y) + "\n";
        info = info + "size x: " + std::to_string(canvas.viewport.size.x) + "\n";
        info = info + "size y: " + std::to_string(canvas.viewport.size.y) + "\n";
        info = info + "mx: " + std::to_string(mouse_pos_real.x) + "\n";
        info = info + "my: " + std::to_string(mouse_pos_real.y) + "\n";
        info = info + "FPS: " + std::to_string(1 / dt) + "\n";
        text_render.setString(info);
        text_fone.setSize({text_render.getLocalBounds().width + 10, text_render.getLocalBounds().height});

        window.clear();

        canvas.draw(window);
        window.draw(text_fone);
        window.draw(text_render);
        window.display();
    }

    return 0;
}
