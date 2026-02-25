#pragma once
#include "SFML/Graphics.hpp"

inline constexpr int SCR_WIDTH = 2560;
inline constexpr int SCR_HEIGHT = 1440;
inline constexpr int fontSize = 15;
inline constexpr float fontFactor = 2;
inline constexpr float gridSize = 20;
inline constexpr float defaultZoom = .75f;
inline constexpr float minZoom = 0.25f;
inline constexpr float maxZoom = 1.5f;
inline constexpr float nodeSelectionRadius = 10;
inline constexpr float wireSelectionRadius = 10;
inline sf::Color wireDefaultColor = sf::Color(154, 159, 166, 255);
inline sf::Color simulationWindowBackgroundColor = sf::Color(90, 90, 90, 255);
