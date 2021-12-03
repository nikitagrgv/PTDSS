#include <iostream>
#include <cstdint>
#include <string>
#include <complex>
#include <functional>

#include "SFML/Graphics.hpp"
#include "SFML/Graphics/Image.hpp"
#include "SFML/System/Clock.hpp"

void addAxis(sf::Image &image, sf::Vector2<double> center, double zoom)
{
    size_t px_zero = image.getSize().x / 2 - center.x * zoom;
    size_t py_zero = image.getSize().y / 2 + center.y * zoom;

    // ox
    if (py_zero < image.getSize().y)
    {
        for (size_t i = 0; i < image.getSize().x; i++)
        {
            image.setPixel(i, py_zero, sf::Color(255, 0, 0));
        }
    }

    // oy
    if (px_zero < image.getSize().x)
    {
        for (size_t i = 0; i < image.getSize().y; i++)
        {
            image.setPixel(px_zero, i, sf::Color(0, 255, 0));
        }
    }
}

void addYx(sf::Image &image, sf::Vector2<double> center, double zoom,
           const std::function<double(double)> f, sf::Color color)
{
    for (size_t px = 0; px < image.getSize().x; px++)
    {
        double x = center.x + (px - image.getSize().x / 2.0) / zoom;
        double y = f(x);
        size_t py = image.getSize().y / 2 + (center.y - y) * zoom;

        if (py < image.getSize().y)
        {
            image.setPixel(px, py, color);
        }
    }
}



void updateAllGraph(sf::Image &image, sf::Vector2<double> center, double zoom)
{
    for (size_t py = 0; py < image.getSize().y; py++)
    {
        double y = center.y - (py - image.getSize().y / 2.0) / zoom;
        for (size_t px = 0; px < image.getSize().x; px++)
        {
            double x = center.x + (px - image.getSize().x / 2.0) / zoom;

            // uint8_t xx = (uint8_t)(x * 100);
            // uint8_t yy = (uint8_t)(y * 100);
            // xx = xx & yy;


            // image.setPixel(px, py, sf::Color(
            //     (yy == 0 ? 0 : 255*xx / yy) / 4,
            //     0,
            //     0
            //     ));
            std::complex<double> c(x/6, y/6);
            std::complex<double> z(0, 0);
 
            double color = 0;
            for (size_t i = 0; i < 20; i++)
            {
                z = z * z + c;
 
                if (abs(z) > 4)
                {
                    color = i;
                    break;
                }
            }
            

            image.setPixel(px, py, sf::Color(
                std::min<double>(color*5, 255),
                0,
                0
                ));


        }

        if (py % (image.getSize().y / 100) == 0)
        {
            std::cout << 100 * py / image.getSize().y << std::endl;
        }
    }
}

int main()
{
    sf::Vector2<int> size(1600, 900);

    sf::RenderWindow window(sf::VideoMode(size.x, size.y), "Zachem ya eto sdelal", sf::Style::Fullscreen);

    sf::Image image;
    image.create(size.x, size.y);

    sf::Vector2<double> center(0, 0);
    double zoom = image.getSize().x / 2 / 12;

    // updateAllGraph(image, center, zoom);

    sf::Texture texture;
    texture.loadFromImage(image);
    sf::Sprite sprite(texture);

    sf::Font font;
    font.loadFromFile("lucon.ttf");
    sf::Text text_render;
    text_render.setFont(font);
    text_render.setCharacterSize(15);
    text_render.setFillColor(sf::Color(255, 255, 255, 200));

    sf::RectangleShape text_fone(sf::Vector2f(120., 24. * 5));
    text_fone.setFillColor(sf::Color(0, 0, 0, 200));

    sf::Clock prog_time;
    sf::Clock clock;
    bool freeze = false;
    double t = prog_time.getElapsedTime().asMilliseconds() / 1000.;
    while (window.isOpen())
    {
        if (!freeze)
        {
            t = prog_time.getElapsedTime().asMilliseconds() / 1000.;
        }
        double dt = clock.restart().asMilliseconds() / 1000.;

        sf::Vector2f mouse_pos;
        mouse_pos.x = sf::Mouse::getPosition(window).x * size.x / window.getSize().x;
        mouse_pos.y = sf::Mouse::getPosition(window).y * size.y / window.getSize().y;

        mouse_pos.x = center.x + (mouse_pos.x - image.getSize().x / 2.0) / zoom;
        mouse_pos.y = center.y - (mouse_pos.y - image.getSize().y / 2.0) / zoom;

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
                    prog_time.restart();
                }

                if (event.key.code == sf::Keyboard::F)
                {
                    freeze = !freeze;;
                }
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    center.x = mouse_pos.x;
                    center.y = mouse_pos.y;
                }

                if (event.mouseButton.button == sf::Mouse::Right)
                {
                    updateAllGraph(image, center, zoom);
                }
            }



        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            center.x -= 500 * dt / zoom;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            center.x += 500 * dt / zoom;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            center.y -= 500 * dt / zoom;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            center.y += 500 * dt / zoom;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            zoom *= 1 + dt * 1.4;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
        {
            zoom /= 1 + dt * 1.4;
        }

        image.create(image.getSize().x, image.getSize().y);
        // updateAllGraph(image, center, zoom);
        addAxis(image, center, zoom);


        // auto fx = [](double x, double t) -> double
        // {
        //     return sin(x + t);
        // };

        // addYxt(image, center, zoom, fx, sf::Color::Cyan, t);

        auto fx2 = [t](double x) -> double
        {
            return sin(1/sqrt(x)) *x * sin(t);
        };
        addYx(image, center, zoom, fx2, sf::Color::Magenta);


        texture.loadFromImage(image);
        sprite.setTexture(texture);

        std::string info;
        info = info + "x: " + std::to_string(center.x) + "\n";
        info = info + "y: " + std::to_string(center.y) + "\n";
        info = info + "zoom: " + std::to_string(zoom / image.getSize().x * 2) + "\n";
        info = info + "mx: " + std::to_string(mouse_pos.x) + "\n";
        info = info + "my: " + std::to_string(mouse_pos.y) + "\n";
        info = info + "FPS: " + std::to_string(1/dt) + "\n";
        text_render.setString(info);

        window.clear();
        window.draw(sprite);

        text_fone.setSize(sf::Vector2f(
            text_render.getLocalBounds().width + 10, text_render.getLocalBounds().height));
        window.draw(text_fone);
        window.draw(text_render);
        window.display();
    }

    return 0;
}
