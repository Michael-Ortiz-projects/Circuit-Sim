#pragma once
#include "../Config.h"
#include "SFML/Graphics.hpp"
#include <iostream>
class TextBox {
public:
    TextBox(sf::Font& font, const sf::Vector2f& pos, const sf::Vector2f& boxSize, int textSize, const std::string& initialText = "", const bool outline = true);
    void handleEvent(const sf::Event& event);
    void draw(sf::RenderWindow& window);
    void setActive(bool active);
    bool isActive() const;
    void setFont(sf::Font& font);

    const std::string& getText() const;
    void setText(const std::string& str);
    bool contains(const sf::Vector2f& point) const;
    void setPosition(const sf::Vector2f& pos);
    void setSize(const sf::Vector2f& size);
    sf::RectangleShape getTextBoundsRect();
private:
    sf::RectangleShape box;
    sf::Text text;
    std::string value;
    bool active = false;
};
