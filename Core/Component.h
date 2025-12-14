#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "SFML/Graphics.hpp"


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
    ComponentType type;
    double value;
    int nodeA;
    int nodeB;
    int identification_number;
    float current;
    float voltage;

    bool isClosed = false;

    Component(int A, int B, const ComponentType& ty, double val);

    Component();

};

