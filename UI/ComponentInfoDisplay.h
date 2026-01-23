#pragma once
#include "TextBox.h"
#include <SFML/Graphics.hpp>
#include <string>

class ComponentInfoDisplay {
public:
    ComponentInfoDisplay(sf::Font& font, const sf::Vector2f& pos, const sf::Vector2f& size, bool active);

    void setLabel(const std::string& label);
    void setValue(const std::string& value);
    void setPosition(const sf::Vector2f& pos);
    void draw(sf::RenderWindow& window);

private:
    TextBox labelBox;
    TextBox valueBox;
    sf::Vector2f boxSize;
};
