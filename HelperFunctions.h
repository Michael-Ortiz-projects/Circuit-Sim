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
#include <Eigen/Dense>

enum class CursorState {
    Default,
    CreatingResistor,
    CreatingVoltageSource,
    CreatingCurrentSource,
    CreatingCapacitor,
    CreatingInductor,
    CreatingSwitch,
    PlacingResistor,
    PlacingVoltageSource,
    PlacingCurrentSource,
    PlacingCapacitor,
    PlacingInductor,
    PlacingSwitch,
    CreatingWire,
    PlacingWire,
    SelectingComponent,
    SelectingNode,
    EditingText
};

std::string to_string(CursorState state);

void drawGrid(sf::RenderWindow& window);

bool VectorForwardPairCheck(const std::vector<int>& vector, int x, int y);

bool alphanum_compare_string(const std::string& a, const std::string& b);

int FindStringIndex(std::string target, std::vector<std::string> vector);

void printMatrix(std::vector<std::vector<float>>& matrix);

void rref(std::vector<std::vector<float>>& mat);

sf::Text ValuetoWideStringLiteral(double value, const sf::Font& font, unsigned int size = 30, sf::Color color = sf::Color::White);

std::string doubleToString(double value);

std::string floatToString(float value);

bool isPointNearSegment(sf::Vector2f mouse, sf::Vector2f a, sf::Vector2f b, float max_distance);

sf::Vector2f snapToGrid(const sf::Vector2f& pos);

bool isCollinear(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c);
/*
Colors
Text = sf::Color(223, 208, 184)
ClearColor = sf::Color(34, 40, 49)
normalColor = sf::Color(57, 62, 70)
hoverColor = sf::Color(148, 137, 121)*/