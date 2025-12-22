#pragma once
#include "SFML/Graphics.hpp"
#include "../Core/Component.h"
#include "../Config.h"
#include "../Debug.h"

class SchematicComponent {
public:
    SchematicComponent(int id, sf::Vector2f pos, float rot, ComponentType t);
    SchematicComponent(const Component& comp);

    void setPosition(const sf::Vector2f& pos);
    void setRotation(float rot);

    void startDrag(const sf::Vector2f& worldPos);
    void dragTo(const sf::Vector2f& worldPos);
    void stopDrag();

    void setTexture(const sf::Texture& texture);
    bool hitBoxContainsPoint(const sf::Vector2f& point);

    const sf::Vector2f& getPosition() const;
    float getRotation() const;
    sf::Sprite& getSprite();
    sf::RectangleShape& getHitBox();
    ComponentType getType() const;

    sf::Vector2f getLeadPositionA() const;
    sf::Vector2f getLeadPositionB() const;

    bool nearLeadA(sf::Vector2f& point);
    bool nearLeadB(sf::Vector2f& point);

    int componentID;
    bool selected = false;

private:
    ComponentType type;
    sf::Vector2f position;
    float rotation;

    sf::Sprite sprite;
    sf::RectangleShape hitBox;

    bool dragging = false;
    sf::Vector2f dragOffset;

    sf::Vector2f leadOffsetA = { -60.f, 0.f };
    sf::Vector2f leadOffsetB = { 60.f, 0.f };
};

