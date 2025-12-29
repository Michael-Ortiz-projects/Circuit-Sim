#include "Grid.h"

Grid::Grid(float spacing) {
    gridSpacing = spacing;
}

void Grid::setSpacing(float spacing) {
    gridSpacing = spacing;
}

void Grid::draw(sf::RenderWindow& window, const sf::View& view)
{
    window.clear(clearColor);

    sf::Vector2f bottomLeft =
        window.mapPixelToCoords({ 0, 0 }, view);
    sf::Vector2f topRight =
        window.mapPixelToCoords(
            { (int)window.getSize().x, (int)window.getSize().y }, view);

    float startX = std::floor(bottomLeft.x / gridSpacing) * gridSpacing;
    float startY = std::floor(bottomLeft.y / gridSpacing) * gridSpacing;
    float endX = std::ceil(topRight.x / gridSpacing) * gridSpacing;
    float endY = std::ceil(topRight.y / gridSpacing) * gridSpacing;

    float halfSize = gridSpacing * 0.20f;

    sf::VertexArray crosses(sf::Lines);

    for (float x = startX; x <= endX; x += gridSpacing)
    {
        for (float y = startY; y <= endY; y += gridSpacing)
        {
            crosses.append(sf::Vertex({ x - halfSize, y }, gridColor));
            crosses.append(sf::Vertex({ x + halfSize, y }, gridColor));

            crosses.append(sf::Vertex({ x, y - halfSize }, gridColor));
            crosses.append(sf::Vertex({ x, y + halfSize }, gridColor));
        }
    }

    window.draw(crosses);
}
