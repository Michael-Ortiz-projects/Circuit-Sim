#pragma once
#include "../UI/TextBox.h"
#include "SchematicComponent.h"
#include "../Debug.h"
#include <string>

struct ElementState {
    std::string label;
    double voltageDelta;
    double value;
    double current;
    ComponentType type;
};

class ElementStateDisplay {
public:
    ElementStateDisplay(sf::Vector2f position, sf::Vector2f size);
    void setState(const ElementState& state, sf::Font& font);
    void setPosition(sf::Vector2f position);
    void draw(sf::RenderWindow& window);
private:
    std::vector<TextBox> textBoxes;
    sf::RectangleShape box;
};