#pragma once
#include "SFML/Graphics.hpp"
#include "HelperFunctions.h"
#include "Component.h"
#include "TextBox.h"
#include "Wire.h"
#include "AssetManager.h"
#include "global_variables.h"



class Entity {
public:
    int uniqueID = -1;
    sf::Sprite idle_symbol;
    sf::Sprite highlight_symbol;
    sf::Color NormalColor = sf::Color(220, 220, 235);
    sf::Color SelectColor = sf::Color(255, 200, 0);
    int rotation;
    sf::Vector2f position;
    sf::Vector2f lead1;
    sf::Vector2f lead2;
    sf::RectangleShape selection_box;
    Component component;
    TextBox IDBox;
    TextBox ValueBox;
    std::string input;
    bool selected = false;
    bool deleted = false;

    Entity();

    Entity(Component& comp);
    //Edit this to change from/to wire index when connecting component to existing wire
    void handleMouseEvents(CursorState& cursorState, std::vector<Wire>& wires, std::unordered_map<int, Entity>& entities, sf::Vector2f mousePosition, int entityID);

    void handleTextInput(const sf::Event& event, const sf::Vector2f& mousePos, CursorState& cursorState);

    void setPosition(sf::Vector2f newPosition, std::vector<Wire>& wires, int dr);

    sf::Vector2f getPosition();

    bool contains(sf::Vector2f point);

    void setSelectingID(bool state);

    void setSelectingValue(bool state);

    void draw(sf::RenderWindow& window);

    void printData();
private:
    void setValueText();
};