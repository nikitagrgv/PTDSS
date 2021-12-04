#include <iostream>
#include <cstdint>
#include <string>
#include <complex>
#include <functional>

#include "SFML/Graphics.hpp"
#include "SFML/Graphics/Image.hpp"
#include "SFML/System/Clock.hpp"



class Viewport
{
public:
    sf::Vector2<double> center;
    sf::Vector2<double> size;
};

sf::Vector2<size_t> toImageCoord(sf::Vector2<double> real_coord, sf::Image &image, Viewport &viewport)
{
    sf::Vector2<size_t> image_coord;
    image_coord.x = round(image.getSize().x * (+real_coord.x - viewport.center.x + 0.5 * viewport.size.x) / viewport.size.x);
    image_coord.y = round(image.getSize().y * (-real_coord.y + viewport.center.y + 0.5 * viewport.size.y) / viewport.size.y);
    return image_coord;
}


sf::Vector2<double> toRealCoord(sf::Vector2<size_t> image_coord, sf::Image &image, Viewport &viewport)
{
    sf::Vector2<double> real_coord;
    real_coord.x = viewport.center.x + ((double)image_coord.x / image.getSize().x - 0.5) * viewport.size.x;
    real_coord.y = viewport.center.y - ((double)image_coord.y / image.getSize().y - 0.5) * viewport.size.y;
    return real_coord;
}




void addAxis(sf::Image &image, Viewport viewport)
{
    sf::Vector2<double> coord_center(0, 0);
    size_t px_zero = toImageCoord(coord_center, image, viewport).x;
    size_t py_zero = toImageCoord(coord_center, image, viewport).y;

    // ox
    if (py_zero < image.getSize().y)
    {
        for (size_t i = 0; i < image.getSize().x; i++)
        {
            image.setPixel(i, py_zero, sf::Color(255, 0, 0)); // red
        }
    }

    // oy
    if (px_zero < image.getSize().x)
    {
        for (size_t i = 0; i < image.getSize().y; i++)
        {
            image.setPixel(px_zero, i, sf::Color(0, 255, 0)); // green
        }
    }
}


void drawPlot(sf::Image &image, Viewport viewport,
           const std::function<double(double)> f, sf::Color color)
{
    for (size_t px = 0; px < image.getSize().x; px++)
    {
        double x = toRealCoord(sf::Vector2<size_t>(px, 0), image, viewport).x;
        double y = f(x);
        size_t py = toImageCoord(sf::Vector2<double>(x, y), image, viewport).y;

        if (py < image.getSize().y)
        {
            image.setPixel(px, py, color);
        }
    }
}



void updateAllGraph(sf::Image &image, Viewport viewport)
{
    for (size_t py = 0; py < image.getSize().y; py++)
    {
        for (size_t px = 0; px < image.getSize().x; px++)
        {
            sf::Vector2<size_t> image_coords(px, py);
            sf::Vector2<double> real_coords = toRealCoord(image_coords, image, viewport); 
            double x = real_coords.x;
            double y = real_coords.y;


            std::complex<double> c(x/6, y/6);
            std::complex<double> z(0, 0);
 
            double color = 0;
            for (size_t i = 0; i < 10; i++)
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
    sf::Vector2<int> window_size(800, 600);

    sf::RenderWindow window(sf::VideoMode(window_size.x, window_size.y), "Zachem ya eto sdelal");

    sf::Image image;
    image.create(window_size.x, window_size.y);

    Viewport viewport;
    viewport.center = sf::Vector2<double>(0, 0);
    viewport.size = sf::Vector2<double>(1., 1. * image.getSize().y / image.getSize().x);


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

        sf::Vector2<size_t> mouse_pos_image;
        sf::Vector2<double> mouse_pos_real;
        mouse_pos_image.x = sf::Mouse::getPosition(window).x * window_size.x / window.getSize().x;
        mouse_pos_image.y = sf::Mouse::getPosition(window).y * window_size.y / window.getSize().y;

        mouse_pos_real.x = toRealCoord(mouse_pos_image, image, viewport).x;
        mouse_pos_real.y = toRealCoord(mouse_pos_image, image, viewport).y;

        //ffffffffffffff
        mouse_pos_image = toImageCoord(mouse_pos_real, image, viewport) + sf::Vector2<size_t>(100000,100000);

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
                    viewport.center.x = mouse_pos_real.x;
                    viewport.center.y = mouse_pos_real.y;
                }

                if (event.mouseButton.button == sf::Mouse::Right)
                {
                    updateAllGraph(image, viewport);
                }
            }



        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            viewport.center.x -= 1. * dt * viewport.size.x;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            viewport.center.x += 1. * dt * viewport.size.x;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            viewport.center.y -= 1. * dt * viewport.size.y;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            viewport.center.y += 1. * dt * viewport.size.y;
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

        image.create(image.getSize().x, image.getSize().y);
        // updateAllGraph(image, center, zoom);
        addAxis(image, viewport);


        auto fx1 = [](double x) -> double
        {
            return sin(x);
        };
        drawPlot(image, viewport, fx1, sf::Color::Magenta);

        texture.loadFromImage(image);
        sprite.setTexture(texture);

        std::string info;
        info = info + "x: " + std::to_string(viewport.center.x) + "\n";
        info = info + "y: " + std::to_string(viewport.center.y) + "\n";
        info = info + "size x: " + std::to_string(viewport.size.x) + "\n";
        info = info + "size y: " + std::to_string(viewport.size.y) + "\n";
        info = info + "mx_i: " + std::to_string(mouse_pos_image.x) + "\n";
        info = info + "my_i: " + std::to_string(mouse_pos_image.y) + "\n";
        info = info + "mx_r: " + std::to_string(mouse_pos_real.x) + "\n";
        info = info + "my_r: " + std::to_string(mouse_pos_real.y) + "\n";
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
