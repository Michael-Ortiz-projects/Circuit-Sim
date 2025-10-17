#pragma once
#include <functional>
#include "SFML/Graphics.hpp"
#include "TextBox.h"


class Button {
private:

    std::function<void()> click_function;

    sf::Color hoverColor = sf::Color(50, 50, 65);
    sf::Color normalColor = sf::Color(45, 45, 55);
    bool isHovered = false;

public:
    TextBox textBox;
    Button(const sf::Vector2f& position, const sf::Font& font, const std::string& text, std::function<void()> on_click_function);

    void draw(sf::RenderWindow& window);

    bool handleEvent(const sf::Event& event, const sf::RenderWindow& window);
};