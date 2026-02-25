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
    sf::RectangleShape getBox() { return box; }

private:
    sf::RectangleShape box;
    sf::Text text;
    std::string value;
    bool active = false;
};

class ScrollTextBox : public sf::Drawable {
public:
    ScrollTextBox();
    ScrollTextBox(const sf::Font& font, unsigned int charSize, sf::Vector2f position, sf::Vector2f size);

    void setString(const std::string& str);
    void appendString(const std::string str);
    void setPosition(sf::Vector2f pos);
    void setSize(sf::Vector2f size);
    void setFont(sf::Font& font);
    void setCharSize(unsigned int size);
    void clear() { text.setString(""); }

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void updateView();
    float getContentHeight() const;

private:
    sf::RectangleShape background;
    sf::Text text;

    sf::View view;          // View used to clip content
    sf::FloatRect viewRect; // In world coords

    float scrollOffset = 0.f;
    float scrollSpeed = 30.f;
};