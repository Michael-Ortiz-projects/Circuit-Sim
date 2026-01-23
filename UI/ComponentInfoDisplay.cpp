#include "ComponentInfoDisplay.h"

ComponentInfoDisplay::ComponentInfoDisplay(sf::Font& font, const sf::Vector2f& pos, const sf::Vector2f& size, bool active)
    : labelBox(font, pos, size, "Label"), valueBox(font, sf::Vector2f(pos.x, pos.y + size.y + 4.f), size, "Value"), boxSize(size)
{
    labelBox.setActive(active);
    valueBox.setActive(active);
}

void ComponentInfoDisplay::setLabel(const std::string& label) {
    labelBox.setText(label);
}

void ComponentInfoDisplay::setValue(const std::string& value) {
    valueBox.setText(value);
}

void ComponentInfoDisplay::setPosition(const sf::Vector2f& pos) {
    labelBox.setPosition(pos);
    valueBox.setPosition(sf::Vector2f(pos.x, pos.y + boxSize.y + 4.f));
}

void ComponentInfoDisplay::draw(sf::RenderWindow& window) {
    labelBox.draw(window);
    valueBox.draw(window);
}
