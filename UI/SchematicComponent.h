#pragma once
#include "SFML/Graphics.hpp"
#include "../Core/NetlistComponent.h"
#include "AssetManager.h"
#include "../Config.h"
#include "../Debug.h"
#include "ComponentInfoDisplay.h"
#include <optional>

struct SchematicTerminal {
    int terminalID;
    sf::Vector2f offset;
    WireNodeReference wireNodeReference;
};

class SchematicComponent {
public:
    int componentID;
    bool selected = false;
    std::vector<SchematicTerminal> schematicTerminals;

    SchematicComponent(const NetlistComponent& comp, sf::Vector2f canvasPos, sf::Font& font);

    void setPosition(const sf::Vector2f& pos);
    void setRotation(float rot);
    void setLabel(const std::string string);
    void setValue(double val);

    void startDrag(const sf::Vector2f& worldPos);
    void dragTo(const sf::Vector2f& worldPos);
    void stopDrag();

    void setTexture(const sf::Texture& texture);
    bool hitBoxContainsPoint(const sf::Vector2f& point);

    void updateDisplay();

    const sf::Vector2f& getPosition() const;
    float getRotation() const;
    sf::Sprite& getSprite();
    sf::RectangleShape& getHitBox();
    ComponentType getType() const;
    ComponentInfoDisplay& getComponentInfoDisplay();
    std::string& getLabel() { return label; }
    double& getValue() { return value; }


    

private:
    ComponentType type;
    std::string label;
    double value;
    sf::Vector2f position;
    float rotation;

    sf::Sprite sprite;
    sf::RectangleShape hitBox;

    bool dragging = false;
    sf::Vector2f dragOffset;


    ComponentInfoDisplay display;
    sf::Vector2f displayOffset;
};

