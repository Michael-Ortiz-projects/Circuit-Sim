#include "Grid.h"

Grid::Grid(float spacing) {
    gridSpacing = spacing;
}

void Grid::setSpacing(float spacing) {
    gridSpacing = spacing;
}

void Grid::draw(sf::RenderWindow& window, const sf::View& view) {
    window.clear(clearColor);

    sf::Vector2f BottomLeftWindow = window.mapPixelToCoords(sf::Vector2i(0, 0), view);
    sf::Vector2f TopRightWindow = window.mapPixelToCoords(sf::Vector2i(window.getSize().x, window.getSize().y), view);

    // Draw vertical grid lines
    for (float x = std::floor(BottomLeftWindow.x / gridSpacing) * gridSpacing; x <= TopRightWindow.x; x += gridSpacing) {
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(x, BottomLeftWindow.y), gridColor),
            sf::Vertex(sf::Vector2f(x, TopRightWindow.y), gridColor)
        };
        window.draw(line, 2, sf::Lines);
    }

    // Draw horizontal grid lines
    float yStart = std::floor(BottomLeftWindow.y / gridSpacing) * gridSpacing;
    float yEnd = std::ceil(TopRightWindow.y / gridSpacing) * gridSpacing;

    for (float y = yStart; y <= yEnd; y += gridSpacing) {
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(BottomLeftWindow.x, y), gridColor),
            sf::Vertex(sf::Vector2f(TopRightWindow.x, y), gridColor)
        };
        window.draw(line, 2, sf::Lines);
    }
}