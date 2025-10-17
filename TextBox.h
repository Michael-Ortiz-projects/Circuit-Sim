#pragma once
#include "global_variables.h"
#include "SFML/Graphics.hpp"


class TextBox {
public:
    sf::RectangleShape box;
    sf::Text text;
    bool selecting = false;
    sf::Color outline_color;

    TextBox();

    TextBox(const sf::Vector2f& position, const sf::Font& font);

    void setTextBoundingBox();

    void draw(sf::RenderWindow& window);
};