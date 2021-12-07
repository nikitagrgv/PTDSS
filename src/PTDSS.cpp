#include "PTDSS.hpp"
#include "Canvas.hpp"
PTDSS::PTDSS(sf::Vector2u window_size) : window(sf::VideoMode(window_size.x, window_size.y),
                                                "PTDSS",
                                                sf::Style::Fullscreen),
                                         canvas(window_size - sf::Vector2u(), this)
{
    // window.setVerticalSyncEnabled(true);
    canvas.setFillColor({18, 25, 32});
}

void PTDSS::processInput()
{
    auto &viewport = canvas.getViewport();

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
                entity_storage.plots.push_back({fx1, sf::Color::Cyan});
            }

            if (event.key.code == sf::Keyboard::Q)
            {
                if (entity_storage.plots.size() > 0)
                {
                    entity_storage.plots.pop_back();
                }
            }
        }

        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                viewport.center = canvas.getMousePosReal();
            }
        }
        if (event.type == sf::Event::MouseWheelScrolled)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
            {
                double resize_coef = 1 + 0.1 * abs(event.mouseWheelScroll.delta);
                if (event.mouseWheelScroll.delta > 0)
                {
                    viewport.size.y /= resize_coef;
                }
                else
                {
                    viewport.size.y *= resize_coef;
                }
            }
            else
            {
                double resize_coef = 1 + 0.1 * abs(event.mouseWheelScroll.delta);
                if (event.mouseWheelScroll.delta > 0)
                {
                    viewport.size /= resize_coef;
                }
                else
                {
                    viewport.size *= resize_coef;
                }
                // sf::Vector2<double> mouse_pos_real = canvas.toRealCoord(mouse_pos.image);
                viewport.center = canvas.getMousePosReal() * (resize_coef - 1) + resize_coef * viewport.center;
            }
        }
    }

    double camera_speed = 0.5;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        viewport.center.x -= camera_speed * dt * viewport.size.x;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        viewport.center.x += camera_speed * dt * viewport.size.x;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        viewport.center.y -= camera_speed * dt * viewport.size.y;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        viewport.center.y += camera_speed * dt * viewport.size.y;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
    {
        viewport.size.x *= 1 + dt * 1.4;
        viewport.size.y *= 1 + dt * 1.4;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
    {
        viewport.size.x /= 1 + dt * 1.4;
        viewport.size.y /= 1 + dt * 1.4;
    }
}

void PTDSS::draw()
{
    window.clear();

    canvas.draw(window);

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // window.display();
}

void PTDSS::run()
{
    auto fx1 = [](double x) -> double
    {
        return sin(1 / x) * sin(2 * 3.141516 * x) * (1 + sin(2 * 3.141516 * x));
    };
    entity_storage.plots.push_back({fx1, sf::Color::Magenta});

    sf::Font font;
    font.loadFromFile("lucon.ttf");
    sf::Text text_render;
    text_render.setFont(font);
    text_render.setCharacterSize(15);
    text_render.setFillColor(sf::Color(255, 255, 255, 200));

    sf::RectangleShape text_fone(sf::Vector2f(120., 24. * 5));
    text_fone.setFillColor(sf::Color(0, 0, 0, 200));

    sf::Time last_time = clock.restart();
    ;
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
        auto &viewport = canvas.getViewport();
        std::string info;
        info = info + "x: " + std::to_string(viewport.center.x) + "\n";
        info = info + "y: " + std::to_string(viewport.center.y) + "\n";
        info = info + "size x: " + std::to_string(viewport.size.x) + "\n";
        info = info + "size y: " + std::to_string(viewport.size.y) + "\n";
        info = info + "mx: " + std::to_string(canvas.getMousePosReal().x) + "\n";
        info = info + "my: " + std::to_string(canvas.getMousePosReal().y) + "\n";
        info = info + "FPS: " + info_fps + "\n";
        info = info + "Plots: " + std::to_string(entity_storage.plots.size()) + "\n";

        text_render.setString(info);
        text_fone.setSize({text_render.getLocalBounds().width + 10, text_render.getLocalBounds().height});
        window.draw(text_fone);
        window.draw(text_render);
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        window.display();
    }
}
