#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <cmath>
#include "SFML/Graphics.hpp"
#include <memory>
#include <functional>
#include <utility>
#include <sstream>
#include "TextBox.h"
#include "HelperFunctions.h"

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
    std::string label;
    ComponentType type;
    double value;
    int fromNode;
    int toNode;
    int identification_number;
    float current;
    float voltage;

    bool isClosed = false;           
    sf::Keyboard::Key controlKey;

    bool operator==(const Component& other) const {
        return label == other.label && identification_number == other.identification_number;
    }

    Component(int from, int to, const ComponentType& ty, std::string label, double val);

    Component();


    void printData(TextBox& detailBox, bool updateBox);

};

struct LoopComponent {
    Component comp;
    bool forward;

    LoopComponent(const Component& c, bool direction);
};

std::string to_string(ComponentType type);

bool alphanum_compare_component(const Component& x, const Component& y);