#include "TextBox.h"

TextBox::TextBox() {}

TextBox::TextBox(const sf::Vector2f& position, const sf::Font& font) {
    text = sf::Text("", font, 25);
    text.setPosition(position);
    text.setScale(1, -1);
    text.setFillColor(sf::Color(220, 220, 240));
    outline_color = sf::Color(90, 90, 105);
}

void TextBox::setTextBoundingBox() {
    sf::FloatRect local = text.getLocalBounds();  // includes glyph offsets like left/top
    

    // Correctly position using local offset in world space
    box.setSize(sf::Vector2f(local.width + 10, local.height + 10));
    box.setPosition(text.getTransform().transformPoint(local.left - 5, local.top - 5));
    box.setScale(text.getScale());
    box.setFillColor(sf::Color(45, 45, 55));
    box.setOutlineColor(outline_color);/*sf::Color(149, 76, 46)*/
    box.setOutlineThickness(2);
}

void TextBox::draw(sf::RenderWindow& window) {
    window.draw(box);
    window.draw(text);
}