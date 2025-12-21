#pragma once
#include "SFML/Graphics.hpp"

struct WireSegment {
    int id;
    int electricalNodeID;
    std::vector<sf::Vector2f> points;
};

