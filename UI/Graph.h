#include "SFML/Graphics.hpp"
#include <iostream>
#include "AssetManager.h"
#include <sstream>
struct GraphBounds {
    float minX, maxX, minY, maxY;
};
class Graph {
public:
    sf::FloatRect selectionRect;
    bool dragging;
    sf::FloatRect drawArea;

    Graph(AssetManager& asset) : font(asset.mainFont) {}
    void setData(const std::vector<sf::Vector2f>& points);

    void setBounds(float minX_, float maxX_, float minY_, float maxY_);

    GraphBounds getBounds();

    void autoScale();

    void setArea(const sf::FloatRect& area, const sf::Vector2u& windowSize);


    bool dataEmpty() { return data.empty(); }

    sf::View& getView() { return view; }

    void draw(sf::RenderWindow& window);
private:
    sf::Font& font;
    std::vector<sf::Vector2f> data;

    float minX = -1.f, maxX = 1.f;
    float minY = -1.f, maxY = 1.f;

    int tickCountX, tickCountY;
    float firstX, firstY, spacingX, spacingY;

    sf::View view;
    
    void updateView();

    float niceFraction(float value);

    void drawData(sf::RenderTarget& target);

    void drawGrid(sf::RenderTarget& target);

    void drawAxes(sf::RenderTarget& target);

    void drawBorder(sf::RenderTarget& target);

    void drawLabels(sf::RenderTarget& target);

    void drawSelectionBox(sf::RenderTarget& target);
};