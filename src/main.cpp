#include <iostream>
#include <cstdint>
#include <string>
#include <complex>
#include <functional>

#include "SFML/Graphics.hpp"
#include "SFML/Graphics/Image.hpp"
#include "SFML/System/Clock.hpp"

#include "Canvas.hpp"

const sf::Vector2u WINDOW_SIZE = {1600, 900};

class PTDSS
{
private:
    sf::Clock clock;
    sf::Time last_time;
    struct
    {
        sf::Vector2i image;
        sf::Vector2<double> real;
    } mouse_pos;

public:
    sf::RenderWindow window;
    Canvas canvas;
    double dt;

public:
    PTDSS() : window(sf::VideoMode(WINDOW_SIZE.x, WINDOW_SIZE.y), "PTDSS", sf::Style::Fullscreen),
              canvas(WINDOW_SIZE)
    {
        // window.setVerticalSyncEnabled(true);
        canvas.fill_color = sf::Color(18, 25, 32);
    }

    double getTime() const
    {
        return clock.getElapsedTime().asMicroseconds() / 1e6;
    }

    sf::Vector2i getMousePosImage()
    {
        sf::Vector2i pos;
        pos.x = sf::Mouse::getPosition(window).x * (int)canvas.getImageSize().x / (int)window.getSize().x;
        pos.y = sf::Mouse::getPosition(window).y * (int)canvas.getImageSize().y / (int)window.getSize().y;
        return pos;
    }

    void processInput()
    {
        mouse_pos.image = getMousePosImage();
        mouse_pos.real = canvas.toRealCoord(mouse_pos.image);

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
                    canvas.restartViewport();
                }

                if (event.key.code == sf::Keyboard::E)
                {
                    auto fx1 = [this](double x) -> double
                    {
                        double t = dt;
                        return sin(x + t);
                    };
                    canvas.plots.push_back({fx1, sf::Color::Cyan});
                }

                if (event.key.code == sf::Keyboard::Q)
                {
                    if (canvas.plots.size() > 0)
                    {
                        canvas.plots.pop_back();
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    canvas.viewport.center.x = mouse_pos.real.x;
                    canvas.viewport.center.y = mouse_pos.real.y;
                }
            }
            if (event.type == sf::Event::MouseWheelScrolled)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                {
                    double resize_coef = 1 - dt * 10 * event.mouseWheelScroll.delta;
                    canvas.viewport.size.y *= resize_coef;
                    mouse_pos.real = canvas.toRealCoord(mouse_pos.image);
                }
                else
                {
                    double resize_coef = 1 - dt * 10 * event.mouseWheelScroll.delta;
                    canvas.viewport.size *= resize_coef;
                    mouse_pos.real = canvas.toRealCoord(mouse_pos.image);
                    canvas.viewport.center = mouse_pos.real * (1 - resize_coef) + resize_coef * canvas.viewport.center;
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
            canvas.viewport.center.y -= camera_speed * dt * canvas.viewport.size.y;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            canvas.viewport.center.y += camera_speed * dt * canvas.viewport.size.y;
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
            return (10 * sin(100 * 2 * 3.141516 * x) * sin(5 * 2 * 3.141516 * x) * (1 + sin(5 * 2 * 3.141516 * x)));
            // return sin(x);
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

            static sf::Clock fps_clock;
            static double fps = 1e6 / clock.getElapsedTime().asMicroseconds();
            static std::string info_fps;
            if (fps_clock.getElapsedTime().asMicroseconds() >= 1e6 / 5)
            {
                fps = 1 / dt;
                fps_clock.restart();

                info_fps = std::to_string(fps);
                info_fps.resize(5, ' ');
            }

            std::string info;
            info = info + "x: " + std::to_string(canvas.viewport.center.x) + "\n";
            info = info + "y: " + std::to_string(canvas.viewport.center.y) + "\n";
            info = info + "size x: " + std::to_string(canvas.viewport.size.x) + "\n";
            info = info + "size y: " + std::to_string(canvas.viewport.size.y) + "\n";
            info = info + "mx: " + std::to_string(mouse_pos.real.x) + "\n";
            info = info + "my: " + std::to_string(mouse_pos.real.y) + "\n";
            info = info + "FPS: " + info_fps + "\n";
            info = info + "Plots: " + std::to_string(canvas.plots.size()) + "\n";

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
    PTDSS metagrapher;
    metagrapher.run();
    return 0;
}
