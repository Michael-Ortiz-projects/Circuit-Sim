#pragma once
#include "SFML/Graphics.hpp"
#include "../Core/Component.h"
#include "AssetManager.h"
#include "../Config.h"
#include "../Debug.h"
#include "ComponentInfoDisplay.h"
#include <optional>
class SchematicComponent {
public:
    int componentID;
    bool selected = false;

    WireNodeReference A_WireNodeReference;
    WireNodeReference B_WireNodeReference;    

    SchematicComponent(const Component& comp, sf::Font& font);

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

    sf::Vector2f getLeadPositionA() const;
    sf::Vector2f getLeadPositionB() const;


    bool nearLeadA(sf::Vector2f& point);
    bool nearLeadB(sf::Vector2f& point);

    

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

    sf::Vector2f leadOffsetA = { -60.f, 0.f };
    sf::Vector2f leadOffsetB = { 60.f, 0.f };

    ComponentInfoDisplay display;
};

