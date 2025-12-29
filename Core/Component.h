#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "SFML/Graphics.hpp"
#include "ElectricalNode.h"
#include "../Config.h"

enum class ComponentType {
    Resistor,
    VoltageSource,
    CurrentSource,
    Capacitor,
    Inductor,
    Switch,
};

class Component {
public:
    int id;
    int nodeA;
    int nodeB;
    ComponentType type;
    double value;


    sf::Vector2f position;
    float rotation;

    double current;
    double voltage;
    bool isClosed = true;

    bool selected;

    WireNodeReference A_WireNodeReference;
    WireNodeReference B_WireNodeReference;

    sf::Vector2f leadOffsetA = { -60.f, 0.f };
    sf::Vector2f leadOffsetB = { 60.f, 0.f };

    Component(int A, int B, const ComponentType& ty, double val = 0);

    void setPosition(const sf::Vector2f& pos);

    void setRotation(float rot);

    void startDrag(const sf::Vector2f& worldPos);

    void dragTo(const sf::Vector2f& worldPos);

    void stopDrag();
};
