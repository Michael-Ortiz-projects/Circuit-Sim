#include "ComponentInfoDisplay.h"
#include <sstream>
#include <iomanip>
ComponentInfoDisplay::ComponentInfoDisplay(sf::Font& font, const sf::Vector2f& pos, const sf::Vector2f& size, int textSize, bool active)
    : labelBox(font, pos, size, textSize, "Label", false), valueBox(font, sf::Vector2f(pos.x, pos.y + size.y + 4.f), size, textSize, "Value", false), boxSize(size)
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
    valueBox.setPosition(sf::Vector2f(pos.x, pos.y + boxSize.y));
}

void ComponentInfoDisplay::centerAt(const sf::Vector2f position) {
    float labelOffset = labelBox.getTextBoundsRect().getGlobalBounds().width/2;
    float valueOffset = valueBox.getTextBoundsRect().getGlobalBounds().width/2;
    labelBox.setPosition(sf::Vector2f(position.x - labelOffset, position.y));
    valueBox.setPosition(sf::Vector2f(position.x - valueOffset, position.y + boxSize.y));
}


void ComponentInfoDisplay::draw(sf::RenderWindow& window) {
    labelBox.draw(window);
    valueBox.draw(window);
}
