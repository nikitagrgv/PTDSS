#include <iostream>
#include <cstdint>
#include <string>
#include <complex>
#include <functional>

#include "SFML/Graphics.hpp"
#include "SFML/Graphics/Image.hpp"
#include "SFML/System/Clock.hpp"

#include "Canvas.hpp"

int main()
{
    sf::Vector2<size_t> window_size(800, 600);

    sf::RenderWindow window(sf::VideoMode(window_size.x, window_size.y), "Zachem ya eto sdelal");
    window.setVerticalSyncEnabled(true);

    Canvas canvas(window.getSize());
    canvas.fill_color = sf::Color(18, 25, 32);

    sf::Font font;
    font.loadFromFile("lucon.ttf");
    sf::Text text_render;
    text_render.setFont(font);
    text_render.setCharacterSize(15);
    text_render.setFillColor(sf::Color(255, 255, 255, 200));

    sf::RectangleShape text_fone(sf::Vector2f(120., 24. * 5));
    text_fone.setFillColor(sf::Color(0, 0, 0, 200));

    auto fx1 = [](double x) -> double
    {
        return sin(x);
    };
    canvas.plots.push_back({fx1, sf::Color::Magenta});

    sf::Clock time;
    sf::Clock clock;
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
                if (event.mouseButton.button == sf::Mouse::Right)
                {
                    double amp = rand() % 100000 / 10000.;
                    double phase = rand() % 31415 / 31415.;
                    double freq = 1 + rand() % 50000 / 5000.;
                    double time_1 = rand() % 100000 / 10000.;
                    double time_2 = rand() % 100000 / 10000.;
                    double time_3 = rand() % 100000 / 10000.;

                    auto fx1 = [amp, phase, freq, time_1, time_2, time_3, time](double x) -> double
                    {
                        double t = time.getElapsedTime().asMilliseconds() / 1000.;
                        return sin(time_1 * t) * amp * sin(freq * x / sin(time_3 * t) + phase * sin(time_2 * t));
                    };
                    canvas.plots.push_back({fx1, sf::Color(rand()%255, rand()%255,rand()%255)});
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
