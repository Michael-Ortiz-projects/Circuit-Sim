#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include "SFML/Graphics.hpp"

#include "Component.h"
class Circuit {
public:
    Circuit() = default;

    int addComponent(const Component& comp, const sf::Vector2f& canvasPos);
    bool removeComponent(int compID);

    Component* getComponent(int compID);
    std::vector<Component>& getComponents();

private:
    int nextComponentID = 0;
    std::vector<Component> components;
    bool isSimulating = false;
    std::unordered_map<int, int> componentIDToIndex; // componentID -> vector index
};