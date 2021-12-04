#include <iostream>
#include <cstdint>
#include <string>
#include <complex>
#include <functional>

#include "SFML/Graphics.hpp"
#include "SFML/Graphics/Image.hpp"
#include "SFML/System/Clock.hpp"

#include "Canvas.hpp"

const sf::Vector2u WINDOW_SIZE = {800, 600};

class Metagrapher
{
private:
    sf::Clock clock;
    sf::Time last_time;
    sf::Vector2<size_t> mouse_pos_image;
    sf::Vector2<double> mouse_pos_real;

public:
    sf::RenderWindow window;
    Canvas canvas;
    double dt;

public:
    Metagrapher() : window(sf::VideoMode(WINDOW_SIZE.x, WINDOW_SIZE.y), "Metagrapher"),
                    canvas(WINDOW_SIZE)
    {
        window.setVerticalSyncEnabled(true);
        canvas.fill_color = sf::Color(18, 25, 32);
    }

    double getTime() const
    {
        return clock.getElapsedTime().asMicroseconds() / 1e6;
    }

    void processInput()
    {
        mouse_pos_image.x = sf::Mouse::getPosition(window).x * window.getSize().x / window.getSize().x;
        mouse_pos_image.y = sf::Mouse::getPosition(window).y * window.getSize().y / window.getSize().y;

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
                    clock.restart();
                }
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    canvas.viewport.center.x = mouse_pos_real.x;
                    canvas.viewport.center.y = mouse_pos_real.y;
                }
                if (event.mouseButton.button == sf::Mouse::Right)
                {
                    auto fx1 = [this](double x) -> double
                    {
                        double t = dt;
                        return sin(x + t);
                    };
                    canvas.plots.push_back({fx1, sf::Color::Cyan});
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
    }

    void draw()
    {
        window.clear();

        canvas.draw(window);

        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // window.display();
    }

    void run()
    {
        auto fx1 = [](double x) -> double
        {
            return sin(x);
        };
        canvas.plots.push_back({fx1, sf::Color::Magenta});

        sf::Font font;
        font.loadFromFile("lucon.ttf");
        sf::Text text_render;
        text_render.setFont(font);
        text_render.setCharacterSize(15);
        text_render.setFillColor(sf::Color(255, 255, 255, 200));

        sf::RectangleShape text_fone(sf::Vector2f(120., 24. * 5));
        text_fone.setFillColor(sf::Color(0, 0, 0, 200));

        clock.restart();
        while (window.isOpen())
        {
            dt = (clock.getElapsedTime() - last_time).asMicroseconds() / 1e6;
            last_time = clock.getElapsedTime();
            processInput();
            draw();

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
            window.draw(text_fone);
            window.draw(text_render);
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            window.display();
        }
    }
};

int main()
{
    Metagrapher metagrapher;
    metagrapher.run();
    return 0;
}
