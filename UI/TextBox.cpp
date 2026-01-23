#include "TextBox.h"

TextBox::TextBox(sf::Font& font, const sf::Vector2f& pos, const sf::Vector2f& size, const std::string& initialText)
    : value(initialText) {
    box.setPosition(pos);
    box.setSize(size);
    box.setFillColor(sf::Color(40, 40, 50));
    box.setOutlineThickness(1.f);
    box.setOutlineColor(sf::Color(120, 120, 120));

    text.setFont(font);
    text.setCharacterSize(16);
    text.setFillColor(sf::Color::White);
    text.setPosition(pos.x + 5.f, pos.y + 5.f);
    text.setString(value);
}

void TextBox::setActive(bool a) {
    active = a;
    box.setOutlineColor(active ? sf::Color::Yellow
        : sf::Color(120, 120, 120));
}

bool TextBox::isActive() const {
    return active;
}

void TextBox::setFont(sf::Font& font) {
    text.setFont(font);
}

void TextBox::handleEvent(const sf::Event& event) {
    if (!active) return;

    if (event.type == sf::Event::TextEntered) {
        uint32_t c = event.text.unicode;

        if (c == 8) { // backspace
            if (!value.empty())
                value.pop_back();
        }
        else if (c >= 32 && c < 127) {
            value += static_cast<char>(c);
        }

        text.setString(value);
    }
}

void TextBox::draw(sf::RenderWindow& window) {
    window.draw(box);
    window.draw(text);
}

const std::string& TextBox::getText() const {
    return value;
}

void TextBox::setText(const std::string& str) {
    value = str;
    text.setString(value);
}

bool TextBox::contains(const sf::Vector2f& point) const {
    return box.getGlobalBounds().contains(point);
}

void TextBox::setPosition(const sf::Vector2f& pos) {
    box.setPosition(pos);
    text.setPosition(pos.x + 5.f, pos.y + 5.f);
}

void TextBox::setSize(const sf::Vector2f& size) {
    box.setSize(size);
}