/*
** EPITECH PROJECT, 2024
** B-OOP-400-TLS-4-1-raytracer-leo.wehrle [WSL: Ubuntu]
** File description:
** Main
*/

#include <SFML/Config.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Window.hpp>
#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>

#include "Camera.hpp"
#include "Main.hpp"
#include "Math/Rectangle3D.hpp"
#include "Parameters.hpp"
#include "Sphere.hpp"

#include <SFML/Graphics.hpp>
#include <ostream>
#include <string>

auto Main::arg_parse() -> bool
{
    try {
        _params.load(_ac, _av);
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "Usage: " << _av[0] << " [scene file]" << std::endl;
        return false;
    }
    return true;
}

auto render_frame(sf::Uint8 *pixels, uint32_t image_width, uint32_t image_height, Camera &cam, Sphere &sphere)
    -> void
{
    for (uint32_t j = 0; j < image_height; ++j) {
        for (uint32_t i = 0; i < image_width; ++i) {
            double u = double(i) / (image_width - 1);
            double v = double(j) / (image_height - 1);
            Vector3D color(0.0, 0.0, 0.0);

            if (sphere.hits(cam.ray(u, v))) {
                color = Vector3D(200, 0, 0);
            } else {
                color = Vector3D(29, 32, 39);
            }
            pixels[(j * image_width + i) * 4 + 0] = static_cast<sf::Uint8>(color._x);
            pixels[(j * image_width + i) * 4 + 1] = static_cast<sf::Uint8>(color._y);
            pixels[(j * image_width + i) * 4 + 2] = static_cast<sf::Uint8>(color._z);
            pixels[(j * image_width + i) * 4 + 3] = 255;
        }
    }
}

auto handle_events(sf::RenderWindow &window, Camera &cam) -> void
{
    sf::Event event;
    constexpr auto movespeed = 0.01;
    while (window.pollEvent(event)) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
        switch (event.type) {

        case sf::Event::Closed:
            window.close();
            break;
        case sf::Event::KeyPressed:
            switch (event.key.code) {
            case sf::Keyboard::Escape:
                window.close();
                break;
            case sf::Keyboard::Key::Down:
            case sf::Keyboard::Key::S:
                cam.move(Vector3D(0, 0, movespeed));
                break;
            case sf::Keyboard::Key::Up:
            case sf::Keyboard::Key::Z:
                cam.move(Vector3D(0, 0, -movespeed));
                break;
            case sf::Keyboard::Key::Right:
            case sf::Keyboard::Key::D:
                cam.move(Vector3D(-movespeed, 0, 0));
                break;
            case sf::Keyboard::Key::Left:
            case sf::Keyboard::Key::Q:
                cam.move(Vector3D(movespeed, 0, 0));
                break;
            case sf::Keyboard::Key::Space:
                cam.move(Vector3D(0, movespeed, 0));
                break;
            case sf::Keyboard::Key::LShift:
                cam.move(Vector3D(0, -movespeed, 0));
                break;
            default:
                break;
            }
        default:
            break;
        }
#pragma GCC diagnostic pop
    }
}

auto render_real_time(uint32_t image_width, uint32_t image_height, Camera &cam, Sphere &sphere) -> void
{

    sf::RenderWindow window(sf::VideoMode(image_width, image_height), "Raytracer");
    sf::Uint8 *pixels = new sf::Uint8[image_width * image_height * 4];
    // framebuffer sprite
    sf::Texture texture;
    texture.create(image_width, image_height);
    sf::Sprite sprite(texture);

    while (window.isOpen()) {
        handle_events(window, cam);
        render_frame(pixels, image_width, image_height, cam, sphere);
        window.clear();
        texture.update(pixels);
        window.draw(sprite);
        window.display();
    }
    delete[] pixels;
}


void write_color(std::ofstream &out, const Vector3D &color)
{
    out << static_cast<int>(color._x) << ' ' << static_cast<int>(color._y) << ' '
        << static_cast<int>(color._z) << '\n';
}

auto Main::render_image(uint32_t image_width, uint32_t image_height, Camera &cam, Sphere &sphere) -> void
{
    std::string file_name = _params._output_file;
    if (file_name.empty()) {
        file_name = "output.ppm";
    }
    std::ofstream out(file_name);
    out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (uint32_t j = 0; j < image_height; ++j) {
        for (uint32_t i = 0; i < image_width; ++i) {
            double u = double(i) / (image_width - 1);
            double v = double(j) / (image_height - 1);
            Vector3D color(0.0, 0.0, 0.0);

            if (sphere.hits(cam.ray(u, v))) {
                color = Vector3D(200, 0, 0);
            } else {
                color = Vector3D(29, 32, 39);
            }
            write_color(out, color);
        }
    }
    std::cout << "Image saved to " << file_name << std::endl;
    out.close();
}

auto Main::run() -> int
{
    const uint32_t image_width = 400;
    const uint32_t image_height = 400;
    Sphere sphere(Point3D(0, -0.5, -1), 0.2);
    Camera cam;

    if (_params._gui) {
        render_real_time(image_width, image_height, cam, sphere);
    }
    if (!_params._scene_file.empty()) {
        render_image(image_width, image_height, cam, sphere);
    }
    return 0;
}

auto main(int ac, char *argv[]) -> int
{
    Main main(ac, argv);
    int exitCode = 0;

    try {
        if (!main.arg_parse()) {
            return 84;
        }
        exitCode = main.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        exitCode = 84;
    }
    return exitCode;
}
